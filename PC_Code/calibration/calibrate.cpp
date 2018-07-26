/*/////////////////////////////
thoroughly plagiarized from https://www.youtube.com/watch?v=v7jutAmWJVQ&list=PLAp0ZhYvW6XbEveYeefGSuLhaPlFML9gP&index=16 and following videos

interactive process to find the camera matrix and distance coefficients of your camera.
User prints out pattern.png chessboard at https://docs.opencv.org/2.4/_downloads/pattern.png.
Hold image in front of camera.
If program successfully draws points on corners of squares, press space to save image.
Press enter to complete calibration once you have saved 10-15 good images from different angles.
Program will use all images to determine camera matrix and distance coefficients 
and writes these values to a file for use by loadCalibration.cpp.
*////////////////////////////////

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

const Size chessboardDimensions = Size(6,9);

/*populates "corners" vector with a vector of 3d points representing where the corners are in the real world
as calculated from the real edge length*/
void createKnownBoardPositions(Size boardSize, float squareEdgeLength, vector<Point3f>& corners)
{
	for(int i = 0; i < boardSize.height; i++)
	{
		for(int j = 0; j < boardSize.width; j++)
		{
			corners.push_back(Point3f(j*squareEdgeLength, i*squareEdgeLength, 0.0f));
		}
	}

}

void getChessboardCorners(vector<Mat> images, vector<vector<Point2f> >& allFoundCorners, bool showResults = false)
{
	for(vector<Mat>::iterator iter = images.begin(); iter != images.end(); iter++)
	{
		vector<Point2f> pointBuffer;
		bool found = findChessboardCorners(*iter, chessboardDimensions, pointBuffer, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
		if(found)
		{
			allFoundCorners.push_back(pointBuffer);	
		} 	
		else
		{
			cout << "It ain't work B\n";
		}

		if(showResults)
		{
			drawChessboardCorners(*iter, chessboardDimensions, pointBuffer, found);
			imshow("Corners", *iter);
			waitKey();
		}
	}
}
		
	 
void cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength, Mat& cameraMatrix, Mat& distanceCoefficients)
{
	//holds 2d points representing where the corners are in the image
	vector<vector<Point2f> > imageCorners;
	getChessboardCorners(calibrationImages, imageCorners, false);

	//holds the 3d points representing where the corners are in the real world
	vector<vector<Point3f> > worldCorners(1);

	//puts the real corner positions in worldCorners
	createKnownBoardPositions(boardSize, squareEdgeLength, worldCorners[0]);

	//makes worldCorners have the same length as the number of images whose corner points it is going to store
	worldCorners.resize(imageCorners.size(), worldCorners[0]);

	vector<Mat> rvectors, tvectors;
	distanceCoefficients = Mat::zeros(8,1, CV_64F);

	//populates cameraMatrix, distanceCoefficients, rvectors, tvectors
	calibrateCamera(worldCorners, imageCorners, boardSize, cameraMatrix, distanceCoefficients, rvectors, tvectors);
}

//writes data to file name
bool saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients)
{
	ofstream outStream;
	if(outStream)
	{
		outStream.open("calibration.txt");	
		
		uint16_t rows = cameraMatrix.rows;
		uint16_t columns = cameraMatrix.cols;

		//writes cameraMatrix dimensions to file
		outStream << rows << endl;
		outStream << columns << endl;

		//writes cameraMatrix data to file
		for(int r = 0; r < rows; r++){
			for(int c = 0; c < columns; c++)
			{
				double value = cameraMatrix.at<double>(r,c);
				outStream << value << endl;
			}
		}

		rows = distanceCoefficients.rows;
		columns = distanceCoefficients.cols;

		//writes distnce coefficients dimensions to file
		outStream << rows << endl;
		outStream << columns << endl;
		//
		//writes distance coefficients to file
		for(int r = 0; r < rows; r++){
			for(int c = 0; c < columns; c++)
			{
				double value = distanceCoefficients.at<double>(r,c);
				outStream << value << endl;
			}
		}

		outStream.close();
		return true;
	}

	return false;
}


//writes found points to output file
/********
void writeFoundPoints(vector<Vec2f> foundPoints){
	ofstream outStream;
	if(outStream)
	{
		outStream.open("foundPoints.txt");	
		for(int r = 0; r < foundPoints.rows; r++){
			for(int c = 0; c < foundPoints.cols; c++){
				outStream << "(" << foundPoints.at<double>(r,c);	
			}
		}

}
*/

int main(int argv, char** argc){
	Mat frame;
	Mat drawToFrame;
	Mat cameraMatrix = Mat::eye(3,3, CV_64F);
	char character;

	Mat distanceCoefficients;
	
	vector<Mat> savedImages;
	
	vector<vector<Point2f> > markerCorners, rejectedCandidates;
	
	VideoCapture vid(1); //webcam

	if(!vid.isOpened()){
		cout << "It ain't work B" << endl;
		return 0;
	}

	int framesPerSecond = 50;

	namedWindow("webcam", CV_WINDOW_AUTOSIZE);
	vector<Vec2f> foundPoints;

	bool finished = false;

	while(!finished){
		if(!vid.read(frame))
			break;
		
		bool found = false;

		//finds chessboard corners in frame, writes them to foundPoints, returns true if corners found
		found = findChessboardCorners(frame, chessboardDimensions, foundPoints, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
	
		//copies frame to drawToFrame
		frame.copyTo(drawToFrame);

		//draws chessboard corners on drawToFrame if found
		drawChessboardCorners(drawToFrame, chessboardDimensions, foundPoints, found);

		if(found) //if chessboard corners were found
			imshow("webcam", drawToFrame);
		else
			imshow("webcam", frame);

		//if key is pressed, react, otherwise wait 1000/fps milliseconds
		char character = waitKey(1000 / framesPerSecond);

	
		//action depends on which key was pressed
		switch(character)
		{
			case ' ':
				//if space, program found good corners, so save image
				if(found){
					cout << "corners found" << endl;
					Mat temp;
					frame.copyTo(temp);
					savedImages.push_back(temp);
				}
				break;
			case 13: //enter
				//finished collecting images, so start calibration	
				cout << "calibration started" << endl;
				cameraCalibration(savedImages, chessboardDimensions, CAL_SQUARE_EDGE, cameraMatrix, distanceCoefficients);
				cout << "camera calibrated" << endl;
				saveCameraCalibration(CALIBRATION_FILE, cameraMatrix, distanceCoefficients);
				cout << "calibration saved" << endl;
				finished = true;
				break;
			case 27: //Esc
				//bad results, so terminate program
				return 0;
		}
		
	}

	
	return 0;	
}

