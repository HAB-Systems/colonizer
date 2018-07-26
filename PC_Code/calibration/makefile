all:
	g++ calibrate.cpp -o cal `pkg-config --cflags --libs opencv`
	g++ loadCalibration.cpp -o load `pkg-config --cflags --libs opencv`
	g++ fiducial.cpp -o fid `pkg-config --cflags --libs opencv`
	g++ takeFiducialImage.cpp -o fidim `pkg-config --cflags --libs opencv`

fid:
	g++ fiducial.cpp -o fid `pkg-config --cflags --libs opencv`

