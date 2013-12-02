#!/bin/bash

############################################################
#  Program:
#  Author :
############################################################


## BEGIN SCRIPT
usage()
{
    cat << EOF

usage: $0 OPTIONS

OPTIONS can be:
    -h      Show this message
    -t      File containting path to files to test

EOF
}

# Show usage when there are no arguments.
if test -z "$1"
then
    usage
    exit
fi

VERBOSE=
FILENAME=

# Check options passed in.
while getopts "h t:" OPTION
do
    case $OPTION in
        h)
            usage
            exit 1
            ;;
        t)
            FILENAME=$OPTARG
            ;;
        ?)
            usage
            exit
            ;;
    esac
done

COMBOS=`cat $FILENAME`

APPS_TS="
sweep,1
split_o,1
split_sweep,1
psplit_centers,10
psplit_centers,20
psplit_centers,30
split_psweep,10
split_psweep,20
split_psweep,30
"


for COMBO in $COMBOS
do
    F=$COMBO
    C=`cat $F | wc -l`
    for APP_T in $APPS_TS
    do
        APP=`echo $APP_T | cut -d"," -f1`
        T=`echo $APP_T | cut -d"," -f2`
        R=`~/src/nway/bin/$APP -f $F -t $T`
        echo -e "$APP\t$T\t$COMBO\t$C\t$R"
    done
done
