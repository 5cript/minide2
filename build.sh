#!/bin/bash

function runCmake {
	if [[ -n "${MSYSTEM}" ]]; then
		cmake -G"MSYS Makefiles" .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_MAKE_PROGRAM=make -DCMAKE_LIBRARY_PATH=/mingw64/x86_64-w64-mingw32/lib/
	else
		cmake .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_MAKE_PROGRAM=make -DCMAKE_LIBRARY_PATH=/mingw64/x86_64-w64-mingw32/lib/
	fi
	
	ret=$?
	if [[ $ret -ne 0 ]]; then
		echo -e "\e[31;5mBuilding" $1 "failed in cmake step!!! Exiting\e[0m"
		exit 1
	fi
}

function runMake {
	make -j8
	
	ret=$?
	if [[ $ret -ne 0 ]]; then
		echo -e "\e[31;5mBuilding" $1 "failed in make step!!! Exiting\e[0m"
		exit 1
	fi
}

function doBuild {
	cd $1
	echo -e "\e[32mBuilding " $1 "...\e[0m"
	pwd 
	mkdir -p build
	cd build
	runCmake $1
	runMake $1
	cd ..
	cd ..
}

doBuild SimpleJSON
doBuild automata
doBuild attender
doBuild sol2
doBuild tiny-process-library
doBuild lldb-mi
doBuild debugger-interface
#doBuild filewatch