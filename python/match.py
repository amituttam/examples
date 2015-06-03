
def match(pattern, text):

    for word in text.split(' '):
        score = 0
        if len(pattern) > len(word):
            continue

        for i in range(0, len(pattern)):
            if score == len(pattern)-1:
                return True

            if pattern[i] == word[i]:
                score += 1
            else:
                break

    return False

def match(pattern, text):
    if len(pattern) > len(text):
        return False


print(match("test", "this is a test"))
print(match("test", "this is a Test"))
print(match("yoyo", "this is a test"))
print(match("", ""))
print(match("", "this is"))
print(match("h", "this is"))
