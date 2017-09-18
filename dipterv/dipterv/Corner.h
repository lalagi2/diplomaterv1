#pragma once

#include <vector>
#include <chrono>
#include <iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


struct CornerPoint
{
	cv::Point2i coord;
	float R;
};

class Corner
{
public:
	std::vector<CornerPoint> cornerPoints;

	void printFingerPrint();
	void run(std::vector<cv::Mat> keyFrames);
};
