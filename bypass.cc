#include "cache.h"

bool Cache::do_bypass(uint64_t addr, int index){
	for (int i = 0; i < config_.associativity; ++ i)
		if (cache_addr[index][i].valid == false)
			return false;
	uint64_t tag = addr & ((1 << 16) - 1);
	if (!(info[tag].valid == true && info[tag].addr == addr && info[tag].maxC == 0 && info[tag].conf == 1))
		return false;
	info[tag].conf = 0;
	printf("used\n");
	return true;
}