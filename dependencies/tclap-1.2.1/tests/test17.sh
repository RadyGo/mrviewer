#!/bin/sh

# failure
../examples/test3  --stringTest=one homer -B > tmp.out 2>&1

if cmp -s tmp.out $srcdir/test17.out; then
	exit 0
else
	exit 1
fi

