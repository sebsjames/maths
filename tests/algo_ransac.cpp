// -*- C++ -*-
/*
 * This file is part of sebsjames/maths, a library of maths code for modern C++
 *
 * See https://github.com/sebsjames/maths
 *
 * Test of ransac implementation in sm::algo
 * A 2D dataset is simulated composed of two parts; a linear part with 120 points and a Gaussian part, spatially distinct with 60 points
 * Standard linear regression will find  a line of best fit through all data points.
 * RANSAC should treat the Gaussian data points as outliers and return the linear best fit parameters for just the linear part of the data.
 * 
 * Author: Alex Blenkinsop
 * Date: June 2026
 */

#include <iostream>
#include <limits>
#include <string>
#include <vector>

import sm.algo;
import sm.random;

int main()
{
	int rtn = 0;

	constexpr std::size_t n_line = 120;
	constexpr std::size_t n_gauss = 60;
	constexpr float m_true = 1.8f;
	constexpr float c_true = 0.75f;

	sm::rand_uniform<float> xline_dist(-5.0f, 5.0f, 42u);
	sm::rand_normal<float> line_noise(0.0f, 0.2f, 43u);

	// Cluster is spatially distinct from the line set.
	sm::rand_normal<float> xgauss_dist(8.0f, 0.6f, 44u);
	sm::rand_normal<float> ygauss_dist(9.0f, 0.6f, 45u);

	std::vector<float> x;
	std::vector<float> y;
	x.reserve(n_line + n_gauss);
	y.reserve(n_line + n_gauss);

	for (std::size_t i = 0; i < n_line; ++i) {
		float xv = xline_dist.get();
		float yv = (m_true * xv) + c_true + line_noise.get();
		x.push_back(xv);
		y.push_back(yv);
	}

	for (std::size_t i = 0; i < n_gauss; ++i) {
		float xv = xgauss_dist.get();
		float yv = ygauss_dist.get();
		x.push_back(xv);
		y.push_back(yv);
	}

	sm::vec<float, 2> mc = sm::algo::ransac<std::vector, float>(x, y, 300, 0.45f, 70, 42u);
	float m_est = mc[0];
	float c_est = mc[1];
	sm::vec<float, 2> mc_linregr = sm::algo::linregr<std::vector, float>(x, y);
	float m_linregr = mc_linregr[0];
	float c_linregr = mc_linregr[1];

	std::cout << "True parameters: m=" << m_true << " c=" << c_true << std::endl;
	std::cout << "Linear regression estimate: m=" << m_linregr << " c=" << c_linregr << std::endl;
	std::cout << "RANSAC estimate: m=" << m_est << " c=" << c_est << std::endl;

	// Use 5% relative tolerances based on the true parameter magnitudes.
	const float m_tol = 0.05f * std::abs(m_true);
	const float c_tol = 0.05f * std::abs(c_true);
	if (std::abs(m_est - m_true) > m_tol) { --rtn; }
	if (std::abs(c_est - c_true) > c_tol) { --rtn; }

	std::cout << "Test " << (rtn < 0 ? "Failed" : "Passed") << std::endl;
	return rtn;
}
