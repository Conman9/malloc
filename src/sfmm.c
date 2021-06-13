/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "sfmm.h"

/**
void sf_show_block(sf_block *bp);
void sf_show_free_list(int index);
void sf_show_free_lists();
void sf_show_quick_list(int index);
void sf_show_quick_lists();
void sf_show_heap();
**/
int num_pages = 0;
sf_block* last_block = NULL;
sf_block* last_used_block = NULL;
void place_freelist(int size, sf_block *block);
/*
*Tries to combine blocks if there is any possible coalesce
*
*/
void attempt_coalesce(sf_block *block){
	//sf_show_heap();
//sf_show_free_lists();
	//printf("Block: %lu\n", (((block->header)^MAGIC)));
	while(true){
		//if(((block->header)^MAGIC) - (((block->header)^MAGIC)&-7) >= 4){ return;}

		int size = ((block->header)^MAGIC)&-7;
		//sf_block *combine_block = NULL;

		size_t *n = (size_t *)block + (size) / sizeof(size_t);
		sf_block *next = (sf_block *)n;
		int size_next = ((next->header)^MAGIC)&-7;


		size_t *p = (size_t *)block - (size) / sizeof(size_t);
		sf_block *prev = (sf_block *)p;
		int size_prev = ((prev->header)^MAGIC)&-7;

		//	printf("NEXT: %d\n", size_next);
		//	printf("PREV: %d\n\n", size_prev);

		if(((next->header)^MAGIC) - size_next < 4 && next->header != 0){
			//Removes values from the free list
			int index = 0;
			for(int x = 0; x < NUM_FREE_LISTS; x++){
				if(size <= 32<<x) {index = x; break;}
			}

			sf_block *sentinel = &sf_free_list_heads[index];

			sentinel->body.links.prev = sentinel;//(sf_free_list_heads[index].body.links.prev)->body.links.prev;
			sentinel->body.links.next = sentinel;//(sf_free_list_heads[index].body.links.next)->body.links.next;
			//sentinel->body.links.prev = (sf_free_list_heads[index].body.links.prev)->body.links.prev;
			//sentinel->body.links.next = (sf_free_list_heads[index].body.links.next)->body.links.next;
			block->body.links.next = NULL;
			block->body.links.prev = NULL;
			next->body.links.prev = NULL;
			next->body.links.next = NULL;
			//	printf("%d\n\n\n", 43412);
			index = 0;
			for(int x = 0; x < NUM_FREE_LISTS; x++){
				if(size_next <= 32<<x) {index = x; break;}
			}

			sentinel = &sf_free_list_heads[index];

			sentinel->body.links.prev = sentinel;//(sf_free_list_heads[index].body.links.prev)->body.links.prev;
			sentinel->body.links.next = sentinel;//(sf_free_list_heads[index].body.links.next)->body.links.next;
			//sentinel->body.links.prev = (sf_free_list_heads[index].body.links.prev)->body.links.prev;
			//sentinel->body.links.next = (sf_free_list_heads[index].body.links.next)->body.links.next;
			block->body.links.next = NULL;
			block->body.links.prev = NULL;
			next->body.links.prev = NULL;
			next->body.links.next = NULL;


			int s = size + size_next;
			block->header = (s)^MAGIC;
			block -> header = (((block -> header)^MAGIC)|PREV_BLOCK_ALLOCATED)^MAGIC;

			size_t* combine_footer = (size_t *)block + s / sizeof(size_t);
			*combine_footer = block -> header;


			place_freelist(s, block);
			//sf_show_free_lists();


		}
		else if(((prev->header)^MAGIC) - size_prev < 4 && prev->header != 0){

			int index = 0;
			for(int x = 0; x < NUM_FREE_LISTS; x++){
				if(size <= 32<<x) {index = x; break;}
			}

			sf_block *sentinel = &sf_free_list_heads[index];

			sentinel->body.links.prev = sentinel;//(sf_free_list_heads[index].body.links.prev)->body.links.prev;
			sentinel->body.links.next = sentinel;//(sf_free_list_heads[index].body.links.next)->body.links.next;
			//sentinel->body.links.prev = (sf_free_list_heads[index].body.links.prev)->body.links.prev;
			//sentinel->body.links.next = (sf_free_list_heads[index].body.links.next)->body.links.next;
			block->body.links.next = NULL;
			block->body.links.prev = NULL;
			prev->body.links.prev = NULL;
			prev->body.links.next = NULL;
						//	printf("%d\n\n\n", 43412);
			index = 0;
			for(int x = 0; x < NUM_FREE_LISTS; x++){
				if(size_prev <= 32<<x) {index = x; break;}
			}

			sentinel = &sf_free_list_heads[index];

			sentinel->body.links.prev = sentinel;//(sf_free_list_heads[index].body.links.prev)->body.links.prev;
			sentinel->body.links.next = sentinel;//(sf_free_list_heads[index].body.links.next)->body.links.next;
			//sentinel->body.links.prev = (sf_free_list_heads[index].body.links.prev)->body.links.prev;
			//sentinel->body.links.next = (sf_free_list_heads[index].body.links.next)->body.links.next;
			block->body.links.next = NULL;
			block->body.links.prev = NULL;
			prev->body.links.prev = NULL;
			prev->body.links.next = NULL;


			int s = ((((block->header)^MAGIC)&-7) + (((prev->header)^MAGIC)&-7));
			block->header = (s)^MAGIC;

			block -> header = (((block -> header)^MAGIC)|PREV_BLOCK_ALLOCATED)^MAGIC;

			size_t* combine_footer = (size_t *)block + s / sizeof(size_t);
			*combine_footer = block -> header;

			place_freelist(s, block);

		}
		else{ break;}
	}
}

/*
*Given a block, it splits it into a usable piece and returns the usable piece
*
*/
void *sf_split(void *page, size_t size){

	//split free block
	sf_block *split_block = NULL;
	split_block = (sf_block *)page;
	split_block->header = ((size) | THIS_BLOCK_ALLOCATED | PREV_BLOCK_ALLOCATED)^MAGIC;
	size_t* split_footer = (size_t *)page + size / sizeof(size_t);
	*split_footer = split_block -> header;
	split_block->prev_footer = 0;

	return split_block;
}

/*
* Places a block back into the free list
*/
void place_freelist(int size, sf_block *block){
	int M = 32;
	int index = 0;
	for(int x = 0; x < NUM_FREE_LISTS; x++){
		if(size <= M<<x) {index = x; break;}
	}
	sf_block * sentinel = &sf_free_list_heads[index];
	//Only sentinel node exists
	if(sf_free_list_heads[index].body.links.next ==  &sf_free_list_heads[index]){
		sentinel->body.links.next = block;
		sentinel->body.links.prev = block;
		block->body.links.next = sentinel;
		block->body.links.prev = sentinel;
	}
	else{
		//sf_show_heap();
		block->body.links.next = sentinel->body.links.next;
		block->body.links.prev = sentinel;
		sentinel->body.links.next = block;
		block->body.links.next->body.links.prev = block;
		//sf_show_heap();
	}

}

/*
*	Allocates memory for an array
*/
void *sf_malloc(size_t size) {
	//if the request size is 0.  If so, then return NULL without setting sf_errno
	if(size <= 0){
		return NULL;
	}
	void *page = NULL;
	sf_block *block = NULL;
	sf_block *split_block = NULL;

	//initializes first block of memory if it doesnt already exist
	if(sf_mem_start() == sf_mem_end()){
		page = sf_mem_grow();

		num_pages++;
		block = (sf_block *)page;
		block->header = (PAGE_SZ - 16)^MAGIC;

		block->prev_footer = 0;

		//Initializes free_lists
		for(int x = 0; x < NUM_FREE_LISTS; x++){
			sf_free_list_heads[x].body.links.next = &sf_free_list_heads[x];
			sf_free_list_heads[x].body.links.prev = &sf_free_list_heads[x];
		}

		//if block is greater than the size needed, split the block
		//Any size less than 32, should be set to 32. If its 32, set it to 48, and so on
			if(((block->header)^MAGIC) > (size)){
				if((size+8) <= 32) size = 32;
				else if((size+8) % 16 != 0) {size = (size+8) + (16 - ((size+8)%16));}
				else if((size+8) % 16 == 0) {size = size+8;}
				//printf("%lu\n",size);
				if(4080 - size <= 32){size = 4080;}
				//splits the block
				split_block = (sf_block *)page;
				split_block->header = ((size) | THIS_BLOCK_ALLOCATED)^MAGIC;
				size_t* split_footer = (size_t *)split_block + size / sizeof(size_t);
				*split_footer = split_block -> header;
				split_block->prev_footer = 0;


				//sets the new free block
				size_t* new_block =  (size_t *)page + (size) / sizeof(size_t);
				block = (sf_block *)new_block;
				block->header = ((4080 - size)|PREV_BLOCK_ALLOCATED)^MAGIC;

				//updates the footer of the heap to match the header
				size_t* footer_block = (size_t *)page + 4080 / sizeof(size_t);
				*footer_block = block -> header;

				last_used_block = split_block;
				if((((block->header)^MAGIC)&-7) > 0){
					place_freelist(((block->header)^MAGIC)&-7, block);
						last_block = block;

				}else{
					last_block = split_block;

				}
			}
			else{
					size_t* footer_block = (size_t *)block + 4080 / sizeof(size_t);
					*footer_block = (block -> header);

				while(true){
					sf_block *new_block = NULL;
					void* newb = sf_mem_grow();
					if(newb == NULL){
						place_freelist(((block->header^MAGIC)&-7), block);
						return NULL;
					}
					else{
						new_block = newb-16;
					}

					//sf_block *new_block = sf_mem_grow()-16;
					new_block->header = (4096)^MAGIC;
					footer_block = (size_t *)block + 4096 / sizeof(size_t);
					*footer_block = (new_block -> header);
					int s = ((block->header)^MAGIC) + ((new_block->header)^MAGIC);

					sf_block *combine_block = NULL;
					combine_block = (sf_block *)block;
					combine_block->header = ((s))^MAGIC;
					size_t* split_footer = (size_t *)block + s / sizeof(size_t);
					*split_footer = combine_block -> header;
					combine_block->prev_footer = 0;

					block = combine_block;
					num_pages++;
					if(((block->header)^MAGIC) > (size)){ break;}

				}
				if((size+8) <= 32) size = 32;
				else if((size+8) % 16 != 0) {size = (size+8) + (16 - ((size+8)%16));}
				else if((size+8) % 16 == 0) {size = size+8;}
				//printf("%lu\n",((block->header)^MAGIC)&-7);
				if((((block->header)^MAGIC)&-7) - size <= 32){size =(((block->header)^MAGIC)&-7) ;}

			//place_freelist(((block->header)^MAGIC)&-7, block);
			//printf("%lu\n", size);
			//return block;
			int old_size = (block->header^MAGIC)&-7;

			//Splits the block
			split_block = sf_split(block, size);
			split_block->header =  (((split_block->header^MAGIC)&-7)|THIS_BLOCK_ALLOCATED)^MAGIC;
			//sets the new free block
			size_t* new_block =  (size_t *)block + size / sizeof(size_t);
			block = (sf_block *)new_block;
			block->header = ((old_size - size)|PREV_BLOCK_ALLOCATED)^MAGIC;
			footer_block = (size_t *)block + ((block->header^MAGIC)&-7) / sizeof(size_t);
			*footer_block = block -> header;

			last_used_block = split_block;

			//puts new block back into free_list
			if((((block->header)^MAGIC)&-7) > 0){
				place_freelist(((block->header)^MAGIC)&-7, block);
				last_block = block;

			}else{
				last_block = split_block;

			}


		}
	}
	//Runs when a heap already exists
	else {

		int in_quicklist = 0;
		int in_freelist = 0;

		//determines size
		if(size+8 <= 32) size = 32;
		else if((size+8) % 16 != 0) size = (size+8) + (16 - ((size+8)%16));
		else if((size+8) % 16 == 0) {size = size+8;}
		//printf("%lu\n", (((block->header)^MAGIC)&-7) );
		//if((((block->header)^MAGIC)&-7) - size <= 32){size =(((block->header)^MAGIC)&-7) ;}
				//sf_show_heap();


		//Check if an open block is in quicklists
		if((size-16)/32 <= 9 && sf_quick_lists[(size-16)/32].length > 0){
			block = sf_quick_lists[(size-16)/32].first;
			in_quicklist = 1;

		}
		else{

			//checks if an open block is in free_lists
			for(int x = 0; x < NUM_FREE_LISTS; x++){
				sf_block *check_block = sf_free_list_heads[x].body.links.next;
				if(check_block == &sf_free_list_heads[x]) {continue;}

				sf_block *sec_block = NULL;

				//loops through until valid free list exists
				while(check_block != sec_block){
					sec_block = sf_free_list_heads[x].body.links.next;
					if(((check_block->header)^MAGIC) >= size && check_block->header != 0){

						//sf_block* b = (sf_free_list_heads[x].body.links.next)->body.links.next;
						//b->body.links.prev = &sf_free_list_heads[x];
						//sf_free_list_heads[x].body.links.next = b;
						sf_free_list_heads[x].body.links.prev = (sf_free_list_heads[x].body.links.prev)->body.links.prev;
						sf_free_list_heads[x].body.links.next = (sf_free_list_heads[x].body.links.next)->body.links.next;
						block = check_block;
						in_freelist = 1;
						break;
					}
				}
				if(in_freelist == 1) {break;}
			}

		}
		//printf("%lu\n",(block->header^MAGIC)&-7);

		if(in_quicklist == 0 && in_freelist == 0){
			//NEED TO IMPLEMENT
			//Should add new page and redo init step from above
			//sf_show_heap();
			//sf_show_free_lists();

			sf_block* new_block = NULL;
			//Check if last block is allocated
			int pre_alloc = 0;
			if(((((last_block->header)^MAGIC) % 16) == 6) || ((last_block->header)^MAGIC) % 16 == 4){
				pre_alloc = 1;
			}

			//sf_block* new_block = sf_mem_grow()-16;

			void* newb = sf_mem_grow();
			if(newb == NULL){
				place_freelist(((block->header^MAGIC)&-7), block);
				return NULL;
			}
			else{
				new_block = newb-16;
				num_pages++;
			}

			if(pre_alloc == 1) {new_block->header = ((4096)|PREV_BLOCK_ALLOCATED)^MAGIC;}
			else { new_block->header = (4096)^MAGIC;}
			size_t* footer_block = (size_t *)new_block + 4096 / sizeof(size_t);
			*footer_block = (new_block -> header);

			//place_freelist((new_block->header^MAGIC)&-7, new_block);
			block = new_block;


			//Try to coalesce with previous block
			if(pre_alloc == 0){
				int s = ((last_block->header)^MAGIC) + ((new_block->header)^MAGIC);
				sf_block *combine_block = NULL;

				combine_block = (sf_block *)last_block;
				combine_block->header = ((s))^MAGIC;
				size_t* split_footer = (size_t *)last_block + s / sizeof(size_t);
				*split_footer = combine_block -> header;
				combine_block->prev_footer = 0;

				//Removes values from the free list
				s =(((new_block->header)^MAGIC)&-7);
				int index = 0;
				for(int x = 0; x < NUM_FREE_LISTS; x++){
					if(s <= 32<<x) {index = x; break;}
				}
				sf_free_list_heads[index].body.links.prev = (sf_free_list_heads[index].body.links.prev)->body.links.prev;
				sf_free_list_heads[index].body.links.next = (sf_free_list_heads[index].body.links.next)->body.links.next;

				s =((((last_block->header)^MAGIC)&-7) - ((new_block->header)^MAGIC));
				index = 0;
				for(int x = 0; x < NUM_FREE_LISTS; x++){
					if(s <= 32<<x) {index = x; break;}
				}
				sf_free_list_heads[index].body.links.prev = (sf_free_list_heads[index].body.links.prev)->body.links.prev;
				sf_free_list_heads[index].body.links.next = (sf_free_list_heads[index].body.links.next)->body.links.next;

				block = combine_block;

			}

			//Need to check if enough free space is aviablable in free_list. If not then loop through again
				while(true){

					sf_block *new_block = NULL;
					if(((block->header)^MAGIC) > (size)){ break;}

					void* newb = sf_mem_grow();
					if(newb == NULL){
						place_freelist(((block->header^MAGIC)&-7), block);
						return NULL;
					}
					else{
						new_block = newb-16;
						num_pages++;
					}

					//sf_block *new_block = sf_mem_grow()-16;
					new_block->header = (4096)^MAGIC;
					footer_block = (size_t *)block + 4096 / sizeof(size_t);
					*footer_block = (new_block -> header);
					int s = ((block->header)^MAGIC) + ((new_block->header)^MAGIC);

					sf_block *combine_block = NULL;
					combine_block = (sf_block *)block;
					combine_block->header = ((s))^MAGIC;
					size_t* split_footer = (size_t *)block + s / sizeof(size_t);
					*split_footer = combine_block -> header;
					combine_block->prev_footer = 0;

					block = combine_block;

				}

			in_freelist = 1;
		}

		//free block found in freelist
		if(in_quicklist == 0 && in_freelist == 1){
			//sf_show_heap();

			if((((block->header)^MAGIC)&-7) - size <= 32){size =(((block->header)^MAGIC)&-7) ;}

			int old_size = (block->header^MAGIC)&-7;

			//Splits the block
			split_block = sf_split(block, size);

			//sets the new free block
			size_t* new_block =  (size_t *)block + size / sizeof(size_t);
			block = (sf_block *)new_block;
			block->header = ((old_size - size)|PREV_BLOCK_ALLOCATED)^MAGIC;
			size_t* footer_block = (size_t *)block + ((block->header^MAGIC)&-7) / sizeof(size_t);
			*footer_block = block -> header;

			last_used_block = split_block;

			//puts new block back into free_list
			if((((block->header)^MAGIC)&-7) > 0){
				place_freelist(((block->header)^MAGIC)&-7, block);
				last_block = block;

			}else{
				last_block = split_block;

			}/*
		sf_free_list_heads[7] = *block;
		sf_free_list_heads[7].body.links.next = block;
		sf_free_list_heads[7].body.links.prev = block;
		block->body.links.next = &sf_free_list_heads[7];
		block->body.links.prev = &sf_free_list_heads[7];

		*/			//sf_show_free_lists();

		}
		//Do quicklist stuff
		if(in_quicklist == 1){

		}

	}

	//after each call to mem_grow, should attempt to coalesce the block with the one preceeding it
	//printf("%p\n\n", split_block);
    return split_block->body.payload;
}



void sf_free(void *pp) {
	//Get header data
	sf_block *block = pp - 16;
	//printf("heap start: %p\n", sf_mem_start()+8);
	//printf("block header: %lu\n",(uintptr_t)(block));
	//printf("prev_footer: %p\n",(block));
	//sf_show_block(block);


	if(block == NULL) { abort(); }
	if(((uintptr_t)block % 16) != 0){ abort(); }			 //pointer is not 16 byte aligned
	if((((block->header)^MAGIC)&-7) < 32) { abort(); }		 //less than 32 size
	if((((block->header)^MAGIC)&-7) % 16 != 0) { abort(); } //header is not multiple of 16

	if(((uintptr_t)sf_mem_start() > (uintptr_t)block) ) { abort(); } //header is before start of first block

	 if((uintptr_t)sf_mem_end() < (uintptr_t)block){ } 								 //or footer is after end of last block
	if(((((block->header)^MAGIC) % 16) != 6) && ((block->header)^MAGIC) % 16 != 4){ abort();} //The allocated bit is 0.

	/*if((((block->header)^MAGIC) - (((block->header)^MAGIC)&-7) == 0 ||
	((block->header)^MAGIC) - (((block->header)^MAGIC)&-7) == 4 ) &&
	(((block->prev_footer)^MAGIC) - (((block->prev_footer)^MAGIC)&-7) == 4 ||
	((block->prev_footer)^MAGIC) - (((block->prev_footer)^MAGIC)&-7) == 6)){abort();}//prev_alloc field is 0, but alloc field in previous block is 1
*/

	//Check if can put in quicklist
	int size = (((block->header)^MAGIC)&-7);
	int index = (size - 32)/16;
	//printf("\nindex: %d\n", index);

	//Add to quicklist if possible
	if(index < 10){
		//sf_block *test = sf_quick_lists[index].first;
		int l = sf_quick_lists[index].length;

		//If at max capacity, must flush the list
		if(l == QUICK_LIST_MAX){
			for(int x = 0; x < QUICK_LIST_MAX; x++){
				sf_block *remove = sf_quick_lists[index].first;


				sf_quick_lists[index].first = sf_quick_lists[index].first->body.links.next;
				//sf_quick_lists[index].first->body.links.prev = sf_quick_lists[index].first;
				remove->body.links.next = NULL;
				remove->body.links.prev = NULL;
				remove->header = ((((remove->header)^MAGIC)&-7)^MAGIC);
				size_t* footer_block = (size_t *)remove + size / sizeof(size_t);
				*footer_block = remove -> header;

				//printf("%p\n", combine);
				place_freelist(((remove->header)^MAGIC)&-7, remove);

				attempt_coalesce(remove);
				//sf_show_heap();

			}
			//next->header = ((((next->header)^MAGIC)&-7) | THIS_BLOCK_ALLOCATED)^MAGIC;
			//printf("%p vs %p\n", block, &block->prev_footer);
			if(((block->prev_footer)^MAGIC) - ((((block->prev_footer)^MAGIC)&-7)) == 2){
				block->header = (((((block->header)^MAGIC)&-7) | THIS_BLOCK_ALLOCATED)^MAGIC);
			}
			//printf("%lu vs %p\n", ((block)->header)^MAGIC, ((&block-8)));

			block->body.links.next = NULL;
			sf_quick_lists[index].first = block;
			sf_quick_lists[index].length += 1;			//returning the existing blocks in the list to the main pool, and then
 			//the block being freed is added to the now-empty list, leaving that list containing one block.
		}
		else if(l == 0){
			//block->body.links.prev = block;

			block->body.links.next = NULL;
			sf_quick_lists[index].first = block;
			sf_quick_lists[index].length += 1;

		}
		//else if(l == 1){
		else{

			sf_quick_lists[index].first -> body.links.prev = block;
			block->body.links.next = sf_quick_lists[index].first;
			//block->body.links.prev = block;

			sf_quick_lists[index].first = block;
			sf_quick_lists[index].length += 1;

		}
		/*else{
			block->body.links.next = sf_quick_lists[index].first ;
			sf_quick_lists[index].first -> body.links.prev = block;
			sf_quick_lists[index].first = block;
			sf_quick_lists[index].length += 1;
		}*/
		//sf_show_quick_lists();
	}
	else{
		//check for coalesce possibility
		//put in free_list
			//if(((block->prev_footer)^MAGIC) - ((((block->prev_footer)^MAGIC)&-7)) == 2){
			//	block->header = (((((block->header)^MAGIC)&-7) | THIS_BLOCK_ALLOCATED)^MAGIC);
			//
		printf("%p vs %p\n", block, &block->prev_footer);

		int p_alloc = ((block->prev_footer)^MAGIC) - (((block->prev_footer)^MAGIC)&-7);
		int t_alloc = ((block->header)^MAGIC) - (((block->header)^MAGIC)&-7);
		printf("p_alloc: %d\n", p_alloc);
		printf("t_alloc: %d\n\n", t_alloc);
			//	sf_show_heap();

/*
		if(t_alloc == 4 || t_alloc == 6) {
			if(p_alloc == 2 || p_alloc == 6){

				block->header = ((((block->header)^MAGIC)&-7)|PREV_BLOCK_ALLOCATED)^MAGIC;
				size_t* footer_block = (size_t *)block + ((block->header^MAGIC)&-7) / sizeof(size_t);
				*footer_block = block -> header;


			}
			else if(p_alloc <= 0){
				block->header = ((((block->header)^MAGIC)&-7)^MAGIC);
				size_t* footer_block = (size_t *)block + ((block->header^MAGIC)&-7) / sizeof(size_t);
				*footer_block = block -> header;

			}


		}
		*/

			block->header = ((((block->header)^MAGIC)&-7)^MAGIC);
			size_t* footer_block = (size_t *)block + ((block->header^MAGIC)&-7) / sizeof(size_t);
			*footer_block = block -> header;
			//sf_block *b = (sf_block *)footer_block;
/*
			if(((block->prev_footer)^MAGIC) - ((((block->prev_footer)^MAGIC)&-7)) == 2 ||
				((block->prev_footer)^MAGIC) - ((((block->prev_footer)^MAGIC)&-7)) == 6 ){
				block->header = (((((block->header)^MAGIC)&-7) | PREV_BLOCK_ALLOCATED)^MAGIC);
				size_t* footer_block = (size_t *)block + ((block->header^MAGIC)&-7) / sizeof(size_t);
				*footer_block = block -> header;

			}

			if(((b->header)^MAGIC) - ((((b->header)^MAGIC)&-7)) == 4 ||
				((b->header)^MAGIC) - ((((b->header)^MAGIC)&-7)) == 6){
				b->header = (((((b->header)^MAGIC)&-7) | PREV_BLOCK_ALLOCATED)^MAGIC);
			}
			*/
			//b->header = ((((b->header)^MAGIC)&-7 ) | THIS_BLOCK_ALLOCATED )^MAGIC;

			place_freelist(((block->header)^MAGIC), block);
			attempt_coalesce(block);
			sf_show_heap();

	}
	//Attempt to coalesce each time sf_free is called
	//sf_show_heap();
    return;
}

void *sf_realloc(void *pp, size_t rsize) {
	sf_block *block = pp - 16;

	if(block == NULL) { abort(); }
	if(((uintptr_t)block % 16) != 0){ abort(); }			 //pointer is not 16 byte aligned
	if((((block->header)^MAGIC)&-7) < 32) { abort(); }		 //less than 32 size
	if((((block->header)^MAGIC)&-7) % 16 != 0) { abort(); } //header is not multiple of 16

	if(((uintptr_t)sf_mem_start() > (uintptr_t)block) ) { abort(); } //header is before start of first block

	 if((uintptr_t)sf_mem_end() < (uintptr_t)block){ } 								 //or footer is after end of last block
	if(((((block->header)^MAGIC) % 16) != 6) && ((block->header)^MAGIC) % 16 != 4){ abort();} //The allocated bit is 0.

	if(rsize == 0) {sf_free(pp); return NULL;}

	//bigger
	if((((block->header)^MAGIC)&-7) < rsize){
		double *ptr = sf_malloc(rsize);
		if(ptr == NULL) {return NULL;}

		memcpy(ptr, block+16, 8);
		sf_free((sf_block *)pp);
		//printf("%d\n", 123);
		return ptr;

	}
		//smaller
	else if((((block->header)^MAGIC)&-7) > rsize){
		if(((((block->header)^MAGIC)&-7) - rsize) < 32){
				sf_block *split_block = block;
				split_block->header = ((rsize) | THIS_BLOCK_ALLOCATED)^MAGIC;
				size_t* split_footer = (size_t *)split_block + rsize / sizeof(size_t);
				*split_footer = split_block -> header;
				split_block->prev_footer = 0;


				//sets the new free block
				size_t* new_block =  (size_t *)block + (rsize) / sizeof(size_t);
				block = (sf_block *)new_block;
				block->header = (((((block->header)^MAGIC)^-7) - rsize)|PREV_BLOCK_ALLOCATED)^MAGIC;

				//updates the footer of the heap to match the header
				size_t* footer_block = (size_t *)block + ((((block->header)^MAGIC)^-7)) / sizeof(size_t);
				*footer_block = block -> header;

				place_freelist(((block->header)^MAGIC)&-7, block);
				return split_block;
			}
			else{
				return pp;

			}

	}
    return NULL;
}
