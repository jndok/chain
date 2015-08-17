//
//  chain.c
//  alloc
//
//  Created by jndok on 17/08/15.
//  Copyright (c) 2015 jndok. All rights reserved.
//

#include "chain.h"

void *get_block_base_address(void *block_addr)
{
    return block_addr-sizeof(mem_block_t);
}

mem_block_size_t get_block_size(void *block_addr)
{
    return *(mem_block_size_t*)(get_block_base_address(block_addr)+sizeof(mem_block_t*));
}

mem_block_lock_t get_block_lock(void *block_addr)
{
    return *(mem_block_lock_t*)(get_block_base_address(block_addr)+sizeof(mem_block_t*)+sizeof(mem_block_size_t)) & ~0x7FFE;
}

void *alloc_block(mem_block_size_t size)
{
    void *block_addr=NULL;
    mem_block_t metadata;
    vm_address_t *start_addr=0;
    
    vm_allocate(mach_task_self_, (vm_address_t*)&start_addr, size+sizeof(mem_block_t), 1);
    block_addr=(start_addr);
    metadata.next=(void*)NULL;
    metadata.sz=(unsigned int)size;
    metadata.locked=0x0;
    
    *(mem_block_t*)block_addr = metadata;
    
#ifdef DBG
    __chain_dbg(stdout, "[+] allocated block of size %d @ %p\n", size, block_addr);
#endif
    
    return block_addr+sizeof(mem_block_t);
}

void dealloc_block(void *block_addr)
{
    vm_deallocate(mach_task_self_, (vm_address_t)get_block_base_address(block_addr), get_block_size(block_addr));
#ifdef DBG
    __chain_dbg(stdout, "[+] deallocated block @ %p\n", block_addr-sizeof(mem_block_t));
#endif
}

void *alloc_block_chain(unsigned int blocks, mem_block_size_t size)
{
    void *head=NULL, *old_block=NULL, *curr=NULL;
    head = alloc_block(size);
    old_block = head;
    for (int i=0; i<blocks-1; i++) {
        curr = alloc_block(size);
        ((mem_block_t*)old_block)->next=curr;
        old_block=curr;
    }
    
    return head;
}

void dealloc_block_chain(void *block_addr_start)
{
    void *curr_block=NULL, *prev_block=NULL;
    curr_block=block_addr_start;
    while (curr_block != NULL) {
        if (prev_block != NULL)
            dealloc_block(prev_block);
        
        if (((mem_block_t*)curr_block)->next) {
            prev_block=curr_block;
            curr_block=((mem_block_t*)curr_block)->next;
        } else {
            dealloc_block(curr_block);
            curr_block=NULL;
        }
    }
}

/*
void dealloc_block_chain_ex(void *block_addr_start)
{
    void *curr_block, *prev_block;
    curr_block=block_addr_start;
    for (int i=0;; i++) {
        if (prev_block)
            dealloc_block(prev_block);
        if (((mem_block_t*)curr_block)->next == NULL) {
            dealloc_block(curr_block);
            printf("dealloc'd block: %p\n", prev_block);
            break;
        } else {
            prev_block=curr_block;
            curr_block=((mem_block_t*)curr_block)->next;
            printf("dealloc'd block #%d: %p\n", i, curr_block);
        }
    }
}
*/
 
int lock_block(void *block_addr)
{
    mem_block_lock_t *lock = (mem_block_lock_t*)(get_block_base_address(block_addr)+sizeof(mem_block_t*)+sizeof(mem_block_size_t));
    *lock = 0x1 & ~*lock;
    return *lock;
}

/* this is über-bugged. please avoid. */
/*
void *copy_to_block(void *block_addr, const void *src, size_t sz)
{
    void *written=NULL;
    
    if (get_block_lock(block_addr) != 0) {
        __chain_dbg(stderr, "[!] block is locked. cannot copy.\n");
        goto out;
    }
    
    if (sz > 0 && sz <= get_block_size(block_addr)) {
        mem_block_size_t block_sz = get_block_size(block_addr);
        size_t write_sz=1;
        for (int i=0; i<block_sz; i++) {
            if (*(int*)(block_addr+i) == 0x0) {
                if (write_sz == sz) {
                    printf("i: %lu\n", ((i+1)-write_sz));
                    written=block_addr+((i+1)-write_sz+1);
                    memcpy(written, src, sz);
                    break;
                }
                write_sz++;
            } else {
                write_sz = 1;
            }
        }
    } else {
        __chain_dbg(stderr, "[!] invalid parameters, cannot copy.\n");
    }
    
out:
    return written;
}
*/