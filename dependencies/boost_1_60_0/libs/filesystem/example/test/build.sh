#!/usr/bin/env bash

# Copyright Beman Dawes, 2010
# Distributed under the Boost Software License, Version 1.0.
# See www.boost.org/LICENSE_1_0.txt
echo Compiling example programs...
b2 $* >b2.log
grep "error" <b2.log
