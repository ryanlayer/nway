#usage:	./nway <num sets> <num intervals> <max interval size> <max gap size>

num_sets="10 50 100 500"
num_intervals=1000000
max_interval_size=1000
max_gap_size=1000

for num_set in $num_sets
do
	echo -en "$num_set\t$num_intervals\t$max_interval_size\t$max_gap_size\t"
	./nway $num_set $num_intervals $max_interval_size $max_gap_size
done
