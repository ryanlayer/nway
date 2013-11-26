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

antibodies = {}

for l in f:
    A = l.rstrip().split('\t')
    if 'broadPeak' in A[0]:
        b=Data(A)

        if not b.antibody in antibodies:
            antibodies[b.antibody] = {}

        if not b.cell in antibodies[b.antibody]:
            antibodies[b.antibody][b.cell] = []

        antibodies[b.antibody][b.cell].append(b)
f.close()

antibody_cell_counts = {}

for antibody in antibodies:
    l = len(antibodies[antibody])
    if not l in antibody_cell_counts:
        antibody_cell_counts[l] = []
    antibody_cell_counts[l].append(antibody)

possible_antibodies = []

for count in sorted(antibody_cell_counts.keys()):
    if count > 1:
        for a in antibody_cell_counts[count]:
            possible_antibodies.append(a)

for antibody in possible_antibodies:
    antibody_pretty = re.sub(r"\(", "-", antibody)
    antibody_pretty = re.sub(r"\)", "-", antibody_pretty)
    rand_choice = []
    print options.out_dir + '/' + antibody_pretty + '.set'
    for cell_type in antibodies[antibody]:
        r = random.choice(antibodies[antibody][cell_type])
        rand_choice.append(options.data_dir + '/' + r.set_name)

    cmd = 'cat ' + \
              ' '.join(rand_choice) + \
              ' > ' + options.out_dir + '/' + antibody_pretty + '.set'
    os.system(cmd)
#rand_antibody = random.choice(possible_antibodies)
#print rand_antibody

#rand_antibody = random.sample(antibodies.keys(), options.num)

#for rand_antibody in rand_antibodies:



#for file in os.listdir(options.data_dir):
#    prefix = file.split('-')[0]
#    if prefix in combos:
#        if not prefix in F:
#            F[prefix] = []
#        F[prefix].append(options.data_dir + '/' + file)
#
#rand_choice = []
#for f in random.sample(F.keys(),options.num):
#    rand_choice.append(random.choice(F[f]))
#
#os.system('cat ' + ' '.join(rand_choice) + '>' + options.out_file)
