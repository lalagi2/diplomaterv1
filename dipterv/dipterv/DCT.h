#pragma once

#include <chrono>
#include <iostream>
#include <map>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class DCT
{
public:
	std::vector<cv::Mat> coefficients;
	std::vector<std::vector<int>> fingerPrint;
	std::vector<int> unsortedPixelsOfRois;
	std::map<std::string, std::vector<std::vector<int>>> dbFingerPrint;

	void printFingerPrint();
	std::string checkMatchInDB(std::vector<int> fingerPrintToSearch);
	void appendToDatabase(int n, std::vector<int> keyFrameNumbers);
	void loadDB();
	void calculateFingerPrint(int median);
	int calculateMedian();
	void run(cv::Mat keyFrames);
};

