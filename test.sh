#!/bin/bash
args="${@:2}"
type="$1"

aux_test="test-${type}.txt"
ex="./gol-${type}.o ${args}"
res="${type}-res.txt"

echo "Testing ${type} time on ${args}" > $aux_test
for i in {1..10}
do
  eval $ex >> $aux_test
done

cat $aux_test | grep : | awk '{print $3}' > out.txt

echo -n "Testing ${type} time on ${args}: " >> $res
./stats.sh >> $res
echo " " >> $res

rm -f out.txt
rm -f $aux_test