#!/usr/bin/env bash

cd "$( dirname "${BASH_SOURCE[0]}" )"

FILE="../chef/src/dimensions.h"

echo -e "#ifndef __DIMENSIONS_H__
#define __DIMENSIONS_H__

#include <math.h>" > $FILE

cat simu.m | ./mat2h.py >> $FILE

echo -e "#endif" >> $FILE

