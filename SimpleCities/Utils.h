#pragma once

#include <opencv2/opencv.hpp>

namespace utils {

	double uniform_rand();
	double uniform_rand(double a, double b);
	double normal_rand(double mean, double sigma);

	float gause(float u, float sigma);


}