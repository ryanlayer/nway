#!/usr/bin/env python
import sys
import math
import numpy as np
import matplotlib
import pylab
import random
from optparse import OptionParser
import operator


class Data:
    def __init__ (self, A, op):
        self.app = A[0]
        self.threads = int(A[1])
        self.sample = A[2]
        self.size = int(A[3])

        if op == 'mean':
            self.time = np.mean(\
                    [int(x) for x in A[4].split(',') if x.isdigit()])

        if op == 'min':
            self.time = min(\
                    [int(x) for x in A[4].split(',') if x.isdigit()])

        if op == 'max':
            self.time = max(\
                    [int(x) for x in A[4].split(',') if x.isdigit()])


    def get_name(self):
        return self.app + ", " + str(self.threads)

parser = OptionParser()
parser.add_option("-d",
                  "--data_file",
                  dest="data_file",
                  help="Data file")

parser.add_option("-o",
                  "--out_file",
                  dest="out_file",
                  help="Output file")

parser.add_option("-t",
                  "--title",
                  dest="title",
                  help="Output file")

parser.add_option("--op",
                  default="mean",
                  dest="data_op",
                  help="Data opperation")

parser.add_option("--ymax",
                  default=6100000,
                  type="int",
                  dest="ymax",
                  help="Max Y value")



(options, args) = parser.parse_args()

if not options.data_file:
    parser.error('Data file not given')

if not options.out_file:
    parser.error('Ouput file not given')

if not options.title:
    parser.error('Title not given')

f = open(options.data_file,'r')

R={}
O=[]

for l in f:
    data = Data(l.rstrip().split('\t'),options.data_op)

    if not data.app in R:
        R[data.app] = {}

    if not data.sample in R[data.app]:
        R[data.app][data.sample] = []

    R[data.app][data.sample].append(data)

    if not data.sample in O:
        O.append(data.sample)

f.close()

matplotlib.rcParams.update({'font.size': 10})
fig = matplotlib.pyplot.figure(figsize=(5,1),dpi=300)
fig.subplots_adjust(wspace=.05,left=.01,bottom=0,top=0.7)
ax = fig.add_subplot(1,1,1)


#app_list =['split_o','split_sweep','psplit_centers','split_psweep']
#app_list =['sweep','split_o','split_sweep','psplit_centers','split_psweep']
app_list =['sweep','split_o','split_sweep','psplit_centers']

markers={'sweep':'s',
         'split_o':'o',
         'split_sweep':'^',
         'psplit_centers':'+',
         'split_psweep':'x'}

fills={'sweep':'none',
       'split_o':'none',
       'split_sweep':'none',
       'psplit_centers':'full',
       'split_psweep':'full'}

colors={1:'black',4:'blue',8:'red',16:'green'}

baseline='sweep'

lables=[]
plots=[]
plots_seen=[]
for app in app_list:
    print app
    i = 0
    lables = []
    for sample in O:
        lables.append(R[app][sample][0].size)
        brun = R[baseline][sample] 
        for run in R[app][sample]:
            print app,run.threads
            if run.threads in colors:
                p,=ax.plot([i],
                        [math.log(brun[0].time/run.time,2)],\
                        #[brun[0].time/run.time],\
                        ".-",\
                        linewidth=3,\
                        markersize=4,\
                        marker=markers[app],\
                        fillstyle=fills[app], \
                        color=colors[run.threads])
                if not run.get_name() in plots_seen:
                    print run.get_name()
                    plots.append(p)
                    plots_seen.append(run.get_name())
        i+=1

print lables

#ax.set_xticks(range(len(lables)))
#ax.set_xticklabels(lables,rotation=270)
ax.set_xticks([])
ax.set_xticklabels([])
ax.set_xlim([-1,i])
#ax.set_ylim([0,300000])
#ax.set_ylim([0,options.ymax])
#ax.set_ylabel('Run Time(mircroseconds)')
#ax.set_xlabel('Number of sets')
#ax.set_title(options.title)
#ax.legend(plots, plots_seen,loc=0,numpoints=1,ncol=2,
    #prop={'size':8},
    #frameon=False)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
ax.spines['bottom'].set_linewidth(0.5)
ax.spines['left'].set_linewidth(0.5)

ax.yaxis.tick_left()

ax.yaxis.grid(b=True, which='major', color = '0.75', linestyle='--')
ax.set_axisbelow(True)
#ax.set_yticklabels([])

matplotlib.pyplot.savefig(options.out_file,bbox_inches='tight')

