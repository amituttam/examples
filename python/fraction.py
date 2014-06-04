#!/usr/bin/env python

from __future__ import print_function
import sys

class Fraction:

	def __init__(self, top, bottom):
		if type(top) != int or type(bottom) != int:
			raise RuntimeError("Numerator or Denominator not an integer")

		if bottom < 0:
			top = -top
			bottom = abs(bottom)

		x = gcd(top, bottom)

		self.num = top/x
		self.den = bottom/x

	def __str__(self):
		return "%s/%s" % (self.num, self.den)

	def __repr__(self):
		return "Fraction(%d,%d)" % (self.num, self.den)

	def __add__(self, b):
		num = self.num * b.den + self.den * b.num
		den = self.den*b.den

		return Fraction(num,den)

	def __eq__(self, b):
		firstnum = self.num * b.den
		secndnum = b.num * self.den

		return firstnum == secndnum

	def __mul__(self, b):
		num = self.num * b.num
		den = self.den * b.den

		return Fraction(num, den) 

	def __div__(self, b):
		c = Fraction(b.den, b.num)
		return self.__mul__(c)

	def __truediv__(self, b):
		c = Fraction(b.den, b.num)
		d = self.__mul__(c)
		res = float(self.num)/self.den
		return res

	def __sub__(self, b):
		num = self.num * b.den
		den = self.den * b.num

		return Fraction(num-den, self.den*b.den)

	def __gt__(self,b):
		firstnum = self.num * b.den
		secondnum = self.den * b.num

		return firstnum > secondnum

	def __lt__(self,b):
		firstnum = self.num * b.den
		secondnum = self.den * b.num

		return firstnum < secondnum

def gcd(m,n):
	if m%n == 0:
		return n
	return gcd(n,m%n)

def main():
	f1 = Fraction(2,5)
	f2 = Fraction(3,9)
	f3 = f1 + f2
	f4 = f1*f2
	f5 = f1/f2
	f6 = f1-f2
	f7 = f1.__truediv__(f2)
	f8 = Fraction(2,9)

	print("f1=%s f2=%s" % (f1,f2), sep=",")
	print("f1+f2=%s" % f3)
	print("f1==f2 is %s" % ("True" if (f1==f2) else "False"))
	print("f1*f2=%s" % f4)
	print("f1/f2=%s" % f5)
	print("f1-f2=%s" % f6)
	print("f1>f2 is %s" % ("True" if (f1>f2) else "False"))
	print("f1<f2 is %s" % ("True" if (f1<f2) else "False"))
	print("(truediv) f1/f2=%s" % f7)
	print("repr(f8)=%r" % f8)

if __name__ == '__main__':
	main()
