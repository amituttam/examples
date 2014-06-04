#!/usr/bin/env python

from __future__ import division
import random
import pdb
import sys

def gen_sentence(best_sentence = None):
	letters = "abcdefghijklmnopqrstuvwxyz "
	if best_sentence == None:
		sentence = [letters[random.randrange(27)] for i in range(28)]
	else:
		sentence = [best_sentence[i] for i in range(28)]
		i = random.randrange(28)
		sentence[i] = letters[random.randrange(27)]

	return ''.join(sentence)

def compare_string(sentence, goal):
	num_correct = 0
	for i in range(28):
		if sentence[i] == goal[i]:
			num_correct += 1

	return num_correct/28

def main():
	goal = "methinks it is like a weasel"
	sentence = gen_sentence()
	score = compare_string(sentence, goal)
	count = 1
	best_sentence = None
	best_score = 0
	print(score, sentence)

	while score < 1:
		# Save best
		if score > best_score:
			best_score = score
			best_sentence = sentence

		sentence = gen_sentence(best_sentence)
		score = compare_string(sentence, goal)

		# Generated the goal sentence
		if score == 1:
			print("DONE! Sentence = %s Count = %d" % (sentence, count))

		# Progress
		if count % 1000000 == 0:
			print("Best: %s (%.2f)" % (best_sentence, best_score))
		count += 1

	print("DONE with loop, sentence='%s' count=%d score=%f best_score=%f" % (sentence, count, score, best_score))

if __name__ == "__main__":
	main()
