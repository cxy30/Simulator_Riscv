#include "cache.h"
#include "def.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

int SetBitNum(uint64_t num){
    int bit = 0;
    while(num != 1){
        int t = num & 1;
        if (t == 1){
            printf("input not pow(2)\n");
            exit(0);
        }
        num = num >> 1;
        bit ++;
    }
    return bit;
}

Cache::Cache(int size, int set, int way, bool write_through, bool write_allocate, Storage *storage){
    //printf("init cache\n");
    config_.size = size;
    config_.associativity = way;
    config_.set_num = set;
    config_.write_allocate = write_allocate;
    config_.write_through = write_through;
    config_.block_size = size / way / set;
    config_.block_bit = SetBitNum(config_.block_size);
    config_.index_bit = SetBitNum(config_.set_num);
    config_.tag_bit = 64 - config_.index_bit - config_.block_bit;
    //printf("tag_bit: %d index_bit: %d block_bit: %d\n", config_.tag_bit, config_.index_bit, config_.block_bit);
    //printf("size %d, associativity %d, set_num %d\n", size, way, set);
    lower_ = storage;
    block_queue = new int*[config_.set_num];
    for(int i = 0; i < config_.set_num; ++ i){
        block_queue[i] = new int[config_.associativity];
        for (int j = 0; j < config_.associativity; ++ j)
            block_queue[i][j] = j;
    }

    cache_addr = new Block*[set];
    for (int i = 0; i < set; ++ i){
        cache_addr[i] = new Block[way];
        for (int j = 0; j < way; ++ j){
            Block tmp;
            tmp.valid = FALSE;
            tmp.dirty = FALSE;
            tmp.content = new char[config_.block_size];
            cache_addr[i][j] = tmp;
        }
    }

    stats_.access_counter = 0;
    stats_.miss_num = 0;
    stats_.access_time = 0;
    stats_.replace_num = 0;
    stats_.fetch_num = 0;
    stats_.prefetch_num = 0;

}

Cache::~Cache(){
    for (int i = 0; i < config_.set_num; ++ i){
        for (int j = 0; j < config_.associativity; ++ j)
            delete cache_addr[i][j].content;
        delete cache_addr[i];
    }
    for (int i = 0; i < config_.set_num; ++ i)
        delete block_queue[i];
    delete block_queue;
    delete cache_addr;
}

void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, int &hit, int &time) {
    uint64_t initial_addr = addr;
    char* initial_content = content;
    int ini_total_bytes = bytes; 

    stats_.access_counter ++;
    stats_.access_time += latency_.bus_latency;

    hit = 0;
    time = 0;
    uint64_t offset = addr & ((1 << config_.block_bit) - 1);
    uint64_t index = (addr >> config_.block_bit) & ((1 << config_.index_bit) - 1);
    uint64_t tag = addr >> (config_.block_bit + config_.index_bit);
    if (offset + bytes > config_.block_size){
        printf("content not in a single block\n");
    exit(0);
    }
    int position = 0;
    for (int i = 0; i < config_.associativity; ++ i){
        if (cache_addr[index][i].tag == tag && cache_addr[index][i].valid == TRUE){
            hit = 1;
            position = i;
            break;
        }
    }

    if (hit == 0)
        stats_.miss_num ++;
    else
        stats_.access_time += latency_.hit_latency;

    int lower_hit = -1, lower_time = 0;
    if (read == 1){
        if (hit == 1)
            HitCache(index, position);
        else{
            char *lower_content = new char[config_.block_size];
            uint64_t lower_addr = addr - offset;
            stats_.fetch_num ++;
            lower_->HandleRequest(lower_addr, config_.block_size, 1,
                lower_content, lower_hit, lower_time);
            position = ReplacePlace(index, tag, lower_content);
            delete lower_content;
        }

        for (int i = 0; i < bytes; ++ i)
            content[i] = cache_addr[index][position].content[offset + i];
    }
    else{
        if (hit == 1){
            HitCache(index, position);
            WriteCache(index, position, offset, bytes, content);
            if (config_.write_through == 1){
                lower_->HandleRequest(addr, bytes, 0, 
                    content, lower_hit, lower_time);
            }
        }
        else{
            if (config_.write_allocate == 1){
                char *lower_content = new char[config_.block_size];
                uint64_t lower_addr = addr - offset;
                stats_.fetch_num ++;
                lower_->HandleRequest(lower_addr, config_.block_size, 1,
                    lower_content, lower_hit, lower_time);
                position = ReplacePlace(index, tag, lower_content);
                WriteCache(index, position, offset, bytes, content);
                if (config_.write_through == 1){
                    int lower_hit, lower_time;
                    lower_->HandleRequest(addr, bytes, 0, 
                        content, lower_hit, lower_time);
                }
                delete lower_content;
            }
            else{
                lower_->HandleRequest(addr, bytes, 0, 
                    content, lower_hit, lower_time);
            }
        }
    }
}

int Cache::ReplacePlace(uint64_t index, uint64_t tag, char* content){
    stats_.replace_num ++;

    int position = replaceLRU(index);


    if (config_.write_through == 0 && cache_addr[index][position].valid == TRUE && cache_addr[index][position].dirty == TRUE){
        int lower_hit, lower_time;
        uint64_t lower_addr = (cache_addr[index][position].tag << (config_.block_bit + config_.index_bit))
            + (index << (config_.block_bit)); 
        lower_->HandleRequest(lower_addr, config_.block_size, 0,
                cache_addr[index][position].content, lower_hit, lower_time);
    }


    for (int i = 0; i < config_.block_size; ++ i)
        cache_addr[index][position].content[i] = content[i];
    cache_addr[index][position].dirty = FALSE;
    cache_addr[index][position].valid = TRUE;
    cache_addr[index][position].tag = tag;
    return position;
}

void Cache::WriteCache(uint64_t index, uint64_t position, uint64_t offset, int bytes, char* content){
    for (int i = 0; i < bytes; ++ i)
        cache_addr[index][position].content[offset + i] = content[i];
    cache_addr[index][position].dirty = TRUE;
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
