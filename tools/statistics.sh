#!/bin/bash

find ./include ./src "(" -name "*.cpp" -or -name "*.h" ")" -print | xargs wc -l
