
def binarySearch(alist, item):
    if len(alist) == 0:
        return False
    else:
        midpoint = len(alist)//2
        if alist[midpoint] == item:
            return True
        else:
            if alist[midpoint] > item:
                return binarySearch(alist[midpoint+1:], item)
            else:
                return binarySearch(alist[:midpoint-1], item)

testList = [0, 1, 2, 8, 13, 17, 19, 32, 42,]
print(binarySearch(testList, 3))
print(binarySearch(testList, 13))
