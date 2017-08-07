#include "cpp11Allocator.h"
#include "FixedBlockAllocator.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <list>
#include <map>

using namespace std::chrono;

int main( )
{
	std::cout << "> Test Case One" << std::endl;
	std::cout << "Fixed Block Memomry Manager : ";
	{
		std::list<int, FixedBlockAllocator<int>> fixedBlockList;

		auto begin = steady_clock::now( );

		for ( int i = 0; i < 1000; ++i )
		{
			fixedBlockList.push_back( i );
		}

		fixedBlockList.clear( );

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<microseconds>( end - begin );

		std::cout << elapsedTime.count( ) << " micro sec" << std::endl;
	}
	
	std::cout << "CRT Heap Manager : ";
	{
		std::list<int, my_allocator<int>> crtHeapList;

		auto begin = steady_clock::now( );

		for ( int i = 0; i < 1000; ++i )
		{
			crtHeapList.push_back( i );
		}

		crtHeapList.clear( );

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<microseconds>( end - begin );

		std::cout << elapsedTime.count( ) << " micro sec" << std::endl;
	}

	list_memory_manager.clear( );

	std::cout << "> Test Case Two" << std::endl;
	std::cout << "Fixed Block Memomry Manager : ";
	{
		std::map<int, int, std::less<int>, FixedBlockAllocator<std::pair<int, int>>> fixedBlockList;

		auto begin = steady_clock::now( );

		for ( int i = 0; i < 1000; ++i )
		{
			fixedBlockList[i] = i;
		}

		fixedBlockList.clear( );

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<microseconds>( end - begin );

		std::cout << elapsedTime.count( ) << " micro sec" << std::endl;
	}

	std::cout << "CRT Heap Manager : ";
	{
		std::map<int, int, std::less<int>, my_allocator<std::pair<int, int>>> crtHeapList;

		auto begin = steady_clock::now( );

		for ( int i = 0; i < 1000; ++i )
		{
			crtHeapList[i] = i;
		}

		crtHeapList.clear( );

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<microseconds>( end - begin );

		std::cout << elapsedTime.count( ) << " micro sec" << std::endl;
	}
}
