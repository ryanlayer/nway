#!/bin/bash

APPS=`ls ~/src/nway/bin/*`

for N in $(seq 10 5 50)
do
    OUT=""
    for APP in $APPS
    do
        R=`$APP -n $N -i 10000 -l 10 -r 1000000 -I 100| \
            awk -v f=$file '{
                OFS="\t";
                split($0,a,"\t");
                for (i=1;i<=length(a);++i) {
                    if (substr(a[i],1,5) == "total") {
                        split(a[i],b,":");
                        print b[2],f;
                    }
                }
            }'`

        NAME=`basename $APP`

        OUT="$OUT$R\t$NAME\t"
    done
    echo -e "$N\t10000\t10\t100\t$OUT"
done
