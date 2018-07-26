
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

const int framesPerSecond = 50;


int main(int argv, char** argc){
	Mat frame;
	Mat cameraMatrix = Mat::eye(3,3, CV_64F);

	Mat distanceCoefficients;
	
	VideoCapture vid(1); //webcam

	if(!vid.isOpened()){
		cout << "It ain't work B" << endl;
		return 0;
	}


	namedWindow("webcam", CV_WINDOW_AUTOSIZE);

	while(true){
		if(!vid.read(frame))
			break;
		
		imshow("webcam", frame);

		bool found = false;

		char character = waitKey(1000 / framesPerSecond);

		if(character == 13)//enter
		{
			Mat temp;
			frame.copyTo(temp);
			
			//saves corrected image as a file
			imwrite(DISTORTED_IMAGE, temp);

			cout << "Image captured" << endl;

			break;
		}
	}

	
	return 0;	
}

