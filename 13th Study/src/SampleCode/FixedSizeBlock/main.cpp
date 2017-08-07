/*
주의 : 이 소스 코드는 기본 스택 크기를 초과한 배열을 할당하므로 스택 크기 옵션을 변경할 필요가 있음.
기본 설정으로 10 MB를 스택으로 예약하도록 변경하였음.
*/

#include "fixedBlockMemoryManager.h"

#include <chrono>
#include <iostream>
#include <random>
#include <vector>

constexpr int testCount = 1'000'000;

class MemMgrTester
{
	int contents_;
public:
	MemMgrTester( int c ) : contents_( c ) {}

	static void* operator new( size_t s )
	{
		return mgr_.allocate( s );
	}

	static void operator delete( void* p )
	{
		return mgr_.deallocate( p );
	}

	static fixed_block_memory_manager<fixed_arena_controller> mgr_;
};

char arena[sizeof( MemMgrTester ) * testCount] = {};
fixed_block_memory_manager<fixed_arena_controller> MemMgrTester::mgr_( arena );

class SomeClass
{
	int contents_;
public:
	SomeClass( int c ) : contents_( c ) {}
};

using namespace std::chrono;

int main( )
{
	MemMgrTester* fixedBlock[testCount];
	SomeClass* crtHeap[testCount];

	std::cout << "> Test Case One" << std::endl;
	std::cout << "Fixed Block Memomry Manager : ";
	{
		auto begin = steady_clock::now( );

		for ( int i = 0; i < testCount; ++i )
		{
			fixedBlock[i] = new MemMgrTester( i );
		}

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<milliseconds>( end - begin );

		std::cout << elapsedTime.count( ) << "ms" << std::endl;

		for ( int i = 0; i < testCount; ++i )
		{
			delete fixedBlock[i];
		}
	}

	std::cout << "CRT Heap Manager : ";
	{
		auto begin = steady_clock::now( );

		for ( int i = 0; i < testCount; ++i )
		{
			crtHeap[i] = new SomeClass( i );
		}

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<milliseconds>( end - begin );

		std::cout << elapsedTime.count( ) << "ms" << std::endl;

		for ( int i = 0; i < testCount; ++i )
		{
			delete crtHeap[i];
		}
	}

	for ( int i = 0; i < testCount; ++i )
	{
		fixedBlock[i] = nullptr;
		crtHeap[i] = nullptr;
	}

	std::random_device rd;
	std::mt19937 mt;
	mt.seed( rd( ) );

	std::uniform_int_distribution<> distribution( 0, 99 );

	std::cout << std::endl;
	std::cout << "> Test Case Two" << std::endl;
	std::cout << "Fixed Block Memomry Manager : ";
	{
		auto begin = steady_clock::now( );

		for ( int i = 0, j = 0; i < testCount; ++i )
		{
			j = distribution( mt );
			if ( fixedBlock[j] )
			{
				delete fixedBlock[j];
				fixedBlock[j] = nullptr;
			}
			else
			{
				fixedBlock[j] = new MemMgrTester( j );
			}
		}

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<milliseconds>( end - begin );

		std::cout << elapsedTime.count( ) << "ms" << std::endl;
	}

	std::cout << "CRT Heap Manager : ";
	{
		auto begin = steady_clock::now( );

		for ( int i = 0, j = 0; i < testCount; ++i )
		{
			j = distribution( mt );
			if ( crtHeap[j] )
			{
				delete crtHeap[j];
				crtHeap[j] = nullptr;
			}
			else
			{
				crtHeap[j] = new SomeClass( j );
			}
		}

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<milliseconds>( end - begin );

		std::cout << elapsedTime.count( ) << "ms" << std::endl;
	}
}