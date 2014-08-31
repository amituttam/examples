
class HashTable:
    def __init__(self):
        self.size = 11
        self.slots = [None]*self.size
        self.data = [None]*self.size

    def hashfunction(self, key, size):
        return key%size

    def rehash(self, oldhash, size):
        return (oldhash+1)%size

    def put(self, key, data):
        hashvalue = self.hashfunction(key, len(self.slots))

        # Hit in first
        if self.slots[hashvalue] == None:
            self.slots[hashvalue] = key
            self.data[hashvalue] = data
        else:
            # Replace data
            if self.slots[hashvalue] == key:
                self.data[hashvalue] = data
            # Collision
            else:
                nextslot = self.rehash(hashvalue, len(self.slots))
                while self.slots[nextslots] != None and self.slots[nextslots] != key:
                    nextslot = self.rehash(nextslot, len(self.slots))

                if self.slots[nextslot] == None:
                    self.slots[nextslot] = key
                    self.data[nextslot] = data
                else:
                    self.data[nextslot] = data

    def get(self, key):
        startslot = self.hashfunction(key,len(self.slots))

        data = None
        stop = False
        found = False
        position = startslot
        while self.slots[position] != None and not found and not stop:
           if self.slots[position] == key:
             found = True
             data = self.data[position]
           else:
             position=self.rehash(position,len(self.slots))
             if position == startslot:
                 stop = True
        return data

    def __getitem__(self,key):
        return self.get(key)

    def __setitem__(self,key,data):
        self.put(key,data)
