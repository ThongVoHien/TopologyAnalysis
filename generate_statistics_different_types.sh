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

printf ""
printf "4-Connectivity"

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
	./4c data_files/generated/${image}_random
	# ./4ccheck data_files/generated/${image}_random data_files/generated/${image}_random_bigrades
	echo "------------"

	echo "Special type (occurrence specification)"
	# ./4c data_files/generated/${image}_special
	./4c data_files/generated/${image}_special
	# ./4ccheck data_files/generated/${image}_special data_files/generated/${image}_special_bigrades
	echo "------------"

	echo "Centralized type"
	# ./4c data_files/generated/${image}_centralized
	./4c data_files/generated/${image}_centralized
	# ./4ccheck data_files/generated/${image}_centralized data_files/generated/${image}_centralized_bigrades
	echo "------------"

	echo "Worst type (multiple centers)"
	# ./4c data_files/generated/${image}_worst
	./4c data_files/generated/${image}_worst
	# ./4ccheck data_files/generated/${image}_worst data_files/generated/${image}_worst_bigrades
	echo "------------"



	echo "====================="

done


# printf ""
# printf ""
# printf ""


# printf "Distance Connectivity"

# for image in $(cat $input);
# do
# 	# printf "row $row column $column value $value"
# 	printf "Image $image:\n"
# 	value="$(cut -d'_' -f1 <<< $image)"
# 	row="$(cut -d'_' -f2 <<< $image)"
# 	column="$(cut -d'_' -f3 <<< $image)"

# 	./random ${image}_random $value $row $column
# 	./special ${image}_special $value $row $column
# 	./centralized ${image}_centralized $value $row $column
# 	./worst ${image}_worst $value $row $column
	

# 	echo "Random type"
# 	# ./4c data_files/generated/${image}_random
# 	./dc data_files/generated/${image}_random
# 	# ./dccheck data_files/generated/${image}_random data_files/generated/${image}_random_bigrades 500
# 	echo "------------"

# 	echo "Special type (occurrence specification)"
# 	# ./4c data_files/generated/${image}_special
# 	./dc data_files/generated/${image}_special
# 	# ./dccheck data_files/generated/${image}_special data_files/generated/${image}_special_bigrades 500
# 	echo "------------"

# 	echo "Centralized type"
# 	# ./4c data_files/generated/${image}_centralized
# 	./dc data_files/generated/${image}_centralized
# 	# ./dccheck data_files/generated/${image}_centralized data_files/generated/${image}_centralized_bigrades 500
# 	echo "------------"

# 	echo "Worst type (multiple centers)"
# 	# ./4c data_files/generated/${image}_worst
# 	./dc data_files/generated/${image}_worst
# 	# ./dccheck data_files/generated/${image}_worst data_files/generated/${image}_worst_bigrades 500
# 	echo "------------"



# 	echo "====================="

# done

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