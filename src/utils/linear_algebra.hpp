#ifndef LINEAR_ALGEBRA_HPP_
#define LINEAR_ALGEBRA_HPP_

#include <Eigen/Dense>

/**
 * @brief Equality comparison
 *
 * Equality comparison between 4D vectors.
 * @param {Eigen::Vector4d &} v; 1st input vector
 * @param {Eigen::Vector4d &} w; 2nd input vector
 * @return Return true is both vectors are equal.
 */
bool are_equal(Eigen::Vector4d &v, Eigen::Vector4d &w) {
	Eigen::Vector4d v_w = v - w;
	return (
		is_equal_to(v_w(0),0.)
	 && is_equal_to(v_w(1),0.)
	 && is_equal_to(v_w(2),0.)
	 && is_equal_to(v_w(3),0.)
	);
}

/**
 * @brief Mean 4D
 *
 * Compute the mean estimator of the input data set.
 * This method is implemented for 4 dimensions.
 * @param {std::vector<Eigen::Vector4d> &} data; input data set
 * @return Return the estimator of the 4D mean vector of the input data set.
 */
Eigen::Vector4d mean4d_estimator(std::vector<Eigen::Vector4d> &data) {
	assert(data.size() > 0);
	Eigen::Vector4d m  = Eigen::Vector4d::Zero();
	for(auto &v : data) {
		m += v;
	}
	m /= ((double) data.size());
	return m;
}

/**
 * @brief Covariance 4D
 *
 * Compute the covariance estimator of the input data set.
 * This method is implemented for 4 dimensions.
 * Mean vector is given as an argument for faster computation in case it would
 * already have been calculated.
 * @param {std::vector<Eigen::Vector4d> &} data; input data set
 * @param {Eigen::Vector4d &} mean; mean of the distribution or estimator
 * @return Return the estimator of the 4x4 covariance matrix of the input data set.
 */
Eigen::Matrix4d cov4d_estimator(
	std::vector<Eigen::Vector4d> &data,
	Eigen::Vector4d &mean)
{
	assert(data.size() > 0);
	Eigen::Matrix4d cov = Eigen::Matrix4d::Zero();
	if(data.size() > 1) {
		for(auto &v : data) {
			cov += (v-mean)*(v-mean).transpose();
		}
		cov /= ((double) data.size() - 1.);
	}
	return cov;
}

/**
 * @brief Covariance 4D
 *
 * Compute the covariance estimator of the input data set.
 * This method is implemented for 4 dimensions.
 * @param {std::vector<Eigen::Vector4d> &} data; input data set
 * @return Return the estimator of the 4x4 covariance matrix of the input data set.
 */
Eigen::Matrix4d cov4d_estimator(std::vector<Eigen::Vector4d> &data) {
	Eigen::Vector4d mean = mean4d_estimator(data);
	return cov4d_estimator(data,mean);
}

/**
 * @brief Mahalanobis distance
 *
 * Compute the Mahalanobis distance between a point and a distribution.
 * An additional parameter is added: the precision with which the determinant of
 * the covariance matrix is considered to be zero.
 * If the determinant is zero, a 'big' value is returned.
 * @param {Eigen::Vector4d &} v; input vector
 * @param {Eigen::Vector4d &} mean; mean of the distribution or estimator
 * @param {Eigen::Matrix4d &} cov; covariance of the distribution or estimator
 * @param {double} precision; precision of the determinant
 * @return Return the Mahalanobis distance.
 */
double mahalanobis_distance(
	Eigen::Vector4d &v,
	Eigen::Vector4d &mean,
	Eigen::Matrix4d &cov,
	double precision = 1e-30)
{
	if(are_equal(v,mean)) {
		return 0.;
	}
	Eigen::Vector4d vm = v-mean;
	if(is_equal_to(cov.determinant(),0.,precision)) {
		return 9e99;
	} else {
		return sqrt(std::max(0.,(vm.transpose() * cov.inverse() * vm)(0)));
	}
}

/**
 * @brief Mahalanobis distance
 *
 * Compute the Mahalanobis distance between a point and a data set.
 * Make use of estimators for the mean and standard deviation.
 * An additional parameter is added: the precision with which the determinant of
 * the covariance matrix is considered to be zero.
 * If the determinant is zero, a 'big' value is returned.
 * @param {std::vector<Eigen::Vector4d> &} data; input data set
 * @param {Eigen::Vector4d &} mean; mean of the distribution or estimator
 * @return Return the estimator of the 4x4 covariance matrix of the input data set.
 */
double mahalanobis_distance(
	Eigen::Vector4d &v,
	std::vector<Eigen::Vector4d> &data,
	double precision = 1e-30)
{
	Eigen::Vector4d mean = mean4d_estimator(data);
	Eigen::Matrix4d cov  = cov4d_estimator(data,mean);
	return mahalanobis_distance(v,mean,cov,precision);
}

#endif // LINEAR_ALGEBRA_HPP_
