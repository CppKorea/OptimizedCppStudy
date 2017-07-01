#include <vector>
#include <algorithm>
#include <set>
#include <unordered_set>

#include <iostream>
#include "generators.hpp"
#include "stop_watch.hpp"

const size_t THRESHOLD = 1000;

template <typename T>
std::pair<long long, long long> measure_linear(const std::vector<T> &elements, const std::vector<T> &keys)
{
	std::pair<long long, long long> ret;
	
	// STD.FIND()
	StopWatch watch;

	if (elements.capacity() <= THRESHOLD)
	{
		for (auto it = keys.begin(); it != keys.end(); ++it)
			auto x = std::find(elements.begin(), elements.end(), *it);

		ret.first = watch.stop();
	}

	// STD.LOWER_BOUND()
	std::vector<T> sequence = elements;
	std::sort(sequence.begin(), sequence.end());

	watch.initialize();
	for (auto it = keys.begin(); it != keys.end(); ++it)
		auto x = std::lower_bound(sequence.begin(), sequence.end(), *it);

	ret.second = watch.stop();

	// RETURNS
	return ret;
}

template <typename Container, typename T>
long long measure_associative(const std::vector<T> &elements, const std::vector<T> &keys)
{
	Container container(elements.begin(), elements.end());
	StopWatch watch;

	for (auto it = keys.begin(); it != keys.end(); ++it)
		auto x = container.find(*it);

	return watch.stop();
}

template <typename T, class Generator>
void measure(const std::vector<T> &elements, const std::vector<T> &keys)
{
	const static size_t ITERATION_SIZE = 100 * 100;

	// LINEAR (FIND & LOWER_BOUND)
	auto vector_times =  measure_linear(elements, keys);

	// ASSOCIATIVES (TREE & HASH)
	auto tree_time = measure_associative<std::multiset<T>>(elements, keys);
	auto hash_time = measure_associative<std::unordered_multiset<T>>(elements, keys);

	// PRINT THE ELAPSED TIMES
	std::cout << elements.size() << ",";
	if (elements.capacity() <= THRESHOLD)
		std::cout << vector_times.first << ",";
	
	std::cout << vector_times.second << ","
		<< tree_time << ","
		<< hash_time << std::endl;
}

template <typename T, class Generator>
void measure
	(
		size_t ELEMENTS_SIZE = 100 * 1000,
		size_t ITERATATION_SIZE = 100 * 1000,
		size_t STEP_SIZE = 1000
	)
{
	// PRINT CONFIGURATIONS
	std::cout << "#elements," << ELEMENTS_SIZE << std::endl;
	std::cout << "#iterations," << ITERATATION_SIZE << std::endl;
	std::cout << std::endl;

	//--------
	// CONSTRUCT INITIAL DATA
	//--------
	std::vector<T> elements;
	std::vector<T> keys;

	elements.reserve(ELEMENTS_SIZE);
	keys.reserve(STEP_SIZE);

	// KEYS
	for (size_t i = 0; i < ITERATATION_SIZE; ++i)
		keys.push_back(Generator()());

	//--------
	// MEASURE ELAPSED TIMES
	//--------
	// COLUMNS
	if (elements.capacity() <= THRESHOLD)
		std::cout << "size,std::find(),std::lower_bound(),std::multiset,std:unordered_multiset" << std::endl;
	else
		std::cout << "size,std::lower_bound(),std::multiset,std:unordered_multiset" << std::endl;

	// PROCESS STEPS
	for (size_t i = 0; i < ELEMENTS_SIZE; i += STEP_SIZE)
	{
		// INSERT ELEMENTS
		for (size_t j = 0; j < STEP_SIZE; j++)
			elements.push_back(Generator()());

		// AND MEASURES THEM
		measure<T, Generator>(elements, keys);
	}
}

int main(int argc, char **argv)
{
	if (argc == 1)
		measure<int, IntegerGenerator>();
	else
		measure<int, IntegerGenerator>
		(
			std::stoull(argv[1]),
			std::stoull(argv[2]),
			std::stoull(argv[3])
		);

	return 0;
}