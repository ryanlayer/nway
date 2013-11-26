#!/usr/bin/env python
# for each antibody, we want a different cell type 
import re
import sys
import numpy as np
import os
import random
import subprocess

from optparse import OptionParser

class Data:
    def __init__(self,A):
        self.set_name = A[0][:-13] + '.set'
        for a in A[1].split(';'):
            field,value = a.split('=')
            if 'cell' in field:
                self.cell = value
            if 'antibody' in field:
                self.antibody = value



parser = OptionParser()

parser.add_option("-d",
    "--data_dir",
    dest="data_dir",
    help="Data directory")

parser.add_option("-o",
    "--out_dir",
    dest="out_dir",
    help="Output dir")


parser.add_option("-f",
    "--files",
    dest="files",
    help="Files directory")

(options, args) = parser.parse_args()

if not options.data_dir:
    parser.error('Data directory not given')

if not options.files:
    parser.error('File directory not given')

if not options.out_dir:
    parser.error('Output directory not given')

f = open(options.files,'r')

cell_types = {}

for l in f:
    A = l.rstrip().split('\t')
    if 'broadPeak' in A[0]:
        b=Data(A)

        if not b.cell in cell_types:
            cell_types[b.cell] = {}

        if not b.antibody in cell_types[b.cell]:
            cell_types[b.cell][b.antibody] = []

        cell_types[b.cell][b.antibody].append(b)
f.close()

cell_type_counts={}
for cell_type in cell_types:
    l = len(cell_types[cell_type])
    if not l in cell_type_counts:
        cell_type_counts[l] = []
    cell_type_counts[l].append(cell_type)

possible_cell_types = []
for count in sorted(cell_type_counts.keys()):
    if count > 1:
        for a in cell_type_counts[count]:
            possible_cell_types.append(a)

for cell_type in possible_cell_types:
    rand_choice = []
    print options.out_dir + '/' + cell_type + '.set'
    for antibody in cell_types[cell_type]:
        r = random.choice(cell_types[cell_type][antibody])
        rand_choice.append(options.data_dir + '/' + r.set_name)

    cmd = 'cat ' + \
              ' '.join(rand_choice) + \
              ' > ' + options.out_dir + '/' + cell_type + '.set'
    os.system(cmd)
