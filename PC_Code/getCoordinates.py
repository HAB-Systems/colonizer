#reads data from OpenCFU, writes only coordinates of colonies to coordinates.txt in csv format

import csv
import sys

#first argument is input file, second argument is output file
inputfile = str(sys.argv[1])
outputfile = str(sys.argv[2])

fread = open(inputfile, "r")
reader = csv.reader(fread)
data = list(reader)

#second and third columns are x and y coordinates
coordinates = [sublist[1:3] for sublist in data]

#writes coordinates to coordinates.txt
fwrite = open(outputfile, "w")
a = 2
for cpair in coordinates:
	fwrite.write(cpair[0] + "," + cpair[1] + "\n")

fwrite.close()
