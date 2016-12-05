#include "cache.h"

int Cache::replaceLRU(int index){
	int position = block_queue[index][0];
    for (int i = 1; i < config_.associativity; ++ i){
        block_queue[index][i - 1] = block_queue[index][i];
    }
    block_queue[index][config_.associativity - 1] = position;
    return position;
}