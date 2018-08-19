#reads data from OpenCFU, writes only coordinates of colonies to coordinates.txt in csv format

import csv
import sys

inputfile = "my_results.csv" 
outputfile = "coordinates.csv"

fread = open(inputfile, "r")
reader = csv.reader(fread)
data = list(reader)

#second and third columns are x and y coordinates
coordinates = [sublist[2:4] for sublist in data]

#writes coordinates to coordinates.csv
fwrite = open(outputfile, "w")
for cpair in coordinates[1:]:
	fwrite.write(cpair[0] + "," + cpair[1] + "\n")

fwrite.close()
