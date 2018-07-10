#pragma once

template <typename T>
struct my_allocator
{
	using value_type = T;

	my_allocator( ) = default;

	template <typename U>
	my_allocator( const my_allocator<U>& ) {}

	T* allocate( std::size_t n, const void* = 0 )
	{
		return reinterpret_cast<T*>( ::operator new( n * sizeof( T ) ) );
	}

	void deallocate( T* ptr, size_t )
	{
		::operator delete( ptr );
	}
};

template <typename T, typename U>
inline bool operator==( const my_allocator<T>&, const my_allocator<U>& )
{
	return true;
}

template <typename T, typename U>
inline bool operator!=( const my_allocator<T>& lhs, const my_allocator<U>& rhs )
{
	return !( lhs == rhs );
}
