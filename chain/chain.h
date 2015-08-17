//
//  chain.h
//  alloc
//
//  Created by jndok on 17/08/15.
//  Copyright (c) 2015 jndok. All rights reserved.
//

#ifndef __alloc__chain__
#define __alloc__chain__

#define DBG
#define __chain_dbg fprintf

#include <mach/mach.h>
#include <stdlib.h>
#include <stdio.h>
//#include <malloc/malloc.h>

typedef unsigned int mem_block_size_t;
typedef unsigned int mem_block_lock_t;

typedef struct mem_block {
    struct mem_block *next;
    mem_block_size_t sz;
    mem_block_lock_t locked:1;
} mem_block_t;

void *get_block_base_address(void *block_addr);
mem_block_size_t get_block_size(void *block_addr);
mem_block_lock_t get_block_lock(void *block_addr);

void *alloc_block(mem_block_size_t size);
void dealloc_block(void *block_addr);

void *alloc_block_chain(unsigned int blocks, mem_block_size_t size);
void dealloc_block_chain(void *block_addr_start);

int lock_block(void *block_addr);

//void *copy_to_block(void *block_addr, const void *src, size_t sz);

#endif /* defined(__alloc__chain__) */
