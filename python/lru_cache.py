# Simple LRU Cache implementation

class LRUCache:
    def __init__(self, capacity):
        # Capacity of cache
        self.capacity = capacity

        # Used to track access history
        self.tm = 0

        # Cache to store key,value pair
        self.cache = {}

        # Store key,tm pair
        self.lru = {}

    def get(self, key):
        if key in self.cache:
            self.lru[key] = self.tm
            self.tm += 1
            return self.cache[key]
        return -1

    def set(self, key, value):
        if len(self.cache) >= self.capacity:
            # find LRU entry
            old_key = min(self.lru.keys(), key=lambda k: self.lru[k])
            self.cache.pop(old_key)
            self.lru.pop(old_key)
        
        self.cache[key] = value
        self.lru[key] = self.tm
        self.tm += 1
