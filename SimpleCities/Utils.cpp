#include "Utils.h"
#include <math.h>
#include <random>

namespace utils {
	const float M_PI = 3.1415926535;

	static std::default_random_engine generator;

	double uniform_rand() {
		return (double)(rand() % 1000) / 1000.0;
	}

	double uniform_rand(double a, double b) {
		return uniform_rand() * (b - a) + a;
	}

	double normal_rand(double mean, double sigma) {
		if (sigma == 0.0) return mean;
		else {
			std::normal_distribution<double> distribution(mean, sigma);
			return distribution(generator);
		}
	}

	float gause(float u, float sigma) {
		return 1.0f / 2.0f / M_PI / sigma / sigma * expf(-u * u / 2.0f / sigma / sigma);
	}


}