#pragma once
#include <vector>
#include <chrono>
#include <iostream>
#include <map>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define M_PI 3.14159265

class RadialProjection
{
public:
	std::vector<std::vector<double>> fingerPrint;
	std::map<std::string, std::vector<std::vector<double>>> dbFingerPrint;

	std::vector<cv::Point> bresenham(cv::Point p0, cv::Point p1);
	int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y);
	int get_line_intersection(cv::Point2f p0, cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f& interSection);
	void rotateVector(float *x, float *y, float angle);

	void printFingerPrint();
	std::string checkMatchInDB(std::vector<std::vector<double>> fingerPrintToSearch);
	void loadDB();
	std::vector<double> run(cv::Mat keyFrames);
	void appendToDatabase(int n, std::vector<int> keyFrameNumbers);
};