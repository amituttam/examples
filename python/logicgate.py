#!/usr/bin/env python

class LogicGate:

	def __init__(self, n):
		self.label = n
		self.output = None

	def getLabel(self):
		return self.label

	def getOutput(self):
		self.output = self.performGateLogic()
		return self.output

class BinaryGate(LogicGate):

	def __init__(self, n):
		LogicGate.__init__(self, n)

		self.pinA = None
		self.pinB = None

	def getPinA(self):
		if self.pinA == None:
			return int(input("Enter Pin A input for gate " + self.getLabel() + "-->"))
		else:
			return self.pinA.getFrom().getOutput()

	def getPinB(self):
		if self.pinB == None:
			return int(input("Enter Pin B input for gate " + self.getLabel() + "-->"))
		else:
			return self.pinB.getFrom().getOutput()

	def setNextPin(self, source):
		if self.pinA == None:
			self.pinA = source
		else:
			if self.pinB == None:
				self.pinB = source
			else:
				raise RuntimeError("Error: NO EMPTY PINS")

class UnaryGate(LogicGate):

	def __init__(self, n):
		LogicGate.__init__(self, n)

		self.pin = None

	def getPin(self):
		if self.pin == None:
			return int(input("Enter Pin input for gate " + self.getLabel() + "-->"))
		else:
			return self.pin.getFrom().getOutput()

	def setNextPin(self, source):
		if self.pin == None:
			self.pin = source
		else:
			raise RuntimeError("Error: NO EMPTY PINS")

class AndGate(BinaryGate):

	def __init__(self, n):
		BinaryGate.__init__(self, n)

	def performGateLogic(self):
		a = self.getPinA()
		b = self.getPinB()

		if a == 1 and b == 1:
			return 1
		else:
			return 0

class OrGate(BinaryGate):

	def __init__(self, n):
		BinaryGate.__init__(self, n)

	def performGateLogic(self):
		a = self.getPinA()
		b = self.getPinB()

		if a == 1 or b == 1:
			return 1
		else:
			return 0

class NotGate(UnaryGate):

	def __init__(self, n):
		UnaryGate.__init__(self, n)

	def performGateLogic(self):
		a = self.getPin()

		if a == 1:
			return 0
		else:
			return 1

class NandGate(AndGate):

	def performGateLogic(self):
		if super().performGateLogic() == 1:
			return 0
		else:
			return 1

class NorGate(OrGate):

	def performGateLogic(self):
		if super().performGateLogic() == 1:
			return 0
		else:
			return 1

class XorGate(BinaryGate):

	def __init__(self,n):
		BinaryGate.__init__(self,n)

	def performGateLogic(self):
		if self.getPinA() != self.getPinB():
			return 1
		else:
			return 0

class HalfAdder:

	def __init__(self,n):
		self.g1 = XorGate(n)
		self.g2 = AndGate(n)

	def getOutput(self):
		s = self.g1.getOutput()
		c = self.g2.getOutput()

		return 2*c+s

class FullAdder:

	def __init__(self,n):
		self.h1 = HalfAdder(n)
		self.h2 = HalfAdder(n)

class Connector:

	def __init__(self, fgate, tgate):
		self.fromgate = fgate
		self.togate = tgate

		tgate.setNextPin(self)

	def getFrom(self):
		return self.fromgate

	def getTo(self):
		return self.togate

def main():

	ha1 = HalfAdder("HA1")
	print(ha1.getOutput())

	x1 = XorGate("X1")
	print(x1.getOutput())

	g1 = AndGate("G1")
	g2 = AndGate("G2")
	g3 = OrGate("G3")
	g4 = NotGate("G4")
	c1 = Connector(g1,g3)
	c2 = Connector(g2,g3)
	c3 = Connector(g3,g4)
	print(g4.getOutput())

	h1 = NandGate("H1")
	h2 = NandGate("H2")
	h3 = AndGate("H3")
	d1 = Connector(h1, h3)
	d2 = Connector(h2, h3)
	print(h3.getOutput())

if __name__ == '__main__':
	main()
