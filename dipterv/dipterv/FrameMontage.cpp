#include "FrameMontage.h"

void FrameMontage::printFingerPrint()
{
	cv::imshow("Fingerprint", fingerPrint);
	cv::waitKey(0);
}

void FrameMontage::calculateFingerPrint()
{
	cv::Mat dctTransformed;
	frameMontage.convertTo(frameMontage, CV_32F);
	cv::dct(frameMontage, dctTransformed);

	cv::Rect myROI(0, 0, 100, 100);

	dctTransformed.convertTo(dctTransformed, CV_8UC1);

	fingerPrint = dctTransformed(myROI);
}

void FrameMontage::run(cv::Mat frame)
{
	cv::resize(frame, frame, cv::Size(400, 300));

	if (frameMontage.cols == 0)
	{
		frameMontage = frame;
	}
	else
	{
		cv::Size newSize;
		newSize.height = frame.rows;
		newSize.width = frameMontage.cols + frame.cols;

		cv::Mat biggerMat(newSize, CV_8UC1);

		frameMontage.copyTo(biggerMat(cv::Rect(0, 0, frameMontage.cols, frameMontage.rows)));
		frame.copyTo(biggerMat(cv::Rect(frameMontage.cols, 0, frame.cols, frame.rows)));

		frameMontage = biggerMat;
	}
}