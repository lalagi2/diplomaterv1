#pragma once

#include <vector>
#include <chrono>
#include <iostream>
#include <map>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


struct CornerPoint
{
	cv::Point coord;
	float R;
};

class Corner
{
public:
	std::vector<std::vector<CornerPoint>> videoFingerPrints;
	std::map<std::string, std::vector<std::vector<CornerPoint>>> dbFingerPrint;
	std::string checkMatchInDB(std::vector<std::vector<CornerPoint>> fingerPrintToSearch);
	float distance(CornerPoint p1, CornerPoint p2);

	void appendToDatabase(int n, std::vector<int> keyFrameNumbers);
	void loadDB();

	void printFingerPrint();
	std::vector<CornerPoint> run(cv::Mat keyFrame);

	bool nincsBenneEddig(int maxX, int maxY);
};
