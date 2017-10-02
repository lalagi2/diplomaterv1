#pragma once
#include <vector>
#include <chrono>
#include <map>
#include <iostream>
#include <fstream>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <omp.h>

class CGO
{
public:
	std::string checkMatchInDB(std::vector<std::vector<double>> fingerPrintToSearch);
	void loadDB();
	void appendToDatabase(int n, std::vector<int> keyFrameNumbers);

	std::map<std::string, std::vector<std::vector<double>>> dbFingerPrint;
	std::vector<std::vector<double>> fingerPrint;

	CGO()
	{

	}

	void printFingerPrint();

	std::vector<double> run(cv::Mat keyFrame);
};
