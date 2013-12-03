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
    "--out_prefix",
    dest="out_prefix",
    help="Output prefix")

parser.add_option("-f",
    "--from",
    type="int",
    dest="from_v",
    help="Max rand set size")

parser.add_option("-t",
    "--to",
    type="int",
    dest="to_v",
    help="Min rand set size")

parser.add_option("-s",
    "--step",
    type="int",
    dest="step_v",
    help="Step size")

(options, args) = parser.parse_args()

if not options.data_dir:
    parser.error('Data directory not given')

if not options.out_prefix:
    parser.error('Ouput prefix not given')

if not options.from_v:
    parser.error('From not given')

if not options.to_v:
    parser.error('To not given')

if not options.step_v:
    parser.error('Step not given')


set = [options.data_dir + '/' + x for x in os.listdir(options.data_dir)]


for i in range(options.to_v, options.from_v-1,-1*options.step_v):
    set_n = random.sample(set,i)
    set = set_n
    cmd = 'cat ' + \
              ' '.join(set) + \
              ' > ' + \
              options.out_prefix + \
                '.f' + str(options.from_v) + \
                '.t' + str(options.to_v) + \
                '.s' + str(options.step_v) + \
                '.' + str(i) + \
                '.set' 
    print cmd
    os.system(cmd)
#    os.system('cat ' + \
#              ' '.join(set) + \
#              '>' + \
#              options.out_prefix + \
#                '.f' + str(options.from_v) +
#                '.t' + str(options.from_t) +
#                '.s' + str(options.step_t) +
#                '.' + str(i) +
#                '.set' )
