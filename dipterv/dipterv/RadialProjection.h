#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class RadialProjection
{

	cv::Mat fingerPrint;
public:
	std::vector<cv::Point> bresenham(cv::Point p0, cv::Point p1);
	void printFingerPrint();
	void calculateFingerPrint(int median);
	int calculateMedian();
	void run(std::vector<cv::Mat> keyFrames);
};