#include "FixedBlockAllocatorForString.h"

#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono;

using FixedBlockString = std::basic_string<char, std::char_traits<char>, FixedBlockAllocator<char>>;

FixedBlockString remove_ctrl_fixed_block( std::string s )
{
	FixedBlockString result;
	for ( size_t i = 0; i < s.length( ); ++i )
	{
		if ( s[i] > 0x20 )
		{
			result = result + s[i];
		}
	}

	return result;
}

std::string remove_ctrl( std::string s )
{
	std::string result;
	for ( size_t i = 0; i < s.length( ); ++i )
	{
		if ( s[i] > 0x20 )
		{
			result = result + s[i];
		}
	}

	return result;
}

int main( )
{
	std::string s( "\07Now is the time\07 for all good men\r\n to come to the aid of their country. \07" );
	s = s + s + s;

	std::cout << "Fixed Block Memomry Manager : ";
	{
		FixedBlockString result;

		auto begin = steady_clock::now( );

		for ( int i = 0; i < 100000; ++i )
			result = remove_ctrl_fixed_block( s );

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<milliseconds>( end - begin );

		std::cout << elapsedTime.count( ) << "ms" << std::endl;
	}

	std::cout << "CRT Heap Manager : ";
	{
		std::string result;

		auto begin = steady_clock::now( );

		for ( int i = 0; i < 100000; ++i )
			result = remove_ctrl( s );

		auto end = steady_clock::now( );
		auto elapsedTime = duration_cast<milliseconds>( end - begin );

		std::cout << elapsedTime.count( ) << "ms" << std::endl;
	}
}