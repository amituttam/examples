
import sys
import math

n = int(sys.argv[1])

prime = [1 if i > 1 else 0 for i in range(0,n)]

for i in range(2,n):
	if prime[i] == True:
		j = i
		while j < n:
			j += i
			if j < n:
				prime[j] = 0
		print i,
