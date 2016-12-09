#include "cache.h"
#include <time.h>
#include <cstdlib>
#define LRU
//#define  RAND
//#define FIFO
//#define AGING
#define AIP 0
int Cache::GetReplacePosition(int index){
    int position=0;

    if (AIP || useBypass){
        position = -1;
        for (int i = 0; i < config_.associativity; ++ i)
            cache_info[index][i].C ++;
        for (int i = 0; i < config_.associativity; ++ i)
            if(cache_addr[index][i].valid == false){
                position = i;
                break;
        }
        if (position == -1){
            int C0 = -1;
            for (int i = 0; i < config_.associativity; ++ i){
                if (cache_info[index][i].conf == true &&
                    cache_info[index][i].C > cache_info[index][i].maxC &&
                    cache_info[index][i].C > cache_info[index][i].maxCp && 
                    cache_info[index][i].totalC > C0){
                    C0 = cache_info[index][i].totalC;
                    position = i;
                    //printf("used\n");
                }
            }
        }
        if (position == -1){
            int C0 = -1;
            for (int i = 0; i < config_.associativity; ++ i)
                if (cache_info[index][i].totalC > C0){
                    C0 = cache_info[index][i].totalC;
                    position = i;
                }
        }
    }

    #ifdef LRU
    position=-1;
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_lastuse[index][i]==-1)
            {
                position=i;
                break;
            } 
    }
    if(position==-1)// can not find a empty place
    {
        int maxval=-1;
        for(int i=0;i<config_.associativity;i++)
        {
            if(block_lastuse[index][i]>maxval)
            {
                maxval=block_lastuse[index][i];
                position=i;
            }
        }
    }// find a positon
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_lastuse[index][i]!=-1)
        block_lastuse[index][i]+=1;
    }
    block_lastuse[index][position]=0;
    #endif
    #ifdef RAND
    position = rand()%(config_.associativity);
    #endif
    #ifdef FIFO
    position=-1;
    for(int i=0 ;i<config_.associativity;i++)
    {
        if(block_enter[index][i]==-1)
        {
                position=i;
                break;
        }
    }
    if(position==-1)
    {
        int maxval=-1;
        for(int i=0;i<config_.associativity;i++)
        {
            if(block_enter[index][i]>maxval)
            {
                maxval=block_enter[index][i];
                position=i;
            }
        }
    }
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_enter[index][i]!=-1)
            block_enter[index][i]+=1;
    }
    block_enter[index][position]=0;
    #endif
    #ifdef AGING
    position=-1;
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_enter[index][i]==-1)
        {
            position=i;
            block_enter[index][i]=0;
            break;
        }
    }
    if(position==-1)
    {
        int leastval=(~(1<<31));
        for(int i=0;i<config_.associativity;i++)
        {
            if(block_enter[index][i]<=leastval)
            {
                leastval = block_enter[index][i];
                position=i;
            }
        }
    }
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_enter[index][i]!=-1)
            block_enter[index][i]=(block_enter[index][i]>>1);
    }
    block_enter[index][position]=block_enter[index][position]|(1<<30);
    #endif
    if(position==-1)
        printf("wrong!\n");
    return position;
}

void Cache::HitCache(uint64_t index, uint64_t position){
    #ifdef LRU   
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_lastuse[index][i]!=-1)
            block_lastuse[index][i]+=1;
    }
    block_lastuse[index][position]=0;
    #endif
    #ifdef FIFO
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_enter[index][i]!=-1)
            block_enter[index][i]+=1;
    }
    #endif
    #ifdef AGING
    for(int i=0;i<config_.associativity;i++)
    {
        if(block_enter[index][i]!=-1)
            block_enter[index][i]=block_enter[index][i]>>1;
    }
    block_enter[index][position]=block_enter[index][position]|(1<<30);
    #endif

    if (AIP || useBypass){
        for (int i = 0; i < config_.associativity; ++ i)
            cache_info[index][i].C ++;
        if (cache_info[index][position].C > cache_info[index][position].maxC)
            cache_info[index][position].maxC = cache_info[index][position].C;
        cache_info[index][position].totalC += cache_info[index][position].C;
        cache_info[index][position].C = 0;
    }
}


void Cache::updateInfo(int index, uint64_t tag, int position, uint64_t newTag){
    //printf("index: %llx tag: %llx position: %llx newTag: %llx\n", index, tag, position, newTag);
    if (AIP || useBypass){
        if (tag != -1){
            uint64_t addr = (tag << config_.index_bit) + index;
            uint64_t tag1 = addr & ((1 << 16) - 1);
            //printf("addr: %llx tag1: %llx\n",addr, tag1);
            if (info[tag1].valid == true && info[tag1].addr == addr){
                if (info[tag1].maxC == cache_info[index][position].maxC)
                    info[tag1].conf = true;
                if (info[tag1].maxC < cache_info[index][position].maxC)
                    info[tag1].maxC = cache_info[index][position].maxC;
            }
            else{
                //if (info[tag1].valid == true)
                //    printf("replace\n");
                info[tag1].valid = true;
                info[tag1].maxC = cache_info[index][position].maxC;
                info[tag1].addr = addr;
                info[tag1].conf = false;
            }

        }

        uint64_t newAddr = (newTag << config_.index_bit) + index;
        uint64_t newTag1 = newAddr & ((1 << 16) - 1);
        if (info[newTag1].valid == true && info[newTag1].addr == newAddr){
            cache_info[index][position].maxCp = info[newTag1].maxC;
            cache_info[index][position].conf = info[newTag1].conf;
        }
        else{
            cache_info[index][position].maxCp = 1000000000;
            cache_info[index][position].conf = false;
        }
        cache_info[index][position].C = 0;
        cache_info[index][position].maxC = 0;
        cache_info[index][position].totalC = 0;
    }
}