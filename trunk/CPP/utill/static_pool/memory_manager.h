#pragma once
//#include <boost/thread.hpp>
//#include <process.h>
//#include <hash_map>
#include <windows.h>
#include <map>
//#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <iostream>
#include <cassert>
#include "../block_map/block_map.hpp"

namespace umtl
{
	class fast_allocator
	{
	public:
		static void * alloc( size_t size );
		static void dealloc( void * p );
	};

	class block_partition
	{
	public:
		typedef block_map< char, fast_allocator > Partitions;

		Partitions partitions_;
		size_t size_;

		block_partition( size_t need_size = 0 ) : size_(need_size) { resize( need_size ); }
		
		inline size_t size()
		{
			return size_;
		}

		inline bool empty() 
		{
			return size() == 0;
		}

		inline void resize( size_t need_size )
		{
			clear();
			partitions_.resize( need_size );
			size_ = need_size;
		}

		inline void * pop( size_t need_size )
		{
			return partitions_.pop( need_size );
		}

		inline void push( void * mem )
		{
			partitions_.push( mem );
		}

	private:
		inline void clear()
		{
			partitions_.clear();
			size_ = 0;
		}

		block_partition( block_partition const & other );
		void operator=( block_partition const & other );
	};

	class memory_manager
	{
	public:
		//typedef boost::recursive_mutex Mutex;
		typedef CRITICAL_SECTION Mutex;
		//typedef boost::lock_guard<Mutex> Locker;
		typedef struct cLocker { 
			cLocker(CRITICAL_SECTION & c) : ct(c) { EnterCriticalSection(&ct); }
			~cLocker() { LeaveCriticalSection(&ct); }
			CRITICAL_SECTION & ct;
		} Locker;

		//typedef std::tr1::shared_ptr< block_partition > block_ptr;
		//typedef std::vector<block_ptr> Addresss;
		typedef std::tr1::unordered_set< block_partition* > Partitions;
		typedef std::map<size_t,Partitions> Blocks;

		typedef std::vector< char* > SmallBuffers;
		typedef std::map< size_t, SmallBuffers > SmallBlocks;

		void * alloc( size_t size );
		void free( void * p );
		void * alloc_array( size_t size ); 
		void free_array( void * p );
		void clear();

		size_t current_size();
		size_t alloc_total();
		size_t dealloc_total();

		inline static memory_manager & get() {
			static memory_manager m;
			return m;
		}                     

		~memory_manager() { 
			clear();
			DeleteCriticalSection( &mutex );
		}
		

	private:
		Mutex mutex;
		Blocks blocks;
		SmallBlocks small_blocks;
		size_t pool_size;
		size_t alloc_cumul;
		size_t dealloc_cumul;

		void * big_alloc( size_t size );
		void * new_big_mem( size_t size );
		void * search_big_mem( Blocks::iterator blockIter, size_t size );
		void big_free( void * p );

		void * small_alloc( size_t size );
		void small_free( void * p );

		static void outOfMem()
		{
			std::cerr<< "Unable to satisfy request for memory\n";
			//std::abort();
			throw std::bad_alloc();
		}

		memory_manager() : pool_size(0), alloc_cumul(0), dealloc_cumul(0) { 
			InitializeCriticalSection(&mutex);
			std::set_new_handler( outOfMem );
		}
		
		static int const _small_size = 128;
	};
}