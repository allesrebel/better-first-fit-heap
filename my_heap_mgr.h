#pragma once
#ifndef _MYHEAP_H
#define _MYHEAP_H

#include <cstddef>

struct managed_memory_t
{
	void* memory;
	const size_t memory_size;
	managed_memory_t( void*, const size_t );
};

void* my_malloc( size_t, managed_memory_t);

void my_free( void*, managed_memory_t );

#endif //_MYHEAP_H
