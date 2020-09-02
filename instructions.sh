#!/bin/bash

mkdir -p data_files

# Compile the files
g++ -std=c++11 -o cubical cubical.cpp
g++ -std=c++11 -o generate generate.cpp
g++ -std=c++11 -o test_cubical test_cubical.cpp
# To generate data
# ./generate [filename] [value] [columns] [rows]
# this creates a file named "filename" with "columns"x"rows" pixels
# with random function values less than "value" associated with each pixel
./generate data_files/test_image 10 5 4

# inspect the data generated
less data_files/test_image

# Run the algorithm
# ./cubical [inputfile] [outputfile] [logfile]
# this will run the algorithm on the "inputfile"
# the bigrades will be written to the "outputfile"
# if "logfile" is specified, then detailed steps are logged
./cubical data_files/test_image data_files/test_image_bigrades data_files/test_image_log

# inspect the bigrades
less data_files/test_image_bigrades

# inspect the log file
less data_files/test_image_log

# The image.py can convert a PNG image to a format that cubical.cpp takes in as input
# We would potentially want a c++ program to do this - image.cpp could be useful in that regard

# This section is describing the problem we have

# generate a 280x280 image with values <= 10
./generate data_files/280_280_10 10 280 280

# inspect the file
less data_files/280_280_10

# run the algorithm on that file
./cubical data_files/280_280_10 data_files/280_280_10_bigrades

# inspect the bigrades
less data_files/280_280_10_bigrades

# To test output of bigrades
# ./test_cubical [filename] [bigrade file]
# This prints "Correct" if the bigrade computation is correct
# This prints "Incorrect" following with the location of error if the bigrade file is incorrect
./test_cubical data_files/280_280_10 data_files/280_280_10_bigrades










##################################
# Helpful function for debugging #
##################################

# valgrind -v --leak-check=yes ./cubical data_files/...

# gdb --args test data_files