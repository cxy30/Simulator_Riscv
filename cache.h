#ifndef CACHE_CACHE_H_
#define CACHE_CACHE_H_

#include <stdint.h>
#include "storage.h"

typedef struct CacheConfig_ {
    int size;
    int associativity;
    int set_num; // Number of cache sets
    int write_through; // 0|1 for back|through
    int write_allocate; // 0|1 for no-alc|alc
    int block_size;
    int block_bit;
    int index_bit;
    int tag_bit;
} CacheConfig;

struct Block{
    bool valid;
    bool dirty;
    uint64_t tag;
    char *content;
};

class Cache: public Storage {
    public:
    Cache(int size, int set, int way, bool write_through, bool write_allocate, Storage *storage);
    ~Cache();

    // Sets & Gets
    void SetConfig(CacheConfig cc);
    void GetConfig(CacheConfig cc);
    void SetLower(Storage *ll) { lower_ = ll; }
    // Main access process
    void HandleRequest(uint64_t addr, int bytes, int read,
                     char *content, bool prefetch);

    private:
    void WriteCache(uint64_t index, uint64_t position, uint64_t offset, int bytes, char* content);
    void HitCache(uint64_t index, uint64_t position);
    int ReplacePlace(uint64_t index, uint64_t tag, char* content, bool prefetch);
    /*
    // Bypassing
    int BypassDecision();
    // Partitioning
    void PartitionAlgorithm();
    // Replacement
    int ReplaceDecision();
    void ReplaceAlgorithm();
    // Prefetching
    int PrefetchDecision();
    void PrefetchAlgorithm();*/



    int GetReplacePosition(int index);


    CacheConfig config_;
    Storage *lower_;
    DISALLOW_COPY_AND_ASSIGN(Cache);
    Block **cache_addr;
    int **block_lastuse;
    int **block_enter;
    int ** prefetch_tag;
};

#endif //CACHE_CACHE_H_ 
