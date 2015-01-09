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

if not options.titles:
    parser.error('Titles not given')

data_files = options.data_files.split(',')
titles = options.titles.split(',')

D = {}

if len(data_files) != len(titles):
    parser.error('Number of data files and titles do not match')

num_plots = len(data_files)

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


    #app_list =['split_o','split_sweep','psplit_centers','split_psweep']
    #app_list =['sweep','split_o','split_sweep','psplit_centers','split_psweep']
    #app_list =['sweep','split_o','split_sweep','psplit_centers','split_psweep']
    app_list =['sweep','split_o','split_sweep','psplit_centers']
    thread_list = [1,4,8,16]

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

    lables=[]
    plots=[]
    plots_seen=[]
    for app in app_list:
        i = 0
        lables = []
        for sample in O:
            N,M,I = [int(x) for x in sample.split(":")]
            for run in R[app][sample]:
                if run.threads in thread_list:

                    if not I in D:
                        D[I] = {}

                    if not app in D[I]:
                        D[I][app] = {}

                    if not run.threads in D[I][app]:
                        D[I][app][run.threads] = {}

                    D[I][app][run.threads][N] = run.time
#                    p,=ax.plot([i],[run.time],'.-',\
#                            linewidth=3,\
#                            marker=markers[app],\
#                            color=colors[run.threads], \
#                            fillstyle=fills[app], \
#                            markersize=4)
#                    if not run.get_name() in plots_seen:
#                        print run.get_name()
#                        plots.append(p)
#                        plots_seen.append(run.get_name())


app_order =['sweep','split_o','split_sweep','psplit_centers']

for I in sorted(D.keys()):
    print I
    for app in app_order:
        for threads in sorted(D[I][app].keys()):
            print app,threads,
            for N in sorted(D[I][app][threads].keys()):
                print D[I][app][threads][N],
            print

