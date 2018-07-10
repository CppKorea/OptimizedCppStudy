#pragma once

#include <algorithm>
#include <exception>

template <typename Arena>
struct fixed_block_memory_manager
{
	template <int N>
	fixed_block_memory_manager( char( &a )[N] );
	fixed_block_memory_manager( fixed_block_memory_manager& ) = delete;
	~fixed_block_memory_manager( ) = default;
	void operator=( fixed_block_memory_manager& ) = delete;

	void* allocate( size_t );
	size_t block_size( ) const;
	size_t capacity( ) const;
	void clear( );
	void deallocate( void* );
	bool empty( ) const;
	size_t size( ) const;

private:
	struct free_block
	{
		free_block* next;
	};
	free_block* free_ptr_ = nullptr;
	size_t block_size_ = 0;
	Arena arena_;

	int size_ = 0;
};

template<typename Arena>
template<int N>
inline fixed_block_memory_manager<Arena>::fixed_block_memory_manager( char( &a )[N] ) : arena_( a )
{
}

template<typename Arena>
inline void * fixed_block_memory_manager<Arena>::allocate( size_t size )
{
	if ( empty( ) )
	{
		free_ptr_ = reinterpret_cast<free_block*>( arena_.allocate( size ) );
		block_size_ = size;
		if ( empty( ) )
		{
			throw std::bad_alloc( );
		}
	}

	if ( size != block_size_ )
	{
		throw std::bad_alloc( );
	}

	auto p = free_ptr_;
	free_ptr_ = free_ptr_->next;
	++size_;
	return p;
}

template<typename Arena>
inline size_t fixed_block_memory_manager<Arena>::block_size( ) const
{
	return block_size_;
}

template<typename Arena>
inline size_t fixed_block_memory_manager<Arena>::capacity( ) const
{
	return arena_.capacity();
}

template<typename Arena>
inline void fixed_block_memory_manager<Arena>::clear( )
{
	free_ptr_ = nullptr;
	arena_.clear( );
}

template<typename Arena>
inline void fixed_block_memory_manager<Arena>::deallocate( void* p )
{
	if ( p == nullptr )
	{
		return;
	}

	auto fp = reinterpret_cast<free_block*>( p );
	fp->next = free_ptr_;
	free_ptr_ = fp;
	--size_;
}

template<typename Arena>
inline bool fixed_block_memory_manager<Arena>::empty( ) const
{
	return free_ptr_ == nullptr;
}

template<typename Arena>
inline size_t fixed_block_memory_manager<Arena>::size( ) const
{
	return size_;
}

struct fixed_arena_controller
{
	template <int N>
	fixed_arena_controller( char( &a )[N] );
	fixed_arena_controller( fixed_arena_controller& ) = delete;
	~fixed_arena_controller( ) = default;
	void operator=( fixed_arena_controller& ) = delete;

	void* allocate( size_t );
	size_t block_size( ) const;
	size_t capacity( ) const;
	void clear( );
	bool empty( ) const;

private:
	void* arena_ = nullptr;
	size_t arena_size_ = 0;
	size_t block_size_;
};

template<int N>
inline fixed_arena_controller::fixed_arena_controller( char( &a )[N] ) : arena_( a ), arena_size_( N ), block_size_( 0 )
{
}

void* fixed_arena_controller::allocate( size_t size )
{
	if ( !empty( ) )
	{
		return nullptr;
	}

	block_size_ = std::max( size, sizeof( void* ) );
	size_t count = capacity( );

	if ( count == 0 )
	{
		return nullptr;
	}

	char* p;
	for ( p = static_cast<char*>( arena_ ); count > 1; --count, p += size )
	{
		*reinterpret_cast<char**>( p ) = p + size;
	}
	*reinterpret_cast<char**>( p ) = nullptr;
	return arena_;
}

inline size_t fixed_arena_controller::block_size( ) const
{
	return block_size_;
}

inline size_t fixed_arena_controller::capacity( ) const
{
	return block_size_ ? ( arena_size_ / block_size_ ) : 0;
}

inline void fixed_arena_controller::clear( )
{
	block_size_ = 0;
}

inline bool fixed_arena_controller::empty( ) const
{
	return block_size_ == 0;
}
