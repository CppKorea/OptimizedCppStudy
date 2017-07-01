#include <iostream>

#include <vector>
#include <list>

#include "stop_watch.hpp"
#include "generators.hpp"

long long val;

template <class Compare, class T>
long long compare(T standard, const std::vector<T> &v)
{
	StopWatch watch;
	for (auto it = v.begin(); it != v.end(); ++it)
		val += (long long)Compare()(standard, *it);

	return watch.stop();
}

int main(int argc, char **argv)
{
	//--------
	// CONSTRUCT TEST DATA
	//--------
	// STANDARD
	double standard = DoubleGenerator()();

	// VALUES TO BE COMPARED
	std::vector<double> v;
	v.reserve(100 * 1000 * 1000);

	for (size_t i = 0; i < v.capacity(); i++)
		v.push_back(DoubleGenerator()());

	//--------
	// MEASURE ELAPSED TIMES
	//--------
	std::cout << "equal,less" << std::endl;
	std::cout << compare<std::equal_to<double>>(standard, v) << ","
		<< compare<std::less<double>>(standard, v) << std::endl;

	system("pause");
	return 0;
}
