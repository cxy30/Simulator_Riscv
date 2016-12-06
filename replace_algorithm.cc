#include "cache.h"

int Cache::GetReplacePosition(int index){
	int position = block_queue[index][0];
    for (int i = 1; i < config_.associativity; ++ i){
        block_queue[index][i - 1] = block_queue[index][i];
    }
    block_queue[index][config_.associativity - 1] = position;
    return position;
}

void Cache::HitCache(uint64_t index, uint64_t position){
    for (int i = 0; i < config_.associativity; ++ i){
        if (block_queue[index][i] == position){
            for (int j = i + 1; j < config_.associativity; ++ j)
                block_queue[index][j - 1] = block_queue[index][j];
            block_queue[index][config_.associativity - 1] = position;
            break; 
        }
    }
}
