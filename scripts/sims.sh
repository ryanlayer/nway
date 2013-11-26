#!/bin/bash

#APPS=`ls ~/src/nway/bin/*`
P="~/src/nway/bin/"

APPS="psplit_centers
split
split_centers
split_o
split_sweep
sweep"

for APP in $APPS
do
    echo $APP
    for N in $(seq 5 5 20)
    do
        o=""
        for i in $(seq 1 10)
        do
        R=`bin/$APP -n $N -i 10000 -l 10 -r 10000000 -I 1000 -t 10 -s $i`
#        R=`bin/$APP -n $N -i 10000 -l 10 -r 10000000 -I 1000 -t 4 -s 1|
#            awk '{
#                OFS="\t";
#                split($0,a,"\t");
#                for (i=1;i<=length(a);++i) {
#                    if (substr(a[i],1,5) == "total") {
#                        split(a[i],b,":");
#                        print b[2];
#                    }
#                }
#            }'`
        if [ "$o" != "" ]
        then
            o="$o\t$R"
        else
            o="$R"
        fi
        done
        echo -e $o
        #S=`echo -e "$o" | ~/src/tools/analysis/stats --min`
        #echo $S
    done
    echo 
done

#for N in $(seq 5 5 30)
#do
#    OUT=""
#    for APP in $APPS
#    do
#        R=`$P$APP -n $N -i 10000 -l 10 -r 10000000 -I 1000 -t 4| \
#            awk -v f=$APP '{
#                OFS="\t";
#                split($0,a,"\t");
#                for (i=1;i<=length(a);++i) {
#                    if (substr(a[i],1,5) == "total") {
#                        split(a[i],b,":");
#                        print b[2],f;
#                    }
#                }
#            }'`
#
#        NAME=`basename $APP`
#
#        OUT="$OUT$R\t$NAME\t"
#    done
#    echo -e "$N\t10000\t10\t100\t$OUT"
#done
