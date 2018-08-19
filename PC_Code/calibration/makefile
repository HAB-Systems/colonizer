all:	clean cal load fid fidim 

cal:
	g++ calibrate.cpp -o calibrate `pkg-config --cflags --libs opencv`
load:
	g++ loadCalibration.cpp -o loadCalibration `pkg-config --cflags --libs opencv`
fid:
	g++ -std=c++11 fiducial.cpp -o fiducial `pkg-config --cflags --libs opencv`
fidim:
	g++ takeFiducialImage.cpp -o takeFiducialImage `pkg-config --cflags --libs opencv`

clean: 
	rm -f calibrate loadCalibration fiducial takeFiducialImage
	

