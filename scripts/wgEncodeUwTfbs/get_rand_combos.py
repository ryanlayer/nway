#!/usr/bin/env python
import sys
import numpy as np
import os
import random
import subprocess

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-d",
    "--data_dir",
    dest="data_dir",
    help="Data directory")

parser.add_option("-o",
    "--out_file",
    dest="out_file",
    help="Output file")


parser.add_option("-n",
    "--num",
    type="int",
    dest="num",
    help="Number of files to include directory")


(options, args) = parser.parse_args()

if not options.data_dir:
    parser.error('Data directory not given')

if not options.out_file:
    parser.error('Ouput file not given')

if not options.num:
    parser.error('File num not given')

F=[]
for file in os.listdir(options.data_dir):
    F.append(options.data_dir + '/' + file)

rand_choice = random.sample(F, options.num)
#for f in random.sample(F.keys(),options.num):
    #rand_choice.append(random.choice(F[f]))

os.system('cat ' + ' '.join(rand_choice) + '>' + options.out_file)
