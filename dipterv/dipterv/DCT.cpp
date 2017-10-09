#include "stdafx.h"
#include <fstream>
#include "DCT.h"

std::vector<std::string> split3(const std::string &text, char sep)
{
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

void DCT::printFingerPrint()
{
	for (auto o : fingerPrint)
	{
		for (auto i : o)
		{
			std::cout << i << " ";
		}

		std::cout << std::endl;
	}
}

std::string DCT::checkMatchInDB(std::vector<int> fingerPrintToSearch)
{
	loadDB();

	std::string bestVideo = "Nincs benne az adatbazisban";
	int bestError = std::numeric_limits<int>::max();
	for (auto video : dbFingerPrint)
	{
		std::string videoName = video.first;
		float errorSum = 0.0f;
		for (int i = 0; i < 64; i++)
		{
			errorSum += abs(fingerPrintToSearch[i] - video.second[0][i]);
		}

		if (errorSum < bestError && errorSum < 1) // 1 = threshold
		{
			bestError = errorSum;
			bestVideo = videoName;
		}
	}

	return bestVideo;
}

void DCT::appendToDatabase(int n, std::vector<int> keyFrameNumbers)
{
	std::ofstream dbFile;
	dbFile.open("DCT_database.txt", std::ios::out | std::ios::app);
	dbFile << std::to_string(n) + ".mp4" << "\n";

	for (auto dct : fingerPrint[0])
	{
		dbFile << dct << " ";
	}

	dbFile << "\n";

	dbFile.close();
}

void DCT::loadDB()
{
	std::vector<std::vector<int>> videoFingerPrints;
	std::vector<int> fingerPrintNumbers;

	std::string line;
	std::ifstream myfile("DCT_database.txt");
	int lineNum = 0;
	std::string elozoVideoNev;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (lineNum % 2 == 0)
			{
				if (videoFingerPrints.size() > 0)
				{
					dbFingerPrint[elozoVideoNev] = videoFingerPrints;
					videoFingerPrints.clear();
				}

				elozoVideoNev = line;
			}
			else
			{
				std::vector<std::string> values = split3(line, ' ');
				int frame = std::stoi(values[0]);
				for (int i = 0; i < 64; i++)
				{
					fingerPrintNumbers.push_back(std::stoi(values[i]));
				}

				videoFingerPrints.push_back(fingerPrintNumbers);
				fingerPrintNumbers.clear();
			}

			lineNum++;
		}

		myfile.close();
	}
}

void DCT::calculateFingerPrint(int median)
{
	std::vector<int> fingerPrintOfAKeyFrame;
	for (auto o : unsortedPixelsOfRois)
	{
		if (o >= median)
		{
			fingerPrintOfAKeyFrame.push_back(1);
		}
		else
		{
			fingerPrintOfAKeyFrame.push_back(0);
		}
	}

	fingerPrint.push_back(fingerPrintOfAKeyFrame);
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

void DCT::run(cv::Mat keyFrames)
{
	cv::GaussianBlur(keyFrames, keyFrames, cv::Size(5, 5), 0, 0);
		
	cv::Mat dctTransformed;
	keyFrames.convertTo(keyFrames, CV_32FC1); // dct miatt más formátumra konvertálás
	cv::dct(keyFrames, dctTransformed);

	cv::Rect myROI(0, 0, 4, 4); // 4x4es ablak létrehozása

	coefficients.push_back(dctTransformed(myROI));

	/*cv::imshow("Fingerprint", dctTransformed);
	cv::waitKey(0);*/

	if (coefficients.size() == 4)
	{
		int median = calculateMedian();
		calculateFingerPrint(median);
		//fingerPrint.push_back(6879);
		coefficients.clear();
	}
}
