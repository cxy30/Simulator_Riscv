way=8

for ((i=(1<<15);i<=(1<<25);i*=2))
do
	for ((j=8;j<=1024;j*=2))
	do
		((seti=$i/$j/$way))
		./lab3 $i $seti $way 0 1 >> ./result/block_size.txt
	done
done

block=64

for ((i=(1<<15);i<=(1<<25);i*=2))
do
	for ((j=1;j<=32;j*=2))
	do
		((seti=$i/$j/$block))
		./lab3 $i $seti $j 0 1 >> ./result/associativity.txt
	done
done

for ((i=(1<<15);i<=(1<<25);i*=2))
do
	((seti=$i/$way/$block))
	./lab3 $i $seti $way 0 1 >> ./result/policy.txt
	./lab3 $i $seti $way 1 0 >> ./result/policy.txt
done
