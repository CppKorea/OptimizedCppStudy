#include <cstddef>
#include <limits>
#include <list>
#include <memory>

template <typename T> struct my_allocator_98
{
	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	pointer address( reference r ) { return &r; }
	const_pointer address( const_reference r ) { return &r; }

	template <typename U> struct rebind
	{
		typedef my_allocator_98<U> other;
	};

	my_allocator_98( ) {}
	template <typename U>
	my_allocator_98( const my_allocator_98<U>& ) {}

	void construct( pointer p, const T& t )
	{
		new ( p ) T( t );
	}

	void destroy( pointer p )
	{
		p->~T( );
	}

	size_type max_size( ) const
	{
		return std::numeric_limits<size_type>::max( ) / sizeof( value_type );
	}

	pointer allocate( size_type n, typename std::allocator<void>::const_pointer = 0 )
	{
		return reinterpret_cast<T*>( ::operator new( n * sizeof( T ) ) );
	}

	void deallocate( pointer p, size_type )
	{
		::operator delete( p );
	}
};

template <typename T, typename U>
inline bool operator==( const my_allocator_98<T>&, const my_allocator_98<U>& )
{
	return true;
}

template <typename T, typename U>
inline bool operator!=( const my_allocator_98<T>& lhs, const my_allocator_98<U>& rhs )
{
	return !( lhs == rhs );
}

int main( )
{
	std::list<int, my_allocator_98<int> > a;

	a.push_back( 1 );
	a.push_back( 2 );
}
