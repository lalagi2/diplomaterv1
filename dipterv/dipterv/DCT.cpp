#include "stdafx.h"
#include "DCT.h"

void DCT::printFingerPrint()
{
	for (auto o : fingerPrint)
	{
		std::cout << o << " ";
	}
}

void DCT::calculateFingerPrint(int median)
{
	for (auto o : unsortedPixelsOfRois)
	{
		if (o >= median)
		{
			fingerPrint.push_back(1);
		}
		else
		{
			fingerPrint.push_back(0);
		}
	}
}

int DCT::calculateMedian()
{
	std::vector<int> pixelsOfMatrices;
	int b = 0;
	for (int n = 0; n < 4; n++)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				char a = coefficients[n].at<char>(i, j);
				pixelsOfMatrices.push_back((int)a);
				unsortedPixelsOfRois.push_back((int)a);
				b++;
			}
		}
	}

	std::sort(pixelsOfMatrices.begin(), pixelsOfMatrices.begin() + pixelsOfMatrices.size()); // ezt lehetne párhuzamosítani

	int median = pixelsOfMatrices[pixelsOfMatrices.size() / 2];
	std::cout << "median: " << pixelsOfMatrices[pixelsOfMatrices.size() / 2] <<std::endl;

	return median;
}

void DCT::run(std::vector<cv::Mat> keyFrames)
{
	for (int i = 0; i < keyFrames.size(); i++)
	{
		cv::GaussianBlur(keyFrames[i], keyFrames[i], cv::Size(5, 5), 0, 0);
		
		cv::Mat dctTransformed;
		keyFrames[i].convertTo(keyFrames[i], CV_32FC1); // dct miatt más formátumra konvertálás
		cv::dct(keyFrames[i], dctTransformed);

		cv::Rect myROI(0, 0, 4, 4); // 4x4es ablak létrehozása

		coefficients.push_back(dctTransformed(myROI));

		/*cv::imshow("Fingerprint", dctTransformed);
		cv::waitKey(0);*/

		if (coefficients.size() == 4)
		{
			int median = calculateMedian();
			calculateFingerPrint(median);
			fingerPrint.push_back(6879);
			coefficients.clear();
		}
	}
}
