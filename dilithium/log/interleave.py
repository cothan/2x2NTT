a = open('even.txt').readlines()
b = open('odd.txt').readlines()

c = [] 

assert len(a) == len(b)
dis = 2
for i in range(0, len(a), dis):
    if dis == 2:
        c += a[i: i+dis]
        c += b[i: i+dis]
    else:
        c += [a[i]]
        c += [b[i]]

with open('out.txt', 'a+') as g:
    g.writelines(c)

