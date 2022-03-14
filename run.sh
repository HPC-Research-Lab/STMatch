#!/bin/bash

for i in {0..20}
do
	./cu_test $1 /data/not_backed_up/shared/pattern/5_${i}.txt
done
