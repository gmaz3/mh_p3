#!/bin/bash

directorio="data"

for i in $(ls $directorio)
do
    echo $i
    ./bin/__nombre__ 'data/'$i 531
done
