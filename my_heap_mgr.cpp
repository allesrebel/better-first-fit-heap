#include "my_heap_mgr.h"

#include <cstdint>

struct list_item_t
{
	void* memory;
	size_t size;
	bool used;
	list_item_t* next_item;
};

void try_split_inplace( list_item_t* ptr, size_t requested )
{
	// capture the next pointer if it exists
	auto next_item = ptr->next_item;
	
	// create a new item after the requested size
	if( ptr->size > ( requested + sizeof( list_item_t ) ) )
	{
		// we split in place
		auto memory = static_cast<uint8_t*>( ptr->memory );
		auto new_slot_ptr = memory + requested;
		auto new_slot = 
			reinterpret_cast<list_item_t*>(new_slot_ptr);
		new_slot->memory = 
			new_slot_ptr 
			+ sizeof(list_item_t);
		new_slot->size = 
			ptr->size 
			- sizeof( list_item_t )
			- requested;
		new_slot->used = false;
		new_slot->next_item = next_item;

		ptr->size = requested;
		ptr->next_item = reinterpret_cast<list_item_t*>(new_slot_ptr);
	}
	// otherwise, we just take the entire block
	// and don't split, (too small of a chunk)
	else
		return;
}

managed_memory_t::managed_memory_t( 
		void* memory,
		const size_t memory_size
) : memory( memory ), memory_size( memory_size )
{
	if( memory_size <= sizeof( list_item_t ) )
		return; // throw exception! or error

	list_item_t* first_item = static_cast<list_item_t*>( memory );
	first_item->memory = 
		static_cast<uint8_t*>( memory ) 
		+ sizeof( list_item_t );
	first_item->size = memory_size - sizeof( list_item_t );
	first_item->used = false;
	first_item->next_item = nullptr;
}

void* my_malloc( 
		size_t requested_size, 
		managed_memory_t heap 
)
{
	size_t free_space = 0;
	size_t free_item_count = 0;
	for( 
			auto ptr = 
				static_cast<list_item_t*>( heap.memory );
			ptr != nullptr;
			ptr = ptr->next_item
	)
	{
		// keep track of running total of space
		if( ptr->used == false )
		{
			free_space += ptr->size;
			free_item_count++;
		}

		if( 
				ptr->used == false 
				&& ptr->size >= requested_size
		)
		{
			try_split_inplace( ptr, requested_size );
			ptr->used = true;
			return ptr->memory;
		}
	}
	// we didn't find a valid list node
	// let's try to combine blocks?
	if( 
			(
				free_space 
				+ sizeof( list_item_t ) * free_item_count 
				- sizeof( list_item_t ) 
			) >= requested_size
	)
	{
		// signal that we're too fragmented
		// first-fit defo has this problem
	}
	return nullptr;
}

void my_free( 
		void* my_ptr, 
		managed_memory_t memory 
)
{
	auto mem_ptr = static_cast<uint8_t*>(my_ptr);
	auto list_item = 
		reinterpret_cast<list_item_t*>(mem_ptr - sizeof( list_item_t ) );
	list_item->used = false;

	// attempt quick defrag, look ahead one item
	// and attempt combine if possible
	if( list_item->next_item->used == false )
	{
		auto next = list_item->next_item;
		auto next_next = next->next_item;
		list_item->size += ( next->size + sizeof(list_item_t) );
		list_item->next_item = next_next;
	}
}
