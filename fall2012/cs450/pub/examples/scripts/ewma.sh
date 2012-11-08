awk 'NR==1 {avg=$1} {avg=alpha*avg+(1-alpha)*$1; print avg}' alpha=${1-0.9}

