#!/usr/bin/env python

for i in range(1, 10000):
        s = str(i)
        ispalindrome = True

        for j in range(0, len(s)/2):
            if s[j] != s[(len(s) - 1) - j]:
                ispalindrome = False
                break

        if ispalindrome:
                print i


