#pragma once

#include <iostream>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class FrameMontage
{
public:
	cv::Mat frameMontage;
	cv::Mat fingerPrint;

	void printFingerPrint();

	void calculateFingerPrint();

	void run(cv::Mat frame);
};
