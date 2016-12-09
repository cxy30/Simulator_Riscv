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
    block_lastuse = new int *[config_.set_num];
    block_enter = new int*[config_.set_num];
    prefetch_tag = new int*[config_.set_num];
    for(int i = 0; i < config_.set_num; ++ i){
        block_lastuse[i] = new int[config_.associativity];
        block_enter[i] = new int[config_.associativity];
        prefetch_tag[i] = new int[config_.associativity];
        for (int j = 0; j < config_.associativity; ++ j)
            {
                block_lastuse[i][j]=-1;
                block_enter[i][j]=-1;
                prefetch_tag[i][j]=0;
            }
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
        {
            delete block_lastuse[i];
            delete block_enter[i];
            delete prefetch_tag[i];
        } 
    delete block_lastuse;
    delete block_enter;
    delete cache_addr;
    delete prefetch_tag;
}

void Cache::HandleRequest(uint64_t addr, int bytes, int read,
                          char *content, bool prefetch) {
    uint64_t initial_addr = addr;
    char* initial_content = content;
    int ini_total_bytes = bytes; 

    if (prefetch == FALSE){
        stats_.access_counter ++;
        stats_.access_time += latency_.bus_latency;
    }
    else{
        stats_.prefetch_num ++;
    }

    int hit = 0;
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

    if (prefetch == FALSE){
        if (hit == 0)
            stats_.miss_num ++;
        //else
        stats_.access_time += latency_.hit_latency;
    }
    if(!prefetch)
    {
        if(hit==1)
        {
            if(prefetch_tag[index][position]==1)
                {

                    //prefetch_tag[index][position]=1;
                    char temp[64];
                    this->HandleRequest(addr+config_.block_size,1,1,temp,true);
                    this->HandleRequest(addr+2*config_.block_size,1,1,temp,true);
                    this->HandleRequest(addr+3*config_.block_size,1,1,temp,true);
                }
            else
            {
                    char temp[64];
                    int emptyNum=0;
                    for(int i=0;i<config_.associativity;i++)
                    {
                        if(block_lastuse[index][i]==-1)
                            emptyNum+=1;
                    }
                    if(emptyNum>3)
                        emptyNum=3;
                    for(int i=1;i<=emptyNum;i++)
                    this->HandleRequest(addr+i*config_.block_size,1,1,temp,true);
            }    
        }
    }
    int lower_hit = -1, lower_time = 0;
    if (read == 1){
        if (hit == 1)
            HitCache(index, position);
        else{
            char *lower_content = new char[config_.block_size];
            uint64_t lower_addr = addr - offset;
            if (prefetch == FALSE)
                stats_.fetch_num ++;
            lower_->HandleRequest(lower_addr, config_.block_size, 1,
                lower_content, prefetch);
            position = ReplacePlace(index, tag, lower_content, prefetch);
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
                    content, prefetch);
            }
        }
        else{
            if (config_.write_allocate == 1){
                char *lower_content = new char[config_.block_size];
                uint64_t lower_addr = addr - offset;
                if (prefetch == FALSE)
                    stats_.fetch_num ++;
                lower_->HandleRequest(lower_addr, config_.block_size, 1,
                    lower_content, prefetch);
                position = ReplacePlace(index, tag, lower_content, prefetch);
                WriteCache(index, position, offset, bytes, content);
                if (config_.write_through == 1){
                    int lower_hit, lower_time;
                    lower_->HandleRequest(addr, bytes, 0, 
                        content, prefetch);
                }
                delete lower_content;
            }
            else{
                lower_->HandleRequest(addr, bytes, 0, 
                    content, prefetch);
            }
        }
    }
    if(!prefetch&&hit==0)
   { 
        char temp[64];
        this->HandleRequest(addr+config_.block_size,1,1,temp,true);
   }      
}

int Cache::ReplacePlace(uint64_t index, uint64_t tag, char* content, bool prefetch){
    if (prefetch == FALSE)
        stats_.replace_num ++;

    int position = GetReplacePosition(index);
    if(prefetch) // set the tag to 1
    prefetch_tag[index][position]=1;
    else
    prefetch_tag[index][position]=0;

    if (config_.write_through == 0 && cache_addr[index][position].valid == TRUE && cache_addr[index][position].dirty == TRUE){
        int lower_hit, lower_time;
        uint64_t lower_addr = (cache_addr[index][position].tag << (config_.block_bit + config_.index_bit))
            + (index << (config_.block_bit)); 
        lower_->HandleRequest(lower_addr, config_.block_size, 0,
                cache_addr[index][position].content, prefetch);
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

