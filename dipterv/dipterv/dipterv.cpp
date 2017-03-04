// dipterv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <omp.h>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "CGO.h"
#include "FrameMontage.h"

#define SCENEDETECECTTHRESHOLD 210
#define VIDEOCUTTHRESHOLD 200

int countFrameDifference(cv::Mat& currentFrame, cv::Mat& lastFrame)
{
	int sumOfDifferences = 0;
	for (int row = 0; row < currentFrame.rows; row++)
	{
		#pragma omp parallel for reduction(+:sumOfDifferences)
		for (int column = 0; column < currentFrame.cols; column++)
		{
			char currentFramePixel = currentFrame.at<char>(cv::Point(column, row));
			char lastFramePixel = lastFrame.at<char>(cv::Point(column, row));
			int difference = abs((int)currentFramePixel - (int)lastFramePixel);
			sumOfDifferences += difference;
		}
	}

	return sumOfDifferences;
}

bool detectNewScene(cv::Mat& frame, cv::Mat& lastFrame, double currentFrame, double lastSceneFrame, int& sumOfDifferences, int& lastSumOfDifferences, std::vector<cv::Mat>& keyFrames)
{
	if (currentFrame > 0)
	{
		sumOfDifferences = countFrameDifference(frame, lastFrame);
	}
	else
	{
		keyFrames.push_back(frame);

		return true;
	}

	if ((abs(sumOfDifferences - lastSumOfDifferences) / 100000 > SCENEDETECECTTHRESHOLD) && currentFrame > lastSceneFrame + 1)
	{
		keyFrames.push_back(frame);

		return true;
	}

	return false;
}

int main()
{
	cv::VideoCapture cap("basejump2.mp4");

	if (!cap.isOpened())
	{
		std::cout << "Cannot open the video file. \n";
		return -1;
	}

	double fps = cap.get(CV_CAP_PROP_FPS);

	cv::namedWindow("video", CV_WINDOW_AUTOSIZE);

	cv::Mat frame;
	cv::Mat lastFrame;
	double lastSceneFrame = 0;
	int sumOfDifferences = 0;
	int lastSumOfDifferences = 0;

	std::vector<cv::Mat> keyFrames;

	FrameMontage frameMontage;
	while (1)
	{
		double currentFrame = cap.get(CV_CAP_PROP_POS_FRAMES);

		if (currentFrame < VIDEOCUTTHRESHOLD)
		{
			if (!cap.read(frame))
			{
				std::cout << "\n Cannot read the video file. \n";
				break;
			}

			cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
			cv::imshow("video", frame);

			// FrameMontage
			if ((int)currentFrame % 50 == 0)
			{
				frameMontage.run(frame);
				
				//cv::imshow("video", frameMontage.frameMontage);
			}

			if (detectNewScene(frame, lastFrame, currentFrame, lastSceneFrame, sumOfDifferences, lastSumOfDifferences, keyFrames))
			{
				lastSceneFrame = currentFrame;
				std::cout << "uj jelenet " << currentFrame << std::endl;
			}

			if (cv::waitKey(10) == 'q') // Wait for 'q' key press to exit
			{
				break;
			}

			lastFrame = frame;
			lastSumOfDifferences = sumOfDifferences;
		}
		else
		{
			break;
		}

	}

	// Centroid of gradient
	//CGO cgo;
	//cv::namedWindow("video2", CV_WINDOW_AUTOSIZE);

	//auto start = std::chrono::system_clock::now();
	//for (unsigned int i = 0; i < keyFrames.size(); i++)
	//{
	//	auto start = std::chrono::system_clock::now();

	//	cgo.run(keyFrames[i]);
	//}

	//auto end = std::chrono::system_clock::now();
	//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	//std::cout << "Elapsed:" << elapsed_seconds << std::endl;

	//cgo.printFingerPrint();

	frameMontage.calculateFingerPrint();
	frameMontage.printFingerPrint();


	return 0;
}


