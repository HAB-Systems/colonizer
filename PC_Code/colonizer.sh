#!/bin/bash
#takes image of colonies, runs opencfu, writes output to csv
inputfile="colonies.csv"
outputfile="coordinates.csv"
image="testColony.jpg"

opencfu -i $image -threshold=Auto | tail -n +2 > $inputfile
python getCoordinates.py $inputfile $outputfile
