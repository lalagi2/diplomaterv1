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
#include "DCT.h"
#include "RadialProjection.h"
#include "Corner.h"

#define SCENEDETECECTTHRESHOLD 210
#define VIDEOCUTTHRESHOLD 10000

int countFrameDifference(cv::Mat& currentFrame, cv::Mat& lastFrame)
{
	int sumOfDifferences = 0;

	int** difference = new int*[currentFrame.rows];
	for (int i = 0; i < currentFrame.rows; i++)
		difference[i] = new int[currentFrame.cols];

	auto start = std::chrono::system_clock::now();

	//#pragma omp parallel for
	for (int row = 0; row < currentFrame.rows; row++)
	{
		for (int column = 0; column < currentFrame.cols; column++)
		{
			char currentFramePixel = currentFrame.at<char>(cv::Point(column, row));
			char lastFramePixel = lastFrame.at<char>(cv::Point(column, row));
			difference[row][column] = abs((int)currentFramePixel - (int)lastFramePixel);
		}
	}

	//#pragma omp parallel for reduction(+:sumOfDifferences)
	for (int row = 0; row < currentFrame.rows; row++)
	{
		for (int column = 0; column < currentFrame.cols; column++)
		{
			sumOfDifferences += difference[row][column];
		}
	}

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	//std::cout << " SSD runtime: " << elapsed_seconds << " sec" <<std::endl;

	// Deallocate the array
	for (int i = 0; i < currentFrame.rows; i++)
		delete[] difference[i];
	delete[] difference;

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

std::vector<cv::Mat> getKeyFrames(std::string videoName)
{
	std::vector<int> keyFrameNumbers;
	cv::VideoCapture cap("f:\\dipterv_video_adatbazis\\" + videoName);
	std::cout << "videoNum " << videoName << std::endl;

	if (!cap.isOpened())
	{
		std::cout << "Cannot open the video file. \n";
		exit(-1);
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

		//if (currentFrame < VIDEOCUTTHRESHOLD)
		{
			if (!cap.read(frame))
			{
				std::cout << "\n Cannot read the video file. \n";
				break;
			}

			cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
			cv::imshow("video", frame);

			// FrameMontage
			/*	if ((int)currentFrame % 50 == 0)
			{
			frameMontage.run(frame);

			cv::imshow("video", frameMontage.frameMontage);
			}*/

			if (detectNewScene(frame, lastFrame, currentFrame, lastSceneFrame, sumOfDifferences, lastSumOfDifferences, keyFrames))
			{
				lastSceneFrame = currentFrame;
				std::cout << "\tuj jelenet " << currentFrame << std::endl;
				keyFrameNumbers.push_back(currentFrame);

				if (keyFrames.size() >= 5)
				{
					break;
				}
			}

			if (cv::waitKey(1) == 'q') // Wait for 'q' key press to exit
			{
				break;
			}

			lastFrame = frame;
			lastSumOfDifferences = sumOfDifferences;
		}
		/*	else
			{
				break;
			}*/
	}

	return keyFrames;
}

int main()
{
//http://answers.opencv.org/question/5768/how-can-i-get-one-single-frame-from-a-video-file/	
	//for (int i = 1; i <= 9; i++)
	//{
	//	std::vector<int> keyFrameNumbers;
	//	std::string videoNumber = std::to_string(i);
	//	cv::VideoCapture cap("f:\\dipterv_video_adatbazis\\0" + videoNumber + ".mp4");
	//	std::cout << "videoNum " << videoNumber << std::endl;

	//	if (!cap.isOpened())
	//	{
	//		std::cout << "Cannot open the video file. \n";
	//		return -1;
	//	}

	//	double fps = cap.get(CV_CAP_PROP_FPS);

	//	cv::namedWindow("video", CV_WINDOW_AUTOSIZE);

	//	cv::Mat frame;
	//	cv::Mat lastFrame;
	//	double lastSceneFrame = 0;
	//	int sumOfDifferences = 0;
	//	int lastSumOfDifferences = 0;

	//	std::vector<cv::Mat> keyFrames;

	//	FrameMontage frameMontage;
	//	while (1)
	//	{
	//		double currentFrame = cap.get(CV_CAP_PROP_POS_FRAMES);

	//		//if (currentFrame < VIDEOCUTTHRESHOLD)
	//		{
	//			if (!cap.read(frame))
	//			{
	//				std::cout << "\n Cannot read the video file. \n";
	//				break;
	//			}

	//			cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	//			cv::imshow("video", frame);

	//			// FrameMontage
	//			/*	if ((int)currentFrame % 50 == 0)
	//			{
	//			frameMontage.run(frame);

	//			cv::imshow("video", frameMontage.frameMontage);
	//			}*/

	//			if (detectNewScene(frame, lastFrame, currentFrame, lastSceneFrame, sumOfDifferences, lastSumOfDifferences, keyFrames))
	//			{
	//				lastSceneFrame = currentFrame;
	//				std::cout << "\tuj jelenet " << currentFrame << std::endl;
	//				keyFrameNumbers.push_back(currentFrame);

	//				if (keyFrames.size() >= 5)
	//				{
	//					break;
	//				}
	//			}

	//			if (cv::waitKey(1) == 'q') // Wait for 'q' key press to exit
	//			{
	//				break;
	//			}

	//			lastFrame = frame;
	//			lastSumOfDifferences = sumOfDifferences;
	//		}
	//	/*	else
	//		{
	//			break;
	//		}*/

	//	}

	//	/*frameMontage.calculateFingerPrint();
	//	frameMontage.printFingerPrint();*/

	//	//// Centroid of gradient
	//	CGO cgo;
	//	//cv::namedWindow("video2", CV_WINDOW_AUTOSIZE);

	//	auto start = std::chrono::system_clock::now();
	//	for (unsigned int i = 0; i < keyFrames.size(); i++)
	//	{
	//		cgo.run(keyFrames[i]);
	//	}

	//	cgo.printFingerPrint();
	//	cgo.appendToDatabase(i, keyFrameNumbers);

	//	//auto end = std::chrono::system_clock::now();
	//	//auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//	//double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	//	//std::cout << "Elapsed:" << elapsed_seconds << std::endl;

	//	//cgo.printFingerPrint();

	//	//DCT dct;

	//	//dct.run(keyFrames);
	//	//dct.printFingerPrint();

	//	/*RadialProjection radialProjection;
	//	radialProjection.run(keyFrames);*/

	//	/*for (auto o : points)
	//	{
	//	std::cout << o << std::endl;
	//	}*/

	///*	Corner corner;
	//	corner.run(keyFrames);*/

	//	//getchar();
	//}
	
	std::vector<cv::Mat> keyFrame = getKeyFrames("nincsbenne.mp4");

	CGO cgo;
	std::vector<std::vector<double>> videoFingerPrint;
	for (auto kf : keyFrame)
	{
		std::vector<double> fingerPrint = cgo.run(kf);
		videoFingerPrint.push_back(fingerPrint);
	}

	std::cout << cgo.checkMatchInDB(videoFingerPrint);
	
	return 0;
}


