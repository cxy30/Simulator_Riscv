i=100
for ((k=80;k<=100;k+=2))
do
	for ((j=0;j<=10;j+=2))
	do
		./bypass_test trace/1.trace $i $k $j >> ./result/trace_test1.txt
	done
done
#./bypass_test trace/1.trace 50 50 >> ./result/trace_test.txt

