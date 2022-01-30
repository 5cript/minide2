#!/bin/bash

cd backend
find foo/bar/ -iname *.hpp -o -iname *.cpp | xargs clang-format -i