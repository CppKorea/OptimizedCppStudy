#include <iostream>

#include <vector>
#include <list>

#include "stop_watch.hpp"

long long val = 0;

template <class Container>
long long iterate(const Container &cont)
{
	StopWatch watch;
	for (auto it = cont.begin(); it != cont.end(); ++it)
		val += *it + 1;

	return watch.stop();
}

int main(int argc, char **argv)
{
	std::cout << "size,vector,list" << std::endl;
	for (size_t size = 1000; size < 100000000; size *= 10)
	{
		std::vector<int> v(100000000, 1);
		std::list<int> l(v.begin(), v.end());

		std::cout << size << ","
			<< iterate(v) << ","
			<< iterate(l) << std::endl;
	}
	return 0;
}
