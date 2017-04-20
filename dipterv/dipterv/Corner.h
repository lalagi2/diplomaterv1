#pragma once

#include <vector>
#include <chrono>
#include <iostream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class Corner
{
public:
	std::vector<cv::Point2i> cornerPoints;

	void printFingerPrint();
	void run(std::vector<cv::Mat> keyFrames);
};
