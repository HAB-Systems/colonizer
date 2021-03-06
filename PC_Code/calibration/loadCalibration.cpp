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

#include "filenames.h"

using namespace cv;
using namespace std;

bool loadCameraCalibration(string filename, Mat& cameraMatrix, Mat& distanceCoefficients)
{
	ifstream inStream("calibration.txt");
	if(inStream)
	{
		uint16_t rows;	
		uint16_t columns;	
	
		//camera matrix dimensions
		inStream >> rows;
		inStream >> columns;

		cameraMatrix = Mat(Size(columns, rows), CV_64F);

		double read = 0.0f;

		//reads in camera matrix
		for(int r = 0; r < rows; r++)
		{
				for(int c = 0; c < columns; c++)
				{
					inStream >> read;
					cameraMatrix.at<double>(r,c) = read;
					cout << cameraMatrix.at<double>(r,c) << endl;	
				}
		}
		
		//distance coefficients dimensions 
		inStream >> rows;
		inStream >> columns;

		distanceCoefficients = Mat::zeros(rows, columns, CV_64F);
		
		//reads in distance coefficients
		for(int r = 0; r < rows; r++)
		{
				for(int c = 0; c < columns; c++)
				{
					inStream >> read;
					distanceCoefficients.at<double>(r,c) = read;
					cout << distanceCoefficients.at<double>(r,c) << endl;	
				}
		}

		inStream.close();
		return true;
	}
	else{
			cout << "File input failed" << endl;
			return false;
	}
}

int main()
{
	Mat cameraMatrix;
	Mat distanceCoefficients;

	//reads cameraMatrix and distanceCoefficients from the file created by calibrate.cpp
	loadCameraCalibration(CALIBRATION_FILE, cameraMatrix, distanceCoefficients);

	//reads original image from file
	Mat distorted = imread(DISTORTED_IMAGE, CV_LOAD_IMAGE_GRAYSCALE);

	//shows original (before correction) image
	imshow("distorted", distorted);

	Mat corrected = distorted.clone();

	//undistorts distorted and populates result to corrected
	undistort(distorted, corrected, cameraMatrix, distanceCoefficients);

	//saves corrected image as a file
	imwrite(CORRECTED_IMAGE, corrected);
	
	//shows corrected image
	imshow("corrected", corrected);

	/*
	shows the difference between distorted and corrected
	*/
	
	Mat distortedbw;
	Mat correctedbw;

	double thresh;

	//thresholds images to black and white
	thresh = threshold(distorted, distortedbw, 100, 255, THRESH_BINARY);
	thresh = threshold(corrected, correctedbw, 100, 255, THRESH_BINARY);

	Mat difference = correctedbw.clone();
	
	//shows pixels where two images are not equivalent in white
	for(int r = 0; r < distorted.rows; r++){
		for(int c = 0; c < distorted.cols; c++){
			if(distortedbw.at<uint8_t>(r,c) == correctedbw.at<uint8_t>(r,c))
				difference.at<uint8_t>(r,c) = 0;
			else
				difference.at<uint8_t>(r,c) = 255;

		}
	}

	imshow("difference", difference);
	waitKey();
	



}
