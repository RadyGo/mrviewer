#!/bin/sh

# this tests whether all required args are listed as
# missing when no arguments are specified
# failure  
../examples/test11  > tmp.out 2>&1

if cmp -s tmp.out $srcdir/test63.out; then
	exit 0
else
	exit 1
fi

