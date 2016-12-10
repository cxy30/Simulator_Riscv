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
  /*
  int size, set, way, write_through, write_allocate;
  if(argc < 6)
  {
    printf("God! Not good enough!\n");
    return 1;
  }
  size = atoi(argv[1]);
  set = atoi(argv[2]);
  way = atoi(argv[3]);
  write_through = atoi(argv[4]);
  write_allocate = atoi(argv[5]);
  */

  Memory* m = new Memory(NULL, 0);
  int size1,size2,set1,set2,way1,way2,lineSize1,lineSize2;
  size1=(1<<15);
  size2=(1<<18);
  way1 = 8;
  way2 = 8;
  lineSize1=64;
  lineSize2=64;
  set1 = size1/way1/lineSize1;
  set2 = size2/way2/lineSize2;

  Cache* l2 = new Cache(size2, set2, way2, 0,1, m, 2);
  l2->SetLower(m);
  Cache* l1 = new Cache(size1, set1, way1, 0,1, l2, 2);
  l1->SetLower(l2);

  l1->bypassTh = atoi(argv[2]) * 1.0 / 100;
  l2->bypassTh = atoi(argv[3]) * 1.0 / 100;
  l1->bypassMinT = atoi(argv[4]);
  l2->bypassMinT = atoi(argv[4]);
  printf("Th: %d %f %f\n",l2->bypassMinT, l1->bypassTh, l2->bypassTh);

  StorageStats s;
  s.access_time = 0;
  s.miss_num = 0;
  s.access_counter = 0;
  s.replace_num = 0;
  s.fetch_num = 0;
  s.prefetch_num = 0;
  m->SetStats(s);
  l1->SetStats(s);
  l2->SetStats(s);

  //printf("main5\n");

  StorageLatency ml;
  ml.bus_latency = 100;
  ml.hit_latency = 0;
  m->SetLatency(ml);

  StorageLatency ll;
  ll.bus_latency = 0;
  ll.hit_latency = 4;// find it in cacti
  l1->SetLatency(ll);
  //printf("main6\n");

  StorageLatency ll2;
  ll2.bus_latency= 6;
  ll2.hit_latency=5;// find it in cacti
  l2->SetLatency(ll2);
  char content[64];

  fin.open(argv[1], ios::in);
  char q;
  lint address;
  int requestNum = 0;
  while(fin >> q)
  {
    fin >> hex >> address;
    if(q == 'r')
    {
      l1->HandleRequest(address, 1, 1, content, 0);
    }
    else if(q == 'w')
    {
      l1->HandleRequest(address, 1, 0, content, 0);
    }
    else
    {
      printf("God! Undefined request!\n");
      return 2;
    }
    requestNum ++;
  }
  fin.close();

  l1->GetStats(s);
  int totalLatency=s.access_time;
  l2->GetStats(s);
  totalLatency+=s.access_time;
  m->GetStats(s);
  totalLatency+=s.access_time;
  printf("total latency: %d\n\n",totalLatency);

  return 0;
}
