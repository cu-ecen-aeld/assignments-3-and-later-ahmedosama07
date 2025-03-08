#!/bin/sh

if [ $# -lt 2 ]
then
    echo "Usage: $0 <filesdir> <searchdir>"
    exit 1
fi

filesdir=$1
searchstr=$2

if [ ! -d "$filesdir" ]
then
    echo "Directory $filesdir does not exist"
    exit 1
fi

numfiles=$(find "$filesdir" -type f | wc -l)
nummatches=$(grep -r "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $numfiles and the number of matching lines are $nummatches"

exit 0