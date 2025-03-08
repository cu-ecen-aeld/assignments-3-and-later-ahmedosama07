#!/bin/bash

if [ $# -lt 2 ]
then
    echo "Usage: $0 <writefile> <writestr>"
    exit 1
fi

writefile=$1
writestr=$2

write_path=$(dirname $writefile)

if ! mkdir -p "$write_path" || ! touch "$writefile"
then
    echo "Failed to create directory $write_path"
    exit 1
fi

echo "$writestr" > "$writefile"