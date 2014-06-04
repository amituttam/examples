sqlist = [x*x for x in range(1,11)]
sqlist
# OUT: [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]
sqlist = [x*x for x in range(1,11) if x%2 != 0]
sqlist
# OUT: [1, 9, 25, 49, 81]
[ch for ch in word for word in ['cat', 'dog', 'rabbit']]
# OUT: Traceback (most recent call last):
# OUT:   File "<input>", line 1, in <module>
# OUT: NameError: name 'word' is not defined
[ch for ch in ['cat', 'dog', 'rabbit']]
# OUT: ['cat', 'dog', 'rabbit']
[x for x in [ch for ch in ['cat', 'dog', 'rabbit']]]
# OUT: ['cat', 'dog', 'rabbit']
[''.join([ch for ch in ['cat', 'dog', 'rabbit']])]
# OUT: ['catdograbbit']
[for ch in ''.join([ch for ch in ['cat', 'dog', 'rabbit']])]
# OUT:   File "<input>", line 1
# OUT:     [for ch in ''.join([ch for ch in ['cat', 'dog', 'rabbit']])]
# OUT:        ^
# OUT: SyntaxError: invalid syntax
[ch for ch in ''.join([ch for ch in ['cat', 'dog', 'rabbit']])]
# OUT: ['c', 'a', 't', 'd', 'o', 'g', 'r', 'a', 'b', 'b', 'i', 't']
set([ch for ch in ''.join([ch for ch in ['cat', 'dog', 'rabbit']])])
# OUT: set(['a', 'c', 'b', 'd', 'g', 'i', 'o', 'r', 't'])
set([ch for ch in ''.join(['cat', 'dog', 'rabbit'])])
# OUT: set(['a', 'c', 'b', 'd', 'g', 'i', 'o', 'r', 't'])
[ch for ch in ''.join(['cat', 'dog', 'rabbit'])]
# OUT: ['c', 'a', 't', 'd', 'o', 'g', 'r', 'a', 'b', 'b', 'i', 't']
[word[i] for word in ['cat', 'dog', 'rabbit'] for i in range(len(word))]
# OUT: ['c', 'a', 't', 'd', 'o', 'g', 'r', 'a', 'b', 'b', 'i', 't']
