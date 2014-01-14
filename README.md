Overview
========

This repository contains the source code for the N-way sweep and slice
algorithms developed in the Quinlan lab at the University of Virginia.

Installation
============
Installation requires the tcmalloc library
(http://goog-perftools.sourceforge.net/doc/tcmalloc.html), which is part of the
gperftools package.  This package can be installed on most operating systems
through the associated package management tool:

- OS X: `brew install gperftools`
- RedHat Linux: `yum install google-perftools-devel`
- Ubuntu Linux: `apt-get install libgoogle-perftools-dev google-perftools`

Otherwise, gperftools can be install from source at
https://code.google.com/p/gperftools/downloads/list

Compile N-way algorithms
::

    cd nway
    make

The executable names that are associated with the algorithms are:
sweep                   bin/sweep
split                   bin/split\_o
split sweep             bin/split\_sweep
parallel split sweep    bin/psplit\_centers

Usage
=====
All of the executables have the same command line parameters:

    -t  number of threads

The number of threads to use for the parallel algorithms.

::

    -b  bed files

The set of sorted bed files to consider.  Wild card characters are uses here to
include multiple files.

::
    -g  genome file

The genome file defines the chromosome order and size.  It is required when
considering bed files

::
    -f  file name

A file of interval sets where each line contains one interval set.  Lines
contain an interval were the start and end positions are space-delimited and
the intervals are tab-delimited.

::
    -n  number of sets

The number of interval sets in the simulated sets.

::
    -i  number of intervals per set

The number of interval per set in the simulated sets.

::
    -I  number of intersections

The number of N-way intersections in the simulated sets.  This number cannot be
larger than the the number of intervals

::
    -l  interval length

The length of intervals in the simulated sets.

::
    -r  range

The maximum interval end position in simulated sets.


::
    -p  to print set

Defines the output format.  1 prints indices, 2 prints interval coordinates,
and 3 prints the coordinates of the region common to the intervals.  If -p is not given, then the output is the algorithm runtime.

::
    -s  random seed

The simulation seed.


Test simulated data sets
===================
To test runtime on simulated data sets use the following options: number of
sets, number of intervals per set, number of intersections, interval length,
range.

Example:

    bin/psplit_centers -t 10 -n 100 -i 1000 -r 1000000000 -l 10 -I 1000
    bin/sweep -t 10 -n 10 -i 1000 -r 1000000000 -l 10 -I 100 -p 1
    bin/split_o -n 10 -i 1000 -r 1000000000 -l 10 -I 1000 -p 2


Test existing data sets
==================
To intersect bed files, a genome file defining the chromosome order and size
must be provided (an example genome file is given in genome/hg19.genome).  A
sample of the DnaseI files from Maurano (DOI: 10.1126/science.1222794) can be
found at http://www.cs.virginia.edu/~rl6sf/Dnase1_sample.tar.gz.

    wget http://www.cs.virginia.edu/~rl6sf/Dnase1_sample.tar.gz
    tar -zxvf Dnase1_sample.tar.gz
    bin/psplit_centers -t 10 -g genomes/hg19.genome -b "Dnase1_sample/*"
    bin/sweep -g genomes/hg19.genome -b "Dnase1_sample/*" -p 2

    
