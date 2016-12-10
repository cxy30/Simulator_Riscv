CC = g++
PROGRAM = riscv

O_FILES =  machine.o register_file.o memory_monitor.o instruction.o syscall.o memory.o cache.o replace_algorithm.o prefetch.o bypass.o

$(PROGRAM) : main.o $(O_FILES)
	$(CC) -o $(PROGRAM) main.o $(O_FILES)

lab3 : lab3-main.o memory.o cache.o replace_algorithm.o prefetch.o bypass.o
	$(CC) -o lab3 lab3-main.o memory.o cache.o replace_algorithm.o bypass.o

bypass_test : bypass_test.o memory.o cache.o replace_algorithm.o prefetch.o bypass.o
	$(CC) -o bypass_test bypass_test.o memory.o cache.o replace_algorithm.o bypass.o

performance : performance.o memory.o cache.o replace_algorithm.o prefetch.o bypass.o
	$(CC) -o performance performance.o memory.o cache.o replace_algorithm.o prefetch.o bypass.o

main.o : main.cc machine.h
	$(CC) -c main.cc

machine.o : machine.cc machine.h alias.h param.h syscall.h
	$(CC) -c machine.cc

register_file.o : register_file.cc register_file.h
	$(CC) -c register_file.cc

memory.o : memory.cc memory.h storage.h
	$(CC) -c memory.cc

cache.o : cache.cc cache.h storage.h
	$(CC) -c cache.cc

memory_monitor.o : memory_monitor.cc memory_monitor.h
	$(CC) -c memory_monitor.cc

instruction.o : instruction.cc instruction.h
	$(CC) -c instruction.cc

syscall.o : syscall.cc syscall.h
	$(CC) -c syscall.cc

lab3-main.o : lab3-main.cc cache.h memory.h param.h
	$(CC) -c lab3-main.cc

bypass_test.o : bypass_test.cc cache.h memory.h param.h
	$(CC) -c bypass_test.cc

replace_algorithm.o: cache.h
	$(CC) -c replace_algorithm.cc

prefetch.o: cache.h
	$(CC) -c prefetch.cc

bypass.o: cache.h
	$(CC) -c bypass.cc

performance.o : performance.cc cache.h memory.h param.h
	$(CC) -c performance.cc

clean : 
	rm $(PROGRAM) main.o $(O_FILES) lab3-main.o lab3 performance.o performance bypass_test bypass_test.o
