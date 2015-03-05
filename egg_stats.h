#pragma once

#include "eggtype.h"
#include <array>
#include <Eigen/Dense>
using namespace Eigen;

/**
	Represents a generalized normal distribution over N variables, described by
	the mean and covariance matrices.
*/
template<int N>
struct MultivariateNormal {
	Matrix<float, N, 1> mean;
	Matrix<float, N, N> covariance;

	/**
		generalization of [(x - mu) / sigma]^2 to N variables
		see: http://en.wikipedia.org/wiki/Mahalanobis_distance
	*/
	double mahalanobisDistanceSq(Matrix<float, N, 1> value) const {
		return (value - mean).transpose() * covariance.inverse() * (value - mean);
	}
};

namespace egg_stats {
	/**
		We model each egg as a normal distribution over all readings for
		that egg. The four variables of the distribution are the red, blue,
		white, and ambient components of the reading

		The parameters for these models are in egg_stats.cc, which is
		auto-generated by a python script from a set of calibration readings
	*/
	extern std::array<MultivariateNormal<4>, EGG_TYPE_COUNT> expectations;
}
