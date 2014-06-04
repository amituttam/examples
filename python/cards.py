#!/usr/bin/env python

class Card:

	def __init__(self,color,number,family):
		self.color = color
		self.number = number
		self.family = family

class Deck:

	def __init__(self,name):
		self.name = name
		self.cards = []

	def addCard(self,card):
		self.cards.append(card)
