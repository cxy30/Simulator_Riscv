#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include "cache.h"
#include "memory.h"
#include "param.h"

using namespace std;

Memory *memory;
Cache *l3, *l2, *l1;
ifstream fin;
ofstream fout;
int hitLatency[18][12] = {};
//int latency1[9][9][9] = {};
//int latency2[9][9][9] = {};
int latency1[18][12] = {};
int latency2[18][12] = {};
int associality[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 0};
//int size[] = {2*(1<<20), 16*(1<<20), 128*(1<<20)};
int size[18] = {};
int block_size = 64;

int initializeSize()
{
	int start = 1*(1<<10);
	for(int i = 0; i < 18; i++)
	{
		size[i] = start;
		start = (start << 1);
	}
}

bool validCache(int size, int associality)
{
	if(associality * block_size > size) return false;
	return true;
}

int calculateLatency(int size, int associality)
{
	if(!validCache(size, associality)) return 0;
	cout << size << ' ' << associality << endl;
	if(block_size * associality == size) associality = 0;
	char ins[128];
	sprintf(ins, "../cacti5.3/cacti %d %d", size, associality);
	system(ins);
	/*
	fin.open("./out.csv", ios::in);
	char a[4096];
	fin.getline(a, 4096);//cout << a << endl;
	fin.getline(a, 4096);//cout << a << endl;
	fin.close();
	string s = a;
	//cout << s << endl;
	for(int i = 0; i < 5; i++)
	{
		int idx = s.find(',');
		s = s.substr(idx+1);
	}
	int idx = s.find(',');
	s = s.substr(1, idx-1);
	double t;
	sscanf(s.c_str(), "%lf", &t);
	int cycles = t / 0.5 + 1;
	//system("rm out.csv");
	*/
	int cycles = 0;
	return cycles;
}

void getLatency()
{
	fin.open("./out.csv", ios::in);
	char a[4096];
	fin.getline(a, 4096);
	for(int i = 0; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])){
				hitLatency[i][j] = -1;
				continue;
			}
			fin.getline(a, 4096);
			string s = a;
			if(s == "") break;
			for(int k = 0; k < 5; k++)
			{
				int idx = s.find(',');
				s = s.substr(idx+1);
			}
			int idx = s.find(',');
			s = s.substr(1, idx-1);
			double t;
			sscanf(s.c_str(), "%lf", &t);
			//if(t > 1e6) t = 0;
			int cycles = t / 0.5 + 1;
			hitLatency[i][j] = cycles;
			if(t > 1e6) hitLatency[i][j] = -1;
		}
	}
	fin.close();
	for(int i = 0; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			cout << hitLatency[i][j] << ' ';
		}
		cout << endl;
	}
}

int HandleTrace(char *filename)
{
	//printf("handletrace\n");
	fin.open(filename, ios::in);
	char c;
	lint add;
	char content[8];
	int hit = 0, time = 0;
	int num = 0;
	while(fin >> c)
	{
		num ++;
		fin >> hex >> add;
		if(c == 'r')
		{
			//printf("%c %lld\n", c, add);
			l1->HandleRequest(add, 1, 1, content, false);
		}
		else
		{
			//printf("%c %lld\n", c, add);
			l1->HandleRequest(add, 1, 0, content, false);
		}
	}
	fin.close();
	StorageStats stats;
	l1->GetStats(stats);
	time += stats.access_time;
	//int miss = 0;
	if(l2 != NULL) 
	{
		l2->GetStats(stats);
		time += stats.access_time;
		//miss += stats.miss_num;
	}
	if(l3 != NULL)
	{
		l3->GetStats(stats);
		time += stats.access_time;
		//miss += stats.miss_num;
	}
	
	memory->GetStats(stats);
	time += stats.access_time;

	printf("num: %d time: %d\n", num, time);//, miss);
	return time;
}

void PrintLatency()
{
	fout.open("res.csv", ios::out);
	printf("trace 1\n");
	for(int i = 0; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			fout << latency1[i][j] << ',';
		}
		fout << endl;
	}
	printf("trace 2\n");
	for(int i = 0; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			fout << latency2[i][j] << ',';
		}
		fout << endl;
	}
	fout.close();
}

int Test(int cacheNum, int *sizes, int *associalities, int *latencies, char *name)
{
	if(cacheNum == 3)
	{
		//printf("%d %d %d\n", cacheNum, sizes[0], associalities[0]);
		for(int i = 0; i < 3; i++)
			if(associalities[i] == 0)
				associalities[i] = sizes[i] / block_size;
		l3 = new Cache(sizes[2], sizes[2] / associalities[2] / block_size, associalities[2], 0, 1, memory);
		l2 = new Cache(sizes[1], sizes[1] / associalities[1] / block_size, associalities[1], 0, 1, l3);
		l1 = new Cache(sizes[0], sizes[0] / associalities[0] / block_size, associalities[0], 0, 1, l2);
	}
	else if(cacheNum == 2)
	{
		for(int i = 0; i < 2; i++)
			if(associalities[i] == 0)
				associalities[i] = sizes[i] / block_size;
		l2 = new Cache(sizes[1], sizes[1] / associalities[1] / block_size, associalities[1], 0, 1, memory);
		l1 = new Cache(sizes[0], sizes[0] / associalities[0] / block_size, associalities[0], 0, 1, l2);
	}
	else
	{
		for(int i = 0; i < 1; i++)
			if(associalities[i] == 0)
				associalities[i] = sizes[i] / block_size;
		l1 = new Cache(sizes[0], sizes[0] / associalities[0] / block_size, associalities[0], 0, 1, memory);
	}
	
	StorageStats s;
	s.access_time = 0;
	s.miss_num = 0;
	s.access_counter = 0;
	s.replace_num = 0;
	s.fetch_num = 0;
	s.prefetch_num = 0;
	memory->SetStats(s);
	if(cacheNum > 2) l3->SetStats(s);
	if(cacheNum > 1) l2->SetStats(s);
	l1->SetStats(s);
	StorageLatency ml;
	ml.bus_latency = 6;
	ml.hit_latency = 100;
	memory->SetLatency(ml);
	if(cacheNum > 2)
	{
		StorageLatency l3l;
		l3l.bus_latency = 6;
		l3l.hit_latency = latencies[2];
		l3->SetLatency(l3l);
	}
	if(cacheNum > 1)
	{
		StorageLatency l2l;
		l2l.bus_latency = 3;
		l2l.hit_latency = latencies[1];
		l2->SetLatency(l2l);
	}
	StorageLatency l1l;
	l1l.bus_latency = 0;
	l1l.hit_latency = latencies[0];
	l1->SetLatency(l1l);
	int t = HandleTrace(name);
	if(l3 != NULL) delete l3;
	if(l2 != NULL) delete l2;
	delete l1;
	l3 = l2 = l1 = NULL;
	return t;
}

void TestL1()
{
	printf("trace 1\n");
	for(int i = 0; i < 11; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])) continue;			
			if(hitLatency[i][j] == -1) continue;
			printf("%d %d\n", i, j);
			int sizes[3] = {size[i], size[9], size[12]};
			int associalities[3] = {associality[j], associality[3], associality[3]};
			int latencies[3] = {hitLatency[i][j], hitLatency[9][3], hitLatency[12][3]};
			int t = Test(1, sizes, associalities, latencies, "./trace/1.trace");
			latency1[i][j] = t;
		}
	}
	printf("trace 2\n");
	for(int i = 0; i < 11; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])) continue;			
			if(hitLatency[i][j] == -1) continue;
			printf("%d %d\n", i, j);
			int sizes[3] = {size[i], size[9], size[12]};
			int associalities[3] = {associality[j], associality[3], associality[3]};
			int latencies[3] = {hitLatency[i][j], hitLatency[9][3], hitLatency[12][3]};
			int t = Test(1, sizes, associalities, latencies, "./trace/2.trace");
			latency2[i][j] = t;
		}
	}
}

void TestL2()
{
	printf("trace 1\n");
	for(int i = 2; i < 15; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])) continue;			
			if(hitLatency[i][j] == -1) continue;
			printf("%d %d\n", i, j);
			int sizes[3] = {size[2], size[i], size[12]};
			int associalities[3] = {associality[0], associality[j], associality[3]};
			int latencies[3] = {hitLatency[2][0], hitLatency[i][j], hitLatency[12][3]};
			int t = Test(2, sizes, associalities, latencies, "./trace/1.trace");
			latency1[i][j] = t;
		}
	}
	printf("trace 2\n");
	for(int i = 2; i < 15; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])) continue;			
			if(hitLatency[i][j] == -1) continue;
			printf("%d %d\n", i, j);
			int sizes[3] = {size[2], size[i], size[12]};
			int associalities[3] = {associality[0], associality[j], associality[3]};
			int latencies[3] = {hitLatency[2][0], hitLatency[i][j], hitLatency[12][3]};
			int t = Test(2, sizes, associalities, latencies, "./trace/2.trace");
			latency2[i][j] = t;
		}
	}
}

void TestL3()
{
	printf("trace 1\n");
	for(int i = 7; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])) continue;			
			if(hitLatency[i][j] == -1) continue;
			printf("%d %d\n", i, j);
			int sizes[3] = {size[2], size[7], size[i]};
			int associalities[3] = {associality[0], associality[0], associality[j]};
			int latencies[3] = {hitLatency[2][0], hitLatency[7][0], hitLatency[i][j]};
			int t = Test(3, sizes, associalities, latencies, "./trace/1.trace");
			latency1[i][j] = t;
		}
	}
	printf("trace 2\n");
	for(int i = 7; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			if(!validCache(size[i], associality[j])) continue;			
			if(hitLatency[i][j] == -1) continue;
			printf("%d %d\n", i, j);
			int sizes[3] = {size[2], size[7], size[i]};
			int associalities[3] = {associality[0], associality[0], associality[j]};
			int latencies[3] = {hitLatency[2][0], hitLatency[7][0], hitLatency[i][j]};
			int t = Test(3, sizes, associalities, latencies, "./trace/2.trace");
			latency2[i][j] = t;
		}
	}
}

int main()
{
	
	memory = new Memory(NULL, false);
	
	initializeSize();
	/*
	for(int i = 0; i < 18; i++)
	{
		for(int j = 0; j < 12; j++)
		{
			calculateLatency(size[i], associality[j]);
		}
	}
	*/
	
	getLatency();
	TestL1();
	//int sizes[3] = {size[2], size[7], size[12]};
	//int associalities[3] = {associality[0], associality[0], associality[3]};
	//int latencies[3] = {hitLatency[2][0], hitLatency[7][0], hitLatency[12][3]};
	//int t = Test(3, sizes, associalities, latencies, "./trace/1.trace");
	
	PrintLatency();
	
	return 0;
}