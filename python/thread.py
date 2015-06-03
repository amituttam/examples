#!/usr/bin/python

from threading import Thread

def count(n):
	while n > 0:
		n -= 1

#count(100000000)
#count(100000000)

t1 = Thread(target=count, args=(100000000,))
t1.start()
t2 = Thread(target=count, args=(100000000,))
t2.start()
t1.join()
t2.join()
