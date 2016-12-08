#include "cache.h"
#include <time.h>
#include <cstdlib>
#define LRU
//#define  RAND
//#define FIFO
//define AGING
int Cache::GetReplacePosition(int index){
    int position=0;
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
}
