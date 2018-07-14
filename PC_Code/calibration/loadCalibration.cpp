#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
//#include "opencv2/aruco.hpp"
#include "opencv2/calib3d.hpp"

#include <stdint.h>
#include <sstream>
#include <iostream>
#include <fstream>

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

	loadCameraCalibration("calibration.txt", cameraMatrix, distanceCoefficients);

}
