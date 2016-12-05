#include "stdio.h"
#include "cache.h"
#include "memory.h"
#include "param.h"
#include <fstream>
#include <cstdlib>
//#include <iostream>
using namespace std;

ifstream fin;
ofstream fout;

int main(int argc, char **argv) {
  int size, set, way, write_through, write_allocate;
  //cout << "ther" << endl;
  //printf("main1\n");
  if(argc < 6)
  {
    printf("God! Not good enough!\n");
    return 1;
  }
  //printf("main2\n");
  size = atoi(argv[1]);
  set = atoi(argv[2]);
  way = atoi(argv[3]);
  write_through = atoi(argv[4]);
  write_allocate = atoi(argv[5]);
  //printf("main3\n");
  Memory* m = new Memory(NULL, 0);
  Cache* l1 = new Cache(size, set, way, write_through, write_allocate, m);
  l1->SetLower(m);
  //printf("main4\n");

  StorageStats s;
  s.access_time = 0;
  s.miss_num = 0;
  s.access_counter = 0;
  s.replace_num = 0;
  s.fetch_num = 0;
  s.prefetch_num = 0;
  m->SetStats(s);
  l1->SetStats(s);
  //printf("main5\n");

  StorageLatency ml;
  ml.bus_latency = 6;
  ml.hit_latency = 100;
  m->SetLatency(ml);

  StorageLatency ll;
  ll.bus_latency = 3;
  ll.hit_latency = 10;
  l1->SetLatency(ll);
  //printf("main6\n");

  int hit = 0, time = 0;
  char content[64];

  fin.open("./trace/1.trace", ios::in);
  char q;
  lint address;
  int requestNum = 0;
  //printf("main7\n");
  while(fin >> q)
  {
    fin >> address;
    //printf("q: %c address: %llx\n", q, address);
    if(q == 'r')
    {
      l1->HandleRequest(address, 1, 1, content, hit, time);
    }
    else if(q == 'w')
    {
      l1->HandleRequest(address, 1, 0, content, hit, time);
    }
    else
    {
      printf("God! Undefined request!\n");
      return 2;
    }
    requestNum ++;
  }
  fin.close();
  //printf("main8\n");
  //printf("%d\t%d\t%d\n", hit, requestNum, time);
  printf("cache:\n");
  l1->printStat();
  printf("\nmemory:\n");
  m->printStat();

  //l1.HandleRequest(0, 0, 1, content, hit, time);
  //printf("Request access time: %dns\n", time);
  //l1.HandleRequest(1024, 0, 1, content, hit, time);
  //printf("Request access time: %dns\n", time);

  //l1.GetStats(s);
  //printf("Total L1 access time: %dns\n", s.access_time);
  //m.GetStats(s);
  //printf("Total Memory access time: %dns\n", s.access_time);
  return 0;
}
