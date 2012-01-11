#pragma once
#include "memory_manager.h"
#include <algorithm>
#include <ctime>

namespace umtl
{
	//-----------------------------------------------------------------------
	//

	size_t memory_manager::current_size()
	{
		Locker l(mutex);
		return pool_size;
	}

	//-----------------------------------------------------------------------
	//

	size_t memory_manager::alloc_total()
	{
		Locker l(mutex);
		return alloc_cumul;
	}

	//-----------------------------------------------------------------------
	//

	size_t memory_manager::dealloc_total()
	{
		return dealloc_cumul;
	}

	//-----------------------------------------------------------------------
	//

	void * memory_manager::alloc( size_t size )
	{
		void * p = 0;

		if( size > (size_t)_small_size )
		{
			p = big_alloc( size );
		}
		else
		{
			p = small_alloc( size );
		}

		return p;
	}

	//////////////////////////////////////////////////////////////////////////
	//

	void * memory_manager::big_alloc( size_t size )
	{
		Locker l(mutex);

		void * p;

		auto i = blocks.lower_bound( size );

		if( i == blocks.end() )
		{
			p = new_big_mem( size );
		}
		else
		{
			for( ; i!=blocks.end(); ++ i )
			{
				p = search_big_mem( i, size );

				if( p )
					break;
			}

			if( !p )
			{
				p = new_big_mem( size );
			}
			else
			{
				//pool_size -= *((size_t*)p - 1);
				pool_size -= size;
			}
		}

		return p;
	}

	//////////////////////////////////////////////////////////////////////////
	//

	void * memory_manager::small_alloc( size_t size )
	{
		Locker l(mutex);

		void * p;

		auto i = small_blocks.lower_bound( size );

		if( i == small_blocks.end() || i->second.empty() )
		{
			p = fast_allocator::alloc( size + sizeof(size_t) );
			size_t * s = (size_t*)p;
			s[0] = size;
			p = &s[1];
		}
		else
		{
			p = i->second.back();

			i->second.pop_back();

			pool_size -= size;
		}

		return p;
	}

	//-----------------------------------------------------------------------
	//

	void memory_manager::free( void * p )
	{
		if(!p) return;

		size_t * rp = (size_t *)(p) - 1;

		size_t size = rp[0];

		if( size > (size_t)_small_size )
		{
			big_free( p );
		}
		else
		{
			small_free( p );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//

	void memory_manager::big_free( void * p )
	{
		if(!p) return;

		size_t * rp = (size_t *)(p) - 2;

		size_t size = rp[1];

		block_partition * partition_address = (block_partition*)(rp[0]);

		{
			Locker l(mutex);

			assert( partition_address );

			if( partition_address )
			{
				partition_address->push( rp );
				blocks[size].insert( partition_address );
			}

			pool_size += size;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//

	void memory_manager::small_free( void * p )
	{
		if(!p) return;

		size_t * rp = (size_t *)(p) - 1;

		size_t size = rp[0];

		{
			Locker l(mutex);

			small_blocks[size].push_back( (char*)p );

			pool_size += size;
		}
	}

	//-----------------------------------------------------------------------
	//

	void * memory_manager::new_big_mem( size_t size )
	{
		size_t need_size = size + sizeof(size_t) + sizeof(size_t);

		block_partition * block = new block_partition( need_size );

		void * p = block->pop( need_size );

		if(p)
		{
			size_t * s = (size_t*)p;
			s[0] = (size_t)block;
			s[1] = size;
			p = &s[2];
			alloc_cumul += size;
		}
		else
		{
			throw std::bad_alloc();
		}

		return p;
	}

	//-----------------------------------------------------------------------
	//

	void * memory_manager::alloc_array( size_t size )
	{
		return alloc(size);
	}

	//-----------------------------------------------------------------------
	//

	void memory_manager::free_array( void * p )
	{
		return free(p);
	}

	//-----------------------------------------------------------------------
	//

	void * memory_manager::search_big_mem( Blocks::iterator blockIter, size_t size )
	{
		void * p = 0;

		Partitions & partitions = blockIter->second;

		for( auto i = partitions.begin(); i != partitions.end(); ++i )
		{
			p = (*i)->pop( size + sizeof(size_t) + sizeof(size_t) );

			if( p )
			{
				size_t * s = (size_t*)p;
				s[0] = (size_t)(*i);
				s[1] = blockIter->first;
				p = &s[2];
				break;
			}
		}

		return p;
	}

	//-----------------------------------------------------------------------
	//

	void memory_manager::clear()
	{
		for( auto i=blocks.begin(); i!=blocks.end(); ++i )
		{
			for( auto j=i->second.begin(); j!=i->second.end(); ++j )
			{
				if( *j )
					delete *j;
			}
		}

		blocks.clear();

		for( auto i=small_blocks.begin(); i!=small_blocks.end(); ++i )
		{
			for( auto j=i->second.begin(); j!=i->second.end(); ++j )
			{
				size_t * s = (size_t*)(*j);

				if( s )
					fast_allocator::dealloc( s-1 );
			}
		}

		small_blocks.clear();

		dealloc_cumul += pool_size;

		pool_size = 0;
	}

	//-----------------------------------------------------------------------
	//

	void * fast_allocator::alloc( size_t size )
	{
		static const int _max_try_count = 10000;

		int try_count = 0;

		void * block = 0;

		for(;;)
		{
			block = malloc( size );

			if( block )
				break;

			if( ++try_count > _max_try_count )
				break;

			// delay
			clock_t startTime = clock();
			static const clock_t max_delay_time = 5;
			for(;;)
			{
				if( clock() - startTime > max_delay_time )
					break;
			}
		}

		return block;
	}

	//-----------------------------------------------------------------------
	//

	void fast_allocator::dealloc( void * p )
	{
		if( p )
			::free(p);
	}

	//-----------------------------------------------------------------------
}