#reads data from OpenCFU, writes only coordinates of colonies to coordinates.txt in csv format

import csv
fread = open("test.txt", "r")
reader = csv.reader(fread)
data = list(reader)

#third and fourth columns are x and y coordinates
coordinates = [sublist[2:4] for sublist in data]

#writes coordinates to coordinates.txt
fwrite = open("coordinates.txt", "w")
a = 2
for cpair in coordinates:
	fwrite.write(cpair[0] + "," + cpair[1] + "\n")

fwrite.close()
