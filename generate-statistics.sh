#!/bin/bash

set -e

input=$1

printf "Cubical Bifiltration Statistics\n\n"

g++ -o dc -std=c++11 dc_entrance_bigrades.cpp
g++ -o 4c -std=c++11 4c_entrance_bigrades.cpp
g++ -o dccheck -std=c++11 test_dc_entrance_bigrades.cpp
g++ -o 4ccheck -std=c++11 test_4c_entrance_bigrades.cpp

# printf "4-Connectivity\n\n"
# for image in $(cat $input);
# do
# 	printf "# file: $image\n"
# 	./4c data_files/test/$image
# 	./4ccheck data_files/test/$image
# 	rm -f data_files/test/${image}_bigrades
# 	printf "\n"
# done

printf "\n\nDistance Connectivity\n\n"
for image in $(cat $input);
do
	printf "# file: $image\n"
	./dc data_files/test/$image
	# ./dccheck data_files/test/$image
	rm -f data_files/test/${image}_bigrades
	printf "\n"
done