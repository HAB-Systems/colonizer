/********************************
 Inputs image corrected by loadCalibration.cpp
 Finds chessboard corners on that image
 Uses known distances between chessboard corners to find pixel-to-cm ratio
 Scales list of colony coordinates from OpenCFU using this ratio
 Uses top left chessboard corner to calibrate image translation
 Applies image translation to list of coordinates
 *********************************/
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

#include <stdint.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

#include "filenames.h"

using namespace cv;
using namespace std;

//chessboard has 6x6 array of corners
const int numrows = 9;		
const int numcols = 6;		
const Size chessboardDimensions = Size(numcols,numrows);

//calculates distance between two points
float distanceBetweenPoints(Point2f a, Point2f b)
{
	return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

//pass image of chessboard, vector of Point2f to be populated by locations of chessboard corners
//if showResults, show the image with found corner points drawn on
void getChessboardCorners(Mat image, vector<Point2f>& foundCorners, bool showResults)
{
	vector<Point2f> pointBuffer;
	bool found = findChessboardCorners(image, chessboardDimensions, pointBuffer, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
	if(found)
	{
		foundCorners = pointBuffer;	
	} 	
	else
	{
		cout << "It ain't work B\n";
	}

	if(showResults)
	{
		//draws corners on image and shows it
		drawChessboardCorners(image, chessboardDimensions, pointBuffer, found);
		imshow("Corners", image);
		waitKey();
	}
}

int main()
{
	//reads in image that has been undistorted by loadCalibration.cpp
	Mat chessboard = imread(CORRECTED_IMAGE, CV_LOAD_IMAGE_GRAYSCALE);
	vector<Point2f> chessboardCorners;

	//populates chessboardCorners with points
	getChessboardCorners(chessboard, chessboardCorners, true);

	//first corner in vector is upper left corner
	int upperLeftX = chessboardCorners.at(0).x;
	int upperLeftY = chessboardCorners.at(0).y;
	 
/*
	//find pixel differences between corner points along rows and along columns
	//put vector of points into 2d array to make for easier subtraction
	Point2f cornerMatrix[numrows][numcols];
	int count = 0;
	for(vector<Point2f>::iterator iter = chessboardCorners.begin(); iter != chessboardCorners.end(); iter++){
		cornerMatrix[count/numcols][count%numcols] = *iter;		
		count ++;
	}

	float distanceSum = 0;
	float distance;

	//prints out matrix of points for validation
	cout << "Points------------------------------------------" << endl;
	for(int col = 0; col < numcols; col++){
		for(int row = 0; row < numrows; row++){
			cout << cornerMatrix[row][col].x << "," << cornerMatrix[row][col].y << " ";
		}
		cout << endl;
	}


	//adds distances along rows to distanceSum, prints for validation
	cout << "Row distances---------------------------------------" << endl;	
	for(int col = 0; col < numcols; col++){
		for(int row = 1; row < numrows; row++){
		distance = distanceBetweenPoints(cornerMatrix[row][col], cornerMatrix[row-1][col]);
			printf("%5f, ", distance);			
			distanceSum += distance;
		}
		cout << endl;
	}
		
	//adds distances along columns to distanceSum, prints for validation
	cout << "Column distances---------------------------------------" << endl;	
	for(int row = 0; row < numrows; row++){
		for(int col = 1; col < numcols; col++){
			distance = distanceBetweenPoints(cornerMatrix[row][col], cornerMatrix[row][col-1]);
			printf("%5f, ", distance);			
			distanceSum += distance;
		}
		cout << endl;
	}

	//total number of distances
	int numDistances = (numrows - 1)*numcols + (numcols - 1)*numrows;

	//calculates average distance
	float averageDistance = distanceSum / numDistances;
   	cout << "Average:\t" << averageDistance << endl;	

	//divides by real distance between corners to find pixels per cm 
	float pixelsPerCm = averageDistance/CAL_SQUARE_EDGE;
		cout << "Pixels per cm:\t" << pixelsPerCm << endl;	
	*/
	//---------------------------------------------------------------
	
		//finds translation between real upper left corner and image upper left corner
		int xdiff = REAL_X_COORDINATE - upperLeftX;
		int ydiff = REAL_Y_COORDINATE - upperLeftY;
		//file containing raw coordinates from OpenCFU (via getCoordinates.py)
		ifstream input_file;
		input_file.open(COORDINATE_FILE);

		//file to write corrected coordinates to
		ofstream output_file;
		output_file.open(CORRECTED_COORDINATE_FILE);

		//translates and dilates coordinates
		if(input_file.is_open() && output_file.is_open())
		{
			//string to temporarily hold numbers read from file
			string coordinate;
			
			//loops through file
			//first getline() gets x coordinate (before ,)
			while(getline(input_file, coordinate, ','))
			{
				//converts x to float
				float x = stof(coordinate);
				
				//gets rest of line (y coordinate), converts to float
				getline(input_file, coordinate);
				float y = stof(coordinate);

				//converts x and y to units of cm and applies translation 
				x = (x-xdiff)*100;
				y = (y-ydiff)*0.01;

				output_file << to_string(x) << "," << to_string(y) << endl;
		}

		input_file.close();


	}
	//if file not found		
	else cout << "File not found\n";

}
