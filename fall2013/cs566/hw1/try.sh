#!/bin/bash
for n in 32 64 128 256 512 1024 2048 4096 8192 16384
    do for k in 1 2 4
       do for p in 1 2 4
        do make run N=$n K=$k P=$p 
        done
    done
done
