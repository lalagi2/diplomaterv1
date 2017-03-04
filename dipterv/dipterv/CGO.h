#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <omp.h>

class CGO
{
public:

	std::vector<std::vector<double>> fingerPrint;

	CGO()
	{

	}

	void printFingerPrint();

	void run(cv::Mat keyFrame);
};
