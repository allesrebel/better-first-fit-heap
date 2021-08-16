#include <cstdio>
#include <cstdlib>

#include "my_heap_mgr.h"

int main()
{

	managed_memory_t heap { malloc(512), 512 };

	printf("heap starts at %p \n", heap.memory );

	auto ptr { 
		static_cast<uint8_t*>( my_malloc( 10, heap ) ) 
	};

	printf("calcuated size of bookkeeping is %lu \n", ptr - static_cast<uint8_t*>( heap.memory ));

	printf("ptr is at %p \n", ptr);
	
	auto more_mem { 
		static_cast<uint8_t*>( my_malloc( 100, heap ) ) 
	};

	printf("moremem_ptr is at %p \n", more_mem );

	auto even_more_mem { 
		static_cast<uint8_t*>( my_malloc( 400, heap ) ) 
	};

	printf("even_more_ptr is at %p \n", even_more_mem );

	my_free( more_mem, heap );

	auto round_2_even_more_mem { 
		static_cast<uint8_t*>( my_malloc( 400, heap ) ) 
	};

	// first fit's weakness
	printf("r2 is at %p \n", round_2_even_more_mem );
	my_free( round_2_even_more_mem, heap );
}
