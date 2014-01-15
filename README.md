Overview
========

This repository contains the source code for the N-way sweep and slice
algorithms developed in the Quinlan lab at the University of Virginia.

Installation
============
Installation requires the tcmalloc library
(http://goog-perftools.sourceforge.net/doc/tcmalloc.html), which is part of the
gperftools package.  This package can be installed on most Linux-based
operating systems through the associated package management tool:

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

The number of threads to use for the parallel algorithms.  This option is ignored for sequential algorithms.

    -b  bed files

The set of sorted bed files to consider.  Wild card characters are uses here to
include multiple files.

    -g  genome file

The genome file defines the chromosome order and size.  It is required when
considering bed files

    -f  file name

A file of interval sets where each line contains one interval set.  Within each
line, interval start and end positions are space-delimited and the intervals
are tab-delimited.

    -n  number of sets

The number of interval sets in the simulated sets.

    -i  number of intervals per set

The number of interval per set in the simulated sets.

    -I  number of intersections

The number of N-way intersections in the simulated sets.  This number cannot be
larger than the the number of intervals

    -l  interval length

The length of intervals in the simulated sets.

    -r  range

The maximum interval end position in simulated sets.


    -p  to print set

Defines the output format.  1 prints indices, 2 prints interval coordinates,
and 3 prints the coordinates of the region common to the intervals.  If -p is not given, then the output is the algorithm runtime.

    -s  random seed

The simulation seed.


Test simulated data sets
===================
To test runtime on simulated data sets use the following options: number of
sets, number of intervals per set, number of intersections, interval length,
range.

Example:

    $ bin/psplit_centers -t 10 -n 100 -i 1000 -r 1000000000 -l 10 -I 1000
    8081 

    $ bin/sweep -t 10 -n 10 -i 1000 -r 1000000000 -l 10 -I 100 -p 1
    3   2   2   1   4   0   0   4   2   0
    40  29  33  30  32  34  26  27  41  40
    57  45  52  42  47  44  36  43  58  49
    63  48  59  49  53  49  41  51  63  57
    66  57  64  58  57  54  47  53  73  63
    ...

    $ bin/split_o -n 10 -i 1000 -r 1000000000 -l 10 -I 1000 -p 2
    372552,372562   372549,372559   372548,372558   372553,372563   372552,372562   372546,372556   372550,372560   372553,372563   372547,372557   372548,372558
    419801,419811   419798,419808   419804,419814   419803,419813   419797,419807   419797,419807   419799,419809   419804,419814   419802,419812   419805,419815
    1089429,1089439 1089432,1089442 1089432,1089442 1089438,1089448 1089437,1089447 1089435,1089445 1089438,1089448 1089429,1089439 1089438,1089448 1089432,1089442
    1100541,1100551 1100536,1100546 1100543,1100553 1100538,1100548 1100544,1100554 1100545,1100555 1100541,1100551 1100545,1100555 1100543,1100553 1100538,1100548
    1229903,1229913 1229897,1229907 1229904,1229914 1229903,1229913 1229898,1229908 1229901,1229911 1229898,1229908 1229903,1229913 1229899,1229909 1229902,1229912
    ...

    $ bin/split_sweep -n 10 -i 1000 -r 1000000000 -l 10 -I 1000 -p 3
    372553  372556
    419805  419807
    1089438 1089439
    1100545 1100546
    1229904 1229907
    ...


Test existing data sets
==================
To intersect bed files, a genome file defining the chromosome order and size
must be provided (an example genome file is given in genome/hg19.genome).  A
sample of the DnaseI files from Maurano et al. (DOI: 10.1126/science.1222794)
can be found at http://www.cs.virginia.edu/~rl6sf/Dnase1_sample.tar.gz.

    $ wget http://www.cs.virginia.edu/~rl6sf/Dnase1_sample.tar.gz

    $ tar -zxvf Dnase1_sample.tar.gz

    $ bin/psplit_centers -t 10 -g genomes/hg19.genome -b "Dnase1_sample/*"
    42874

    $ bin/sweep -g genomes/hg19.genome -b "Dnase1_sample/*" -p 2
    235634,235750   235651,235765   235640,235770   235634,235741   235635,235763
    237718,237910   237723,237787   237720,237782   237720,237846   237717,237786
    569811,570041   569786,570041   569750,570094   569808,570066   569581,570074
    713930,714650   713909,714641   713895,714610   713945,714473   713887,714344
    752463,752765   752598,752772   752462,752776   752496,752759   752464,752766
    762549,763150   762490,763142   762439,763148   762555,763148   762579,763150
    839790,840279   839785,842534   839888,840367   839947,840341   839897,840311
    840629,841869   839785,842534   840575,841771   840583,841175   840552,841786
    840629,841869   839785,842534   840575,841771   841407,841918   840552,841786
    846254,846948   846092,847046   846477,846980   846515,846950   846453,847029
    ...
