#!/bin/bash

CONFIGS="$(basename -s .mk config/*.mk)"

for i in $CONFIGS; do \
	echo "Compiling $i" && make -s clean && make -s CONFIG=$i; \
done
