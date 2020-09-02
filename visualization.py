import sys
import numpy as np
import matplotlib.pyplot as plt
from ast import literal_eval

def build(function, thickness):
	global image, rows, columns, bigrades
	total_set = 0
	for r in range(rows):
		for c in range(columns):
			image[r][c] = False
			pixel = bigrades[c+r*columns]
			for f, t in pixel:
				if f <= function and t <= thickness:
					image[r][c] = True
					total_set += 1
					break
	return total_set

if len(sys.argv) != 2:
	exit(1)

bigrades_file = open(sys.argv[1], "r").readlines()

columns, rows = literal_eval(bigrades_file[0])

bigrades = [[literal_eval(bigrade) for bigrade in line.strip().split()[1:]] for line in bigrades_file[1:]]
num_pixels = len(bigrades)
image = np.zeros((rows, columns), np.bool_)

fn, th, ts = 0, -1, 0
while True:
	point = input("Press Enter for 0,0 or to thicken by 1. Enter q to quit. Enter function and thickness values (Last: "+str(fn)+","+str(th)+"): ")
	point = point.strip()
	if point == "q":
		break
	if point == "":
		if ts == num_pixels:
			th = 0
			fn += 1
		else:
			th += 1
	else:
		point = point.split(',')
		if len(point) != 2:
			print("Incorrect input. Try again!")
			continue
		try:
			fn, th = (int)(point[0]), (int)(point[1])
			if fn < 0 or th < 0:
				print("Cannot take negative values (yet!). Try again!")
				continue
		except:
			print("Incorrect input. Try again!")
			continue
	ts = build(fn, th)
	if ts == num_pixels:
		colormap = 'gray'
	else:
		colormap = 'binary'

	plt.imshow(image, cmap=colormap, extent=[0,columns,0,rows])
	plt.title(sys.argv[1].split('/')[-1].strip(), pad=15)
	plt.show()

exit(0)