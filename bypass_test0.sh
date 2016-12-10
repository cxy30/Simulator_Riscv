j=5
for ((i=90;i<=100;i+=2))
do
	for ((k=80;k<=100;k+=2))
	do
		./bypass_test trace/6.trace $i $k $j >> ./result/trace_test06.txt
	done
done
#./bypass_test trace/1.trace 50 50 >> ./result/trace_test.txt

