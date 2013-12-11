#!/usr/bin/env python
import sys
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
                  "--data_files",
                  dest="data_files",
                  help="Comma seperated data files")

parser.add_option("-o",
                  "--out_file",
                  dest="out_file",
                  help="Output file")

parser.add_option("-t",
                  "--titles",
                  dest="titles",
                  help="Plot Titles")

parser.add_option("--op",
                  default="mean",
                  dest="data_op",
                  help="Data opperation")


(options, args) = parser.parse_args()

if not options.data_files:
    parser.error('Data files not given')

if not options.out_file:
    parser.error('Ouput file not given')

if not options.titles:
    parser.error('Titles not given')

data_files = options.data_files.split(',')
titles = options.titles.split(',')


if len(data_files) != len(titles):
    parser.error('Number of data files and titles do not match')

num_plots = len(data_files)

matplotlib.rcParams.update({'font.size': 10})
fig = matplotlib.pyplot.figure(figsize=(5,1*num_plots),dpi=300)
fig.subplots_adjust(wspace=.05,left=.01,bottom=0,top=0.7)

for n in range(len(data_files)):
    data_file = data_files[n]
    title = titles[n]

    print data_file,n

    f = open(data_file,'r')

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

    ax = fig.add_subplot(num_plots,1,n+1)


    #app_list =['split_o','split_sweep','psplit_centers','split_psweep']
    #app_list =['sweep','split_o','split_sweep','psplit_centers','split_psweep']
    #app_list =['sweep','split_o','split_sweep','psplit_centers','split_psweep']
    app_list =['sweep','split_sweep','psplit_centers','split_psweep']

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



    colors={1:'black',10:'blue',20:'red',30:'green'}

    lables=[]
    plots=[]
    plots_seen=[]
    for app in app_list:
        i = 0
        lables = []
        for sample in O:
            lables.append(R[app][sample][0].size)
            for run in R[app][sample]:
                p,=ax.plot([i],[run.time],'.-',\
                        linewidth=3,\
                        marker=markers[app],\
                        color=colors[run.threads], \
                        fillstyle=fills[app], \
                        markersize=4)
                if not run.get_name() in plots_seen:
                    print run.get_name()
                    plots.append(p)
                    plots_seen.append(run.get_name())
            i+=1

    print lables

    #if n == num_plots -1:
    #    ax.set_xticks(range(len(lables)))
    #    ax.set_xticklabels(lables,rotation=270)
    #    ax.set_xlabel('Number of sets')
    #else:
    ax.set_xticks([])
    ax.set_xticklabels([])
    #for spine in ax.spines.itervalues():
        #print spine
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['bottom'].set_linewidth(0.5)
    ax.spines['left'].set_linewidth(0.5)

    ax.yaxis.tick_left()

    ax.set_yscale('log')
    ax.set_xlim([-1,i])
    #ax.set_ylim([1000,19000000])
    #ax.set_ylim([1000,1900000])
    ax.set_ylim([19000,1900000])
    ax.yaxis.grid(b=True, which='major', color = '0.75', linestyle='--')
    ax.set_axisbelow(True)
    ax.set_yticklabels([])

    #ax.set_ylabel('Run Time(mircroseconds)')
    #ax.set_title(title)
    #ax.legend(plots, plots_seen,loc=0,numpoints=1,ncol=2,
        #prop={'size':8},
        #frameon=False)

matplotlib.pyplot.savefig(options.out_file,bbox_inches='tight')

