#!/bin/bash

set -e

input=$1

printf "Image generated\n"

g++ -o random -std=c++11 generate_image.cpp
g++ -o special -std=c++11 generate_special_image.cpp
g++ -o centralized -std=c++11 generate_image_centralized_type.cpp
g++ -o worst -std=c++11 generate_image_worst_type.cpp



printf "Cubical Bifiltration Statistics\n"

g++ -o dc -std=c++11 dc_entrance_bigrades.cpp
g++ -o 4c -std=c++11 4c_entrance_bigrades.cpp
g++ -o dccheck -std=c++11 test_dc_entrance_bigrades.cpp
g++ -o 4ccheck -std=c++11 test_4c_entrance_bigrades.cpp

printf "Run Statistics\n\n"

for image in $(cat $input);
do
	# printf "row $row column $column value $value"
	printf "Image $image:\n"
	value="$(cut -d'_' -f1 <<< $image)"
	row="$(cut -d'_' -f2 <<< $image)"
	column="$(cut -d'_' -f3 <<< $image)"

	./random ${image}_random $value $row $column
	./special ${image}_special $value $row $column
	./centralized ${image}_centralized $value $row $column
	./worst ${image}_worst $value $row $column
	

	echo "Random type"
	# ./4c data_files/generated/${image}_random
	./dc data_files/generated/${image}_random
	echo "------------"

	echo "Special type (occurrence specification)"
	# ./4c data_files/generated/${image}_special
	./dc data_files/generated/${image}_special
	echo "------------"

	echo "Centralized type"
	# ./4c data_files/generated/${image}_centralized
	./dc data_files/generated/${image}_centralized
	echo "------------"

	echo "Worst type (multiple centers)"
	# ./4c data_files/generated/${image}_worst
	./dc data_files/generated/${image}_worst
	echo "------------"



	echo "====================="

done

# printf "4-Connectivity\n\n"
# for image in $(cat $input);
# do
# 	printf "# file: $image\n"
# 	./4c data_files/test/$image
# 	./4ccheck data_files/test/$image
# 	rm -f data_files/test/${image}_bigrades
# 	printf "\n"
# done

# printf "\n\nDistance Connectivity\n\n"
# for image in $(cat $input);
# do
# 	printf "# file: $image\n"
# 	./dc data_files/test/$image
# 	# ./dccheck data_files/test/$image
# 	rm -f data_files/test/${image}_bigrades
# 	printf "\n"
# done