# TopologyAnalysis

Algorithms for building bifiltration from digital images

## Prerequisites
* Python 3.x
* C++11 or higher
* Python modules:
	* opencv-python
	* numpy
	* matplotlib

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
./gen [filename] [function-value-limit] [columns] [rows]
```
If you have an existing `.png` image, convert it to the supported format by:
```
python make_image.py [path-to-png-image]
```
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

You can also visualize the bifiltration from the bigrades file:
```
python visualization.py [bigrades-file]
```
In the terminal prompt, enter the bigrade pair for which you want to visualize the image (for example: 2,3).
Examine the plot created and close it to enter another bigrade.
You can also press "Enter" every time to thicken by 1.