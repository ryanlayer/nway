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
                  "--data_file",
                  dest="data_file",
                  help="Data file")

parser.add_option("--op",
                  default="mean",
                  dest="data_op",
                  help="Data opperation")

(options, args) = parser.parse_args()

if not options.data_file:
    parser.error('Data file not given')

f = open(options.data_file,'r')

D = {}

for l in f:
    data = Data(l.rstrip().split('\t'),options.data_op)

    if 'combo' in data.sample:
        N = int(data.sample.split('.')[4])
    else:
        N = [int(x) for x in data.sample.split('.')[1:]][0]

    if not data.app in D:
        D[data.app] = {}

    if not data.threads in D[data.app]:
        D[data.app][data.threads] = {}

    D[data.app][data.threads][N] = data.time

f.close()

app_order =['sweep','split_o','split_sweep','psplit_centers']

for app in app_order:
    for threads in sorted(D[app].keys()):
        print app,threads,
        for N in sorted(D[app][threads].keys()):
            print D[app][threads][N],
        print

