#!/usr/bin/env python
import sys
import numpy as np
import os
import random
import subprocess

from optparse import OptionParser

class Data:
    def __init__(self,A):
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
    "--out_file",
    dest="out_file",
    help="Output file")


parser.add_option("-n",
    "--num",
    type="int",
    dest="num",
    help="Number of files to include directory")

parser.add_option("-f",
    "--files",
    dest="files",
    help="Files directory")

(options, args) = parser.parse_args()

if not options.data_dir:
    parser.error('Data directory not given')

if not options.out_file:
    parser.error('Ouput file not given')

if not options.num:
    parser.error('File num not given')

if not options.files:
    parser.error('File directory not given')

f = open(options.files,'r')

files = {}

for l in f:
    A = l.rstrip().split('\t')
    if 'broadPeak' in A[0]:
        #set_name = A[0][:-3] + '.set'
        b=Data(A)
        print b.cell, b.antibody

f.close()

#combos=['fSkin_fibro_abdomen', \
#        'fSkin_fibro_back', \
#        'fSkin_fibro_bicep_L', \
#        'fSkin_fibro_bicep_R', \
#        'fSkin_fibro_leg_L_quad', \
#        'fSkin_fibro_leg_R_quad', \
#        'fSkin_fibro_scalp', \
#        'fSkin_fibro_upper_back', \
#        'fKidney_renal_cortex_R', \
#        'fMuscle_trunk', \
#        'fSpinal_cord', \
#        'fKidney_renal_cortex_L', \
#        'fKidney_renal_pelvis_L', \
#        'fKidney_renal_pelvis_R', \
#        'fPlacenta', \
#        'fAdrenal', \
#        'fKidney_L', \
#        'fKidney_R', \
#        'fKidney', \
#        'fKidney_renal_pelvis', \
#        'fKidney_renal_cortex', \
#        'fLung_R', \
#        'fThymus', \
#        'fStomach', \
#        'fBrain', \
#        'fHeart', \
#        'fLung', \
#        'fLung_L', \
#        'fMuscle_back', \
#        'fMuscle_leg', \
#        'fIntestine_Sm', \
#        'fIntestine_Lg', \
#        'fMuscle_arm'] 
#F={}
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
