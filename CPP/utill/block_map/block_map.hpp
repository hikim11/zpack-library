#pragma once
#include <map>
//#include <unordered_map>
#include <cassert>

namespace umtl
{
	//////////////////////////////////////////////////////////////////////////
	//

	struct BlockMapDefaultAllocator
	{
		static void * alloc( size_t size ) { return size ? new char[size] : 0; }
		static void dealloc( void * p ) { if( p ) delete[] p; }
	};

	//////////////////////////////////////////////////////////////////////////
	//

	template< typename DataType, typename Allocator = BlockMapDefaultAllocator >
	class block_map
	{
	public:
		typedef DataType										value_t;
		typedef std::multimap< size_t, __int64 >				SizeMap;
		typedef std::map< __int64, SizeMap::iterator >			AddressMap;
		typedef std::map< __int64, size_t >						PopList;

		block_map( size_t block_size = 0 ) : size_( block_size ), start_addr_(0) { resize( block_size ); }

		inline size_t size() { return size_; }

		void resize( size_t block_size ) 
		{ 
			clear(); 
			size_ = block_size;
			start_addr_ = (__int64)Allocator::alloc( size_ );
			insert( size_, start_addr_ );
		}

		void * pop( size_t need_size )
		{
			if( need_size == 0 )
				return 0;

			if( size_map_.empty() || address_map_.empty() )
				return 0;

			auto sizeIt = size_map_.lower_bound( need_size );

			if( sizeIt == size_map_.end() )
				return 0;

			size_t partition_size = sizeIt->first;
			__int64 partition_address = sizeIt->second;

			auto addrIt = address_map_.find( partition_address );
			assert( addrIt != address_map_.end() );

			address_map_.erase( addrIt );
			size_map_.erase( sizeIt );

			size_t rest_size = partition_size - need_size;
			__int64 rest_address = partition_address + need_size;

			if( rest_size )
				insert( rest_size, rest_address );

			pop_list_.insert( std::make_pair( partition_address, need_size ) );

			return (void*)partition_address;
		}

		void push( void * p )
		{
			if( !p || pop_list_.empty() )
				return;

			__int64 mem_address = (__int64)p;

			auto popIt = pop_list_.find( mem_address );

			assert( popIt != pop_list_.end() );

			size_t mem_size = popIt->second;

			pop_list_.erase( popIt );

			
			auto next_address = address_map_.lower_bound( mem_address );

			auto prev_address = next_address;

			if( !address_map_.empty() && prev_address != address_map_.begin() )
				--prev_address;
			else
				prev_address = address_map_.end();

			push( prev_address, mem_address, mem_size, next_address );
		}

		void clear()
		{
			address_map_.clear();
			size_map_.clear();
			pop_list_.clear();

			if( start_addr_ )
				Allocator::dealloc( (void*)start_addr_ );

			start_addr_ = 0;
			size_ = 0;
		}

	private:

		void push( AddressMap::iterator prev_address, __int64 mem_address, size_t mem_size, AddressMap::iterator next_address )
		{
			// 1. 앞뒤 파티션이 모두 있는 경우
			if( prev_address != address_map_.end() && next_address != address_map_.end() )
			{
				// 1.1. 이전 파티션과 이어져 있는 경우
				if( (prev_address->first + prev_address->second->first) == mem_address )
				{
					// 1.1.1 이전 파티션과 이어지고 다음 파티션과도 이어져 있는 경우
					if( (mem_address + mem_size) == next_address->first )
					{
						// 3개의 파티션을 하나로 합친다.
						merge( prev_address, mem_address, mem_size, next_address );
					}
					// 1.1.2 이전 파티션과 이어지고 다음 파티션과는 이어지지 않은 경우
					else
					{
						// 이전 파티션과 하나로 합친다.
						merge( prev_address, mem_address, mem_size );
					}
				}
				// 1.2. 이전 파티션과 이어지지 않은 경우
				else
				{
					// 1.2.1 다음 파티션과 이어진 경우
					if( (mem_address + mem_size) == next_address->first )
					{
						// 다음 파티션과 합친다.
						merge( mem_address, mem_size, next_address );
					}
					// 1.2.2 다음 파티션과 이어지지 않은 경우
					else
					{
						// 새로운 파티션을 등록한다.
						insert( mem_size, mem_address );
					}
				}
			}
			// 2. 이전 파티션만 있는 경우
			else if( prev_address != address_map_.end() )
			{
				// 2.1. 이전 파티션과 이어진 경우
				if( (prev_address->first + prev_address->second->first) == mem_address )
				{
					// 이전 파티션과 합친다.
					merge( prev_address, mem_address, mem_size );
				}
				// 2.2. 이전 파티션과 이어지지 않은 경우
				else
				{
					// 새로운 파티션을 등록한다.
					insert( mem_size, mem_address );
				}
			}
			// 3. 다음 파티션만 있는 경우
			else if( next_address != address_map_.end() )
			{
				// 3.1. 다음 파티션과 이어진 경우
				if( (mem_address + mem_size) == next_address->first )
				{
					// 다음 파티션과 합친다.
					merge( mem_address, mem_size, next_address );
				}
				// 3.2. 다음 파티션과 이어지지 않은 경우
				else
				{
					// 새로운 파티션을 등록한다.
					insert( mem_size, mem_address );
				}
			}
			// 4. 파티션이 하나도 없는경우
			else if( address_map_.empty() )
			{
				// 새로운 파티션을 등록한다.
				insert( mem_size, mem_address );
			}
		}

		void merge( AddressMap::iterator & prev_address, __int64 mem_address, size_t mem_size, AddressMap::iterator & next_address )
		{
			size_t new_mem_size = prev_address->second->first + mem_size + next_address->second->first;
			__int64 new_mem_address = prev_address->first;

			size_map_.erase( next_address->second );
			size_map_.erase( prev_address->second );

			address_map_.erase( prev_address++ );
			address_map_.erase( prev_address );

			insert( new_mem_size, new_mem_address );
		}

		void merge( AddressMap::iterator & prev_address, __int64 mem_address, size_t mem_size )
		{
			size_t new_mem_size = prev_address->second->first + mem_size;
			__int64 new_mem_address = prev_address->first;

			size_map_.erase( prev_address->second );
			address_map_.erase( prev_address );

			insert( new_mem_size, new_mem_address );
		}

		void merge( __int64 mem_address, size_t mem_size, AddressMap::iterator & next_address )
		{
			size_t new_mem_size = mem_size + next_address->second->first;
			__int64 new_mem_address = mem_address;

			size_map_.erase( next_address->second );
			address_map_.erase( next_address );

			insert( new_mem_size, new_mem_address );
		}

		void insert( size_t block_size, __int64 addr )
		{
			if( block_size && addr )
			{
				address_map_.insert( 
					std::make_pair( addr, 
						size_map_.insert( std::make_pair( block_size, addr ) )
					)
				);
			}
		}

		SizeMap		size_map_;
		AddressMap	address_map_;
		PopList		pop_list_;

		size_t		size_;
		__int64		start_addr_;
	};

	//////////////////////////////////////////////////////////////////////////
}