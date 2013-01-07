#!/usr/bin/env python
import random
import itertools
import time

class Timer(object):
	def __init__(self, verbose=False):
		self.verbose = verbose

	def __enter__(self):
		self.start = time.time()
		return self

	def __exit__(self, *args):
		self.end = time.time()
		self.secs = self.end - self.start
		self.msecs = self.secs * 1000  # millisecs
		if self.verbose:
			print 'elapsed time: %f ms' % self.msecs

#{{{def b_search_starts(key,S,lo,hi):
# i is the interval we are searching for within 
# the intervals set S between S[lo] and S[hi]
def b_search_starts(key,S,lo,hi):
	mid = -1
	while (hi - lo > 1):
		mid = (hi + lo) / 2;
		#print mid
		# test if the start of i is after then end
		# of S[mid]
		if S[mid][0] < key:
			lo = mid
		else:
			hi = mid
	return hi
#}}}

#{{{def b_search_ends(key,S,lo,hi):
def b_search_ends(key,S,lo,hi):
	mid = -1
	while (hi - lo > 1):
		mid = (hi + lo) / 2;
		# test if the start of i is after then end
		# of S[mid]
		if S[mid][1] < key:
			lo = mid
		else:
			hi = mid
	return hi
#}}}

#{{{def rand_flat_sets(num_sets,
def rand_flat_sets(num_sets,
				   num_intervals,\
				   max_interval_size,\
				   max_gap_size,\
				   seed):
	random.seed(seed)

	# generate some interval sets
	A=[]
	B=[]
	C=[]
	A_last = 0
	B_last = 0
	C_last = 0

	S = []
	for j in range(0,num_sets):
		S.append([])

	j_lasts = []
	for i in range(0,num_intervals):
		j_lasts.append(0)

	for i in range(0,num_intervals):
		for j in range(0,num_sets):
			j_next = j_lasts[j] + random.randint(1, max_gap_size)	
			j_lasts[j] = j_next + random.randint(1, max_interval_size)	
			S[j].append( (j_next,j_lasts[j]) )

	return S
#}}}

#{{{ def nway_split(S):
def nway_split(S):
	num_S = len(S)
	s_range = (0,num_S - 1)	
	Q = []
	q_0 = []
	for i in range(s_range[0],s_range[1]+1):
		q_0.append((0,len(S[i])-1))
	Q.append(q_0)

	return _nway_split(S,s_range,Q,0)

def _nway_split(S,s_range,Q,depth):
	num_S = s_range[1]-s_range[0]+1
	T = {}

	for q in Q:
		a = S[s_range[0]]
		a_dim = q[0]
		mid = (a_dim[1]+1 + a_dim[0]-1)/2
		root = a[mid]
		new_q = ()
		r = []

		q_left = [ (a_dim[0],mid-1) ]
		q_right = [ (mid+1,a_dim[1]) ]

		for i in range(1,num_S):
			s = S[s_range[0]+i]
			s_dim = q[i]

			# get the position of last interval in s to end before root starts
			# b_search gives either the match index, or the # insert position,
			# in either case the correct inveral is 1 less
			# than the insert position
			# Exampe:
			#                   0      1          2
			#                   -----   ------     -------
			#                 ^  ^  ^ ^   ^  ^  ^     ^  ^  ^
			# b_search_ends:  0  0  0 1   1  1  2     2  2  3
			# s_left_i:      -1 -1 -1 0   0  0  1     1  1  2
			s_left_i = b_search_ends(root[0],s,s_dim[0]-1,s_dim[1]+1) - 1

			#print "s_left_i:",s_left_i

			# get position of the first interval in s to start after root ends
			# b_search gives either the match, or if there is not match, the
			# insert position, if there is a match then move s_right_ up by one
			# Exampe:
			#                   0      1          2
			#                   -----   ------     -------
			#                 ^ ^  ^    ^  ^   ^   ^  ^     ^
			# b_search_ends:  0 0  1    1  2   2   2  3     3
			# s_left_i:       0 1  1    2  2   2   3  3     3

			s_right_i = b_search_starts(root[1],s,s_dim[0]-1,s_dim[1]+1)
			# make sure that s_right_i is in the valid range
			if s_right_i <= s_dim[1] and \
				root[1] == s[s_right_i][0]:
				s_right_i+=1

			# the current interval intersects anything in inclusive range 
			# [s_left_i+1,s_right_i-1]
			# if s_right_i-1 < s_left_i+1 then no intersection occured
			s_center = (s_left_i+1,s_right_i-1)

			#print "s_center:",s_center,s[s_center[0]:s_center[1]+1]

			# the intervals to the left and right of s_center will be searched next
			# in the simple case where
			# s = [s_dim[0],s_dim[1]] and s_center = [s_left_i+1,s_right_i-1]
			# then  
			# s_left is [s_dim[0],s_center[0]-1] and 
			# s_right is [s_center[1]+1,s_dim[1]]
			# it is possible that 
			#  - a[mid-1] intersects s[s_center[0]] and 
			#  - a[mid+1] intersects a[s_center[1]], 
			# 
			# Example:
			#      mid-1  mid    mid+1
			# A:   -----  ----   ------
			# B: ---  -------------  ----
			# 	 ^end_i            ^start_i
			# 
			s_left = (s_dim[0],s_center[0]-1) # simple case
			# check if a[mid-1] intersects s[s_center[0]]
			# also do some bound checking (mid-1 might not exist), s_left_i
			if mid > a_dim[0] and \
					s_center[0] >= s_dim[0] and \
					s_center[0] <= s_dim[1] and \
					a[mid - 1][1] >= s[s_center[0]][0]:
				s_left = (s_dim[0],s_center[0]) # a[mid-1] intersects s[s_center[0]]

			s_right = (s_center[1]+1,s_dim[1]) # simple case
			if mid < a_dim[1] and \
					s_center[1] >= s_dim[0] and \
					s_center[1] <= s_dim[1] and \
					a[mid + 1][0] <= s[s_center[1]][1]:
				s_right = (s_center[1],s_dim[1]) # a[mid+1] intersects s[s_right_i-1]

			#print "l:",s_left,"c:",s_center,"r:",s_right
			q_left.append(s_left)
			q_right.append(s_right)
			r.append(s_center)
		T[mid]=r

		if False not in [l[0]<=l[1] for l in q_left]:
			Q.append(q_left)

		if False not in [l[0]<=l[1] for l in q_right]:
			Q.append(q_right)
		

	#print '\td:',depth,'T:',T
	# t is the index of an interval in S[s_range[0]]
	# T[t] is a list of ranges where T[t][0] is a range in S[s_range[0] + 1]
	# if T[t][0][0] > T[t][0][1] (the start of the range is larger than
	# end of the range), then the range is is considered empty
	R = []

	for t in T.keys():
		# skip anthing that has an empty range
		if False in [p[0]<=p[1] for p in T[t]]:
			continue
		else:
			if len(T[t]) > 1:
				_s_range = (s_range[0]+1,s_range[1])
				_Q = [T[t]]
				u = _nway_split(S,_s_range,_Q,depth+1)
				if len(u) > 0:
					#print 't:',t,'u:',u
					for w in u:
						v=[]
						v.append([S[s_range[0]][t]])
						for x in w:
							v.append(x)
						R.append(v)
#					for w in u:
#						print w
#						v = []	
#						v.append([S[s_range[0]][t]])
#						v.append(w)
#						R.append(v)
			else: # final pair-wise intersection was found
				v = []
				v.append([S[s_range[0]][t]])
				v.append(S[s_range[1]][ T[t][0][0]:T[t][0][1]+1 ])
				#print 't:',t,'T[t]:',T[t],'v:',v
				R.append(v)	
	return R
#}}}

#{{{def nway_sweep(S):
def _nway_sweep(S,num_S,curr_s,end_s):

	T=[]

	in_context = []
	for i in range(0,num_S):
		in_context.append([])

	head = 0
	at_end = False
	drained_lists = False
	not_ended = range(0,num_S)

	while len(not_ended) > 0:

		# select the min interval among the sets that have not ended
		head = not_ended[0]
		for i in not_ended:
			# some of the pointers may be off the end, don't consider them
			if	curr_s[head] <= end_s[head] and \
				curr_s[i] <= end_s[i] and \
					S[i][curr_s[i]] <= S[head][curr_s[head]]:
				head = i

		# see if we need to get rid of anything in context
		for s in in_context:
			#if the end of an interval in context is less than the start of the
			#next interval, then it is no longer in context
			if len(s) > 0 and \
					s[0][1] < S[head][curr_s[head]][0]:
				s.pop(0)
		in_context[head].append(S[head][curr_s[head]])

		# test to see if there is an nway intersection
		if 0 not in [len(i) for i in in_context]:
			for r in itertools.product(*in_context):
				T.append(r)

		curr_s[head]+=1

		# see if the current list ended
		if (curr_s[head] > end_s[head]):
			not_ended.remove(head)

	return T
#}}}

#{{{def nway_sweep(S):
def nway_sweep(S):
	num_S = len(S)

	curr_s = []
	end_s = []
	for i in range(0,num_S):
		curr_s.append(0)
		end_s.append(len(S[i])-1)

	return _nway_sweep(S,num_S,curr_s,end_s)
#}}}

def main():
	num_intervals = 10000
	max_interval_size = 100
	max_gap_size = 100
	seed = 1

	for num_sets in range(10,50,10):
		S = rand_flat_sets(num_sets, \
						   num_intervals, \
						   max_interval_size, \
						   max_gap_size, \
						   seed)

		#for s in S:
			#print s
		print

		with Timer() as t_sweep:
			R1 = nway_sweep(S)

		with Timer() as t_split:
			R2= nway_split(S)

		print num_sets,t_sweep.secs,t_split.secs,len(R1),len(R2)

if __name__ == "__main__":
	main()
