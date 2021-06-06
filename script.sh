#!/bin/bash

directorio="data"

for i in $(ls $directorio)
do
    echo $i
    ./bin/busquedaLocalReiterada-ES 'data/'$i 531
done
