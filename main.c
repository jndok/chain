#include "chain/chain.h"

#define BLOCK_NO 16

int main(int argc, const char * argv[]) {
    
    puts("[i] allocating memory...");
    void *p = alloc_block_chain(BLOCK_NO, PAGE_SIZE);
    puts("[i] press enter to free memory...");
    getchar();
    dealloc_block_chain(p);
    
    return 0;
}
