#!/usr/bin/env python

import collections

class LRUCache:
    def __init__(self, capacity):
        self.capacity = capacity
        self.cache = collections.OrderedDict()

    def get(self,key):
        try:
            val = self.cache[key]
            self.cache[key] = val
            return val
        except:
            return -1

    def set(self,key,value):
        try:
            self.cache.pop(key)
        except KeyError:
            if len(self.cache) >= self.capacity:
                self.cache.pop(key,last=False)
            self.cache[key] = value
