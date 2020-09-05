# TopologyAnalysis

Algorithms for building bifiltration from digital images

## Prerequisites
* Python 3.x
* C++11 or higher
* Python modules:
	* opencv-python
	* numpy
	* matplotlib

## Quickstart

To generate some data and run the algorithm:
```
./quickstart.sh
```
You can control the name and size of the data file to generate:
```
./quickstart.sh <name> <function-value-limit> <columns> <rows>
./quickstart.sh 256_1024_768_image 256 1024 768
```
This will create a data file corresponding to a `1024`x`768` image with function values ranging from `0` to `255`.
Then it will run the algorithm to create a bigrade file and test the file for correctness.
It will also print out the runtime statistics while making the bigrades file.

## Compile

```
g++ -o gen -std=c++11 generate.cpp
g++ -o big -std=c++11 bigrades.cpp
g++ -o alt -std=c++11 alt_bigrades.cpp
g++ -o test -std=c++11 test_bigrades.cpp
```

## Run

### Creating data files

To generate a random image file:
```
./gen [data-file] [function-value-limit] [columns] [rows]
```
This creates a file named `data-file` with `columns`x`rows` pixels with random function values less than `function-value-limit` associated with each pixel.
If you have an existing `.png` image, convert it to the supported format by:
```
python make_image.py [path-to-png-image] > [data-file]
```
For an RGB image, the function value will be the average of the 3 values.
Examples are provided in the `data_files` directory.

### Computing bigrades

Once you have a supported data file, you can compute the bigrades by:
```
./big [data-file] [bigrades-file] [log-file]
```
The `log-file` is mostly used for debugging purposes and can be omitted for usual runs.
This will create the `bigrades-file` containing the bigrades.
It will also print out runtime statistics for the computation.
You may also use the alternative (slower) algorithm for computing bigrades:
```
./alt [data-file] [bigrades-file] [log-file]
```
This creates the same bigrades file, but as of now, there are no runtime statistics.

## Testing

You can check the correctness of the calculated bigrades by:
```
./test [data-file] [bigrades-file]
```
The `data-file` and `bigrades-file` should be the same ones supplied during bigrade computation.
This prints "Correct" if the bigrade computation is correct.
If not, it prints "Incorrect" followed by the location of error.

You can also visualize the bifiltration from the bigrades file:
```
python visualize.py [bigrades-file]
```
In the terminal prompt, enter the bigrade pair for which you want to visualize the image (for example: 2,3).
Examine the plot created and close it to enter another bigrade.
You can also press "Enter" every time to thicken by 1.