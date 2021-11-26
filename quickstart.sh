#!/bin/bash

# defaults
filename="test_image"
value="10"
columns="5"
rows="4"

if [ "$#" -eq 0 ]; then
	printf "Using all defaults\n"
elif [ "$#" -eq 4 ]; then
	filename=$1
	value=$2
	columns=$3
	rows=$4
else
	printf "Error: Invalid number of arguments\n"
	exit 1;
fi

# make directory for data files
printf "Making directory for data_file if it doesn't exist\n"
mkdir -p data_files

# compile the files
printf "Compiling all files\n"
g++ -o gen -std=c++11 generate_image.cpp
g++ -o big -std=c++11 4c_positive_entrance_bigrades.cpp
g++ -o test -std=c++11 test_4c_positive_entrance_bigrades.cpp

# generate data
printf "Generating data\n"
./gen data_files/${filename} ${value} ${columns} ${rows}

# run the algorithm
printf "Running the algorithm\n\n"
./big data_files/${filename} data_files/${filename}_bigrades

# test correcteness
printf "\nTesting correctness\n"
./test data_files/${filename} data_files/${filename}_bigrades
