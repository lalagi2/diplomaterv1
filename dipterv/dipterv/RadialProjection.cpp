#include "stdafx.h"
#include <fstream>
#include "RadialProjection.h"

std::vector<std::string> split4(const std::string &text, char sep)
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

std::vector<cv::Point> RadialProjection::bresenham(cv::Point p0, cv::Point p1)
{
	std::vector<cv::Point> pixelCoordinates;

	float x1 = (float)p0.x;
	float x2 = (float)p1.x;
	float y1 = (float)p0.y;
	float y2 = (float)p1.y;

	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x<maxX; x++)
	{
		if (steep)
		{
			cv::Point p(y, x);
			pixelCoordinates.push_back(p);
		}
		else
		{

			cv::Point p(x, y);
			pixelCoordinates.push_back(p);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}

	return pixelCoordinates;
}

int RadialProjection::get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float * i_x, float * i_y)
{
	float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
	s10_x = p1_x - p0_x;
	s10_y = p1_y - p0_y;
	s32_x = p3_x - p2_x;
	s32_y = p3_y - p2_y;

	denom = s10_x * s32_y - s32_x * s10_y;
	if (denom == 0)
		return 0; // Collinear
	bool denomPositive = denom > 0;

	s02_x = p0_x - p2_x;
	s02_y = p0_y - p2_y;
	s_numer = s10_x * s02_y - s10_y * s02_x;
	if ((s_numer < 0) == denomPositive)
		return 0; // No collision

	t_numer = s32_x * s02_y - s32_y * s02_x;
	if ((t_numer < 0) == denomPositive)
		return 0; // No collision

	if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
		return 0; // No collision
				  // Collision detected
	t = t_numer / denom;
	if (i_x != NULL)
		*i_x = p0_x + (t * s10_x);
	if (i_y != NULL)
		*i_y = p0_y + (t * s10_y);

	return 1;
}

int RadialProjection::get_line_intersection(cv::Point2f p0, cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f& intersection)
{
	float p0_x = p0.x;
	float p1_x = p1.x;
	float p2_x = p2.x;
	float p3_x = p3.x;

	float p0_y = p0.y;
	float p1_y = p1.y;
	float p2_y = p2.y;
	float p3_y = p3.y;

	float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
	s10_x = p1_x - p0_x;
	s10_y = p1_y - p0_y;
	s32_x = p3_x - p2_x;
	s32_y = p3_y - p2_y;

	denom = s10_x * s32_y - s32_x * s10_y;
	if (denom == 0)
		return 0; // Collinear
	bool denomPositive = denom > 0;

	s02_x = p0_x - p2_x;
	s02_y = p0_y - p2_y;
	s_numer = s10_x * s02_y - s10_y * s02_x;
	if ((s_numer < 0) == denomPositive)
		return 0; // No collision

	t_numer = s32_x * s02_y - s32_y * s02_x;
	if ((t_numer < 0) == denomPositive)
		return 0; // No collision

	if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
		return 0; // No collision
				  // Collision detected
	t = t_numer / denom;

	intersection.x = p0_x + (t * s10_x);
	intersection.y = p0_y + (t * s10_y);

	return 1;
}

void RadialProjection::rotateVector(float * x, float * y, float angle)
{
	float tempx = *x;
	float tempy = *y;
	*x = tempx * cos(angle * M_PI / 180.0) - tempy * sin(angle  * M_PI / 180.0);
	*y = tempx * sin(angle  * M_PI / 180.0) + tempy * cos(angle * M_PI / 180.0);
}

void RadialProjection::printFingerPrint()
{
}

std::string RadialProjection::checkMatchInDB(std::vector<std::vector<double>> fingerPrintToSearch)
{
	loadDB();

	std::string bestVideo = "Nincs benne az adatbazisban";
	float bestError = std::numeric_limits<double>::max();
	for (auto video : dbFingerPrint)
	{
		std::string videoName = video.first;
		float errorSum = 0.0f;
		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 40; j++)
			{
				errorSum += fabs(fingerPrintToSearch[i][j] - video.second[i][j]);
			}
		}

		if (errorSum < bestError && errorSum < 3) // 1 = threshold
		{
			bestError = errorSum;
			bestVideo = videoName;
		}
	}

	return bestVideo;
}

void RadialProjection::loadDB()
{
	std::vector<std::vector<double>> videoFingerPrints;
	std::vector<double> fingerPrintNumbers;

	std::string line;
	std::ifstream myfile("Radial_database.txt");
	int lineNum = 0;
	std::string elozoVideoNev;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (lineNum % 6 == 0)
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
				std::vector<std::string> values = split4(line, ' ');
				int frame = std::stoi(values[0]);
				for (int i = 1; i < 40; i++)
				{
					fingerPrintNumbers.push_back(std::stod(values[i]));
				}

				videoFingerPrints.push_back(fingerPrintNumbers);
				fingerPrintNumbers.clear();
			}

			lineNum++;
		}

		myfile.close();
	}

	else std::cout << "Unable to open file";
}

std::vector<double> RadialProjection::run(cv::Mat keyFrame)
{
	auto start = std::chrono::system_clock::now();
	float R[180];
	float szoras[180];
	float egyenesAtlag[180];
	float szorasNegyzet[180];
		
	//#pragma omp parallel for
	for (int angle = 0; angle < 180; angle += 1)
	{
		float iranyVektorX = -1.0f;
		float iranyVektorY = 0.0f;

		rotateVector(&iranyVektorX, &iranyVektorY, (float)angle);
		cv::Point2f intersectFloat;
		cv::Point2f intersect2Float;
		if (angle < 29.358f)
		{
			get_line_intersection(cv::Point2f(0.0f, 0.0f), cv::Point2f(0.0f, 719.0f), cv::Point2f(640.0f, 360.0f), cv::Point2f(640.0f + iranyVektorX * 10000.0f, 360.0f + iranyVektorY * 10000.0f), intersectFloat);
			get_line_intersection(cv::Point2f(1279.0f, 0.0f), cv::Point2f(1279.0f, 719.0f), cv::Point2f(640.0f, 360.0f), cv::Point2f(640.0f + iranyVektorX * 10000.0f * -1.0f, 360.0f + iranyVektorY * 10000.0f * -1.0f), intersect2Float);
		}
		else if (angle > 29.358f && angle < (180.0f - 29.358f))
		{
			get_line_intersection(cv::Point2f(0.0f, 0.0f), cv::Point2f(1279.0f, 0.0f), cv::Point2f(640.0f, 360.0f), cv::Point2f(640.0f + iranyVektorX * 10000.0f, 360.0f + iranyVektorY * 10000.0f), intersectFloat);
			get_line_intersection(cv::Point2f(0.0f, 719.0f), cv::Point2f(1279.0f, 719.0f), cv::Point2f(640.0f, 360.0f), cv::Point2f(640.0f + iranyVektorX * 10000.0f * -1.0f, 360.0f + iranyVektorY * 10000.0f * -1.0f), intersect2Float);
		}
		else
		{
			get_line_intersection(cv::Point2f(1279.0f, 0.0f), cv::Point2f(1279.0f, 719.0f), cv::Point2f(640.0f, 360.0f), cv::Point2f(640.0f + iranyVektorX * 10000.0f, 360.0f + iranyVektorY * 10000.0f), intersectFloat);
			get_line_intersection(cv::Point2f(0.0f, 0.0f), cv::Point2f(0.0f, 719.0f), cv::Point2f(640.0f, 360.0f), cv::Point2f(640.0f + iranyVektorX * 10000.0f * -1.0f, 360.0f + iranyVektorY * 10000.0f * -1.0f), intersect2Float);
		}

		cv::Point intersect = intersectFloat;
		cv::Point intersect2 = intersect2Float;

		std::vector<cv::Point> pixelsCoordinates = bresenham(intersect, intersect2);

		float Mu = 0.0f;
		float PLeftSide = 0.0f;
		float PRightSide = 0.0f;
		for (auto coordinate : pixelsCoordinates)
		{
			cv::Scalar pixelIntensity = keyFrame.at<uchar>(coordinate.y, coordinate.x);

			PLeftSide += pixelIntensity[0] * pixelIntensity[0];
			PRightSide += pixelIntensity[0];

			Mu += pixelIntensity[0];
		}

		PLeftSide /= pixelsCoordinates.size();
		PRightSide /= pixelsCoordinates.size();
		Mu /= pixelsCoordinates.size();

		PRightSide = PRightSide * PRightSide;

		float P = PLeftSide - PRightSide;

		szorasNegyzet[angle] = P;
		egyenesAtlag[angle] = Mu;

		float _szoras = 0.0f;

		for (int i = 0; i < pixelsCoordinates.size(); i++)
		{
			cv::Scalar pixelIntensity = keyFrame.at<uchar>(pixelsCoordinates[i].y, pixelsCoordinates[i].x);

			_szoras += (pixelIntensity[0] - Mu) * (pixelIntensity[0] - Mu);
		}

		_szoras /= pixelsCoordinates.size();
		szoras[angle] = _szoras;
	}

	float sumEgyenesAtlag = 0.0f;
	for (auto o : egyenesAtlag)
	{
		sumEgyenesAtlag += o;
	}

	sumEgyenesAtlag /= 180.0f; //egyenesAtlag.size();

	float sumSzorasAtlag = 0.0f;
	for (auto o : szoras)
	{
		sumSzorasAtlag += o;
	}

	sumSzorasAtlag /= 180.0f; //szoras.size();

	for (int i = 0; i < 180 /*egyenesAtlag.size()*/; i++)
	{
		float _R = (szorasNegyzet[i] - sumEgyenesAtlag) / sumSzorasAtlag;
		if (fabs(sumSzorasAtlag) < 1e-8)
		{
			R[i] = 0.0f;
		}
		else
		{
			R[i] = _R;
		}
	}

	// DCT
	float sum = 0.0f;
	float N = 180;
	std::vector<double> coeffs;
	for (int n = 0; n < 40; n++)
	{
		sum = 0.0f;
		for (int phi = 0; phi < 180 /*R.size()*/ - 1; phi++)
		{
			sum += R[phi] * cos((M_PI * ((2 * phi) + 1) * n) / (2 * N));
		}

		float D = sqrt(2 / N) * sum;
		coeffs.push_back(D);
	}

	fingerPrint.push_back(coeffs);

	return coeffs;

	auto end = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
	std::cout << " Radial runtime: " << elapsed_seconds << " sec" << std::endl;
}

void RadialProjection::appendToDatabase(int n, std::vector<int> keyFrameNumbers)
{
	std::ofstream dbFile;
	dbFile.open("Radial_database.txt", std::ios::out | std::ios::app);
	dbFile << std::to_string(n) + ".mp4" << "\n";

	for (int i = 0; i < 5; i++)
	{
		dbFile << keyFrameNumbers[i] << " ";
		for (auto radial : fingerPrint[i])
		{
			dbFile << radial << " ";
		}

		dbFile << "\n";
	}

	dbFile.close();
}
