#!/bin/bash

CLANG_FORMAT_EXEC="${CLANG_FORMAT:-clang-format}"
echo $CLANG_FORMAT_EXEC

cd backend
find . -iname *.hpp -o -iname *.cpp | xargs ${CLANG_FORMAT_EXEC} -i