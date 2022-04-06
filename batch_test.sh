#!/bin/bash

for i in {0..5}
do
	./cu_test $1 /data/not_backed_up/shared/pattern/4_${i}.txt
done
