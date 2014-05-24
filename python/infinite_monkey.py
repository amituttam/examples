#!/usr/bin/env python
# methinks it is like a weasel

from __future__ import division
import random
import pdb

def gen_sentence():
	letters = "abcdefghijklmnopqrstuvwxyz "
	sentence = []
	for i in range(0,27):
		sentence.append(letters[random.randint(0,26)])

	return ''.join(sentence)

def compare_string(sentence, goal):
	num_correct = 0
	for i in range(0,27):
		if sentence[i] == goal[i]:
			num_correct += 1

	return num_correct/27

goal = "methinks it is like a weasel"
count = 0
done = False
best_sentence = ""
best_score = 0

while not done:
	sentence = gen_sentence()
	score = compare_string(sentence, goal)

	# Save best
	if score > best_score:
		best_score = score
		best_sentence = sentence

	# Generated the goal sentence
	if score == 1:
		done = True
		print("DONE! Sentence = %s Count = %d" % (best_sentence, count))

	# Progress
	if count == 100000:
		print("Best: %s (%.2f)" % (best_sentence, best_score))
		count = 0

	count += 1
