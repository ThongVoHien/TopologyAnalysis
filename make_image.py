import cv2
import sys

if len(sys.argv) != 2:
	exit(1)

img = cv2.imread(sys.argv[1])

dimensions = img.shape

rows = img.shape[0]
columns = img.shape[1]
channels = img.shape[2]

print(columns, rows)

for y in range(rows):
	for x in range(columns):
		value = (int)(((int)(img[y][x][0])+(int)(img[y][x][1])+(int)(img[y][x][2]))/3)
		print(value, end=' ')
	print()