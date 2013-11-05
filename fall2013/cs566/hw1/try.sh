make delete
    for k in 2 4
       do for p in 2 
        do for n in 64 128 256 512 1024 2048
        do make run N=$n K=$k P=$p
        done
    done
done
