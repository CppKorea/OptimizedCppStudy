#include "fixedBlockMemoryManager.h"

#include <iostream>
#include <vector>
#include <chrono>

constexpr int testCount = 1000;

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

char arena[sizeof(MemMgrTester) * testCount];
fixed_block_memory_manager<fixed_arena_controller> MemMgrTester::mgr_( arena );

class SomeClass
{
	int contents_;
public:
	SomeClass( int c ) : contents_( c ) {}
};

int main( )
{
	MemMgrTester* fixedBlock[testCount];
	SomeClass* crtHeap[testCount];

	std::chrono::time_point<std::chrono::steady_clock> begin = std::chrono::steady_clock::now();

	for ( int i = 0; i < testCount; ++i )
	{
		fixedBlock[i] = new MemMgrTester( i );
	}

	for ( int i = 0; i < testCount; ++i )
	{
		delete fixedBlock[i];
	}

	std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now( );

	std::chrono::microseconds elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>( end - begin );

	std::cout << elapsedTime.count() << std::endl;

	begin = std::chrono::steady_clock::now( );

	for ( int i = 0; i < testCount; ++i )
	{
		crtHeap[i] = new SomeClass( i );
	}

	for ( int i = 0; i < testCount; ++i )
	{
		delete crtHeap[i];
	}

	end = std::chrono::steady_clock::now( );

	elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>( end - begin );

	std::cout << elapsedTime.count( ) << std::endl;
}