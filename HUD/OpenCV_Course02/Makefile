LIBRARIES = -framework opencv2
FILES = main.cpp

EXE = output
#for apple silicon change the -arch value to arm64
all:
	g++ -g -Wall $(LIBRARIES) $(FILES) -o $(EXE)
clean:
	-rm $(EXE)


#include <imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>



#g++ main.cpp -I"/usr/local/include/" -L"usr/local/Cellar/opencv/4.8.0_1/lib" -lopencv_core -lopencv_highgui -videoio -o main


