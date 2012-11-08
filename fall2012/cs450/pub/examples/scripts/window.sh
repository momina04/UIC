awk '{window[c%ws]=$1; sum+=window[c%ws]; sum-=window[(c+1)%ws]; c++; print sum/((c<ws)?c:ws)}' ws=${1-25}
