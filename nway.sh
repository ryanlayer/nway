#usage:	./nway <num sets> <num intervals> <max interval size> <max gap size>

num_sets="10 50 100 500"
num_intervals="100 10000 100000"
max_interval_size=100
max_gap_size=100

for num_interval in $num_intervals
do
	for num_set in $num_sets
	do
		echo -en "$num_set\t$num_interval\t$max_interval_size\t$max_gap_size\t"
		./nway $num_set $num_interval $max_interval_size $max_gap_size
	done
done
