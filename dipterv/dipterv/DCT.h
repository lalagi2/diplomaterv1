#pragma once

#include <chrono>
#include <iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class DCT
{
public:
	std::vector<cv::Mat> coefficients;
	std::vector<int> fingerPrint;
	std::vector<int> unsortedPixelsOfRois;

	void printFingerPrint();
	void calculateFingerPrint(int median);
	int calculateMedian();
	void run(std::vector<cv::Mat> keyFrames);
};

