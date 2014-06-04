#!/usr/bin/env python

from contextlib import contextmanager
import os

@contextmanager
def working_dir(path):
	curdir = os.getcwd()
	os.chdir(path)
	try:
		yield
	finally:
		os.chdir(curdir)

with working_dir("/tmp"):
	print "I am here"
