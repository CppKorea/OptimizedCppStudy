#pragma once

#include "fixedBlockMemoryManager.h"

char arena[40004];
fixed_block_memory_manager<fixed_arena_controller> list_memory_manager( arena );

template <typename T>
struct FixedBlockAllocator
{
	using value_type = T;

	FixedBlockAllocator( ) = default;

	template <typename U>
	FixedBlockAllocator( const FixedBlockAllocator<U>& ) {}

	T* allocate( std::size_t n, const void* = 0 )
	{
		return reinterpret_cast<T*>( list_memory_manager.allocate( n * sizeof( T ) ) );
	}

	void deallocate( T* ptr, size_t )
	{
		list_memory_manager.deallocate( ptr );
	}
};

template <typename T, typename U>
inline bool operator==( const FixedBlockAllocator<T>&, const FixedBlockAllocator<U>& )
{
	return true;
}

template <typename T, typename U>
inline bool operator!=( const FixedBlockAllocator<T>& lhs, const FixedBlockAllocator<U>& rhs )
{
	return !( lhs == rhs );
}
