#pragma once

#include <random>
#include <string>

struct DoubleGenerator
{
	double operator()()
	{
		static std::random_device device;
		static std::uniform_real_distribution<double> distribution(0.0, 10000);

		return distribution(device);
	}
};

struct IntegerGenerator
{
	double operator()()
	{
		return (int)DoubleGenerator()();
	}
};

struct StringGenerator
{
	std::string operator()()
	{
		return std::to_string(DoubleGenerator()());
	}
};