#include <Windows.h>
#include <iostream>

constexpr int KB = 1024;
constexpr int MB = 1024 * KB;

void wait( )
{
	int input;
	std::cin >> input;
}

int main( )
{
	wait( );

	int* lfhAlloc = (int*)malloc( 2 * KB );

	wait( );

	int* crtAlloc = (int*)malloc( 32 * KB );

	wait( );

	HANDLE defaultHeap = GetProcessHeap( );
	int* heapAlloc = (int*)HeapAlloc( defaultHeap, 0, 32 * KB );

	wait( );

	int* sizeExtension = (int*)HeapAlloc( defaultHeap, 0, 2 * MB );

	wait( );

	HANDLE privateHeap = HeapCreate( 0, 1, 1 );

	wait( );

	HANDLE privateHeap2 = HeapCreate( 0, 7 * KB, 7 * KB );

	wait( );
}