#!/usr/bin/python

import sys
import math

n = int(sys.argv[1])

# Fill up array with True/False
prime = [1 if i > 1 else 0 for i in range(0,n)]

print "math.sqrt(%d) = %f->%d" % (n, math.sqrt(n), int(math.sqrt(n)))

# Loop from 2, 3, 4, ..., sqrt(n)
for i in range(2, int(math.ceil((math.sqrt(n))))):
	# This check is needed since first j == 4, and gets set to False
	if prime[i] == True:
		for j in range(0, n):
			k = i**2 + j*i
			if k < n:
				prime[k] = 0

j = 0
for i in prime:
	if i == True:
		print j,
	j += 1
