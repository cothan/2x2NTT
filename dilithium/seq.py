import copy 

a = list(range(64))
kkk = [0]*64


def reshape(a):
    b = [] 
    for i in range(0, len(a), 4):
        t = a[i:i+4]
        b.append(t)
    return b 

b = reshape(a)
zero = reshape(kkk)

def extract(array):
    pass 

def layer01(r):
    index =    [0, 8, 4, 12, 
                1, 9, 5, 13,
                2, 10, 6, 14,
                3, 11, 7, 15] 
    store_index = index

    x = copy.deepcopy(zero)
    for i in range(0, len(index), 4):
        t0 = b[index[i]]
        t1 = b[index[i+1]]
        t2 = b[index[i+2]]
        t3 = b[index[i+3]]


        t1, t2 = t2, t1
        
        print('------')
        print(t0, t1, t2, t3)
        

        n3 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

        print('++++++')
        print(n0, n1, n2, n3)
        
        x[store_index[i+0]] =  n0
        x[store_index[i+1]] =  n1
        x[store_index[i+2]] =  n2
        x[store_index[i+3]] =  n3
    
    print('============')
    for i in x:
        print(i)
    return x 


def layer23(r):
    index =    [0, 2, 1, 3,
                4, 6, 5, 7,
                8, 10, 9, 11,
                12, 14, 13, 15] 
    store_index = index

    x = copy.deepcopy(zero)
    for i in range(0, len(index), 4):
        t0 = b[index[i]]
        t1 = b[index[i+1]]
        t2 = b[index[i+2]]
        t3 = b[index[i+3]]


        t1, t2 = t2, t1
        
        print('------')
        print(t0, t1, t2, t3)

        n3 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

        print('++++++')
        print(n0, n1, n2, n3)
        
        x[store_index[i+0]] =  n0
        x[store_index[i+1]] =  n1
        x[store_index[i+2]] =  n2
        x[store_index[i+3]] =  n3
    
    print('============')
    for i in x:
        print(i)
    return x 

def layer45(r):
    index =    [0, 4, 8, 12,
                1, 5, 9, 13, 
                2, 6, 10, 14,
                3, 7, 11, 15
                ] 
    store_index = index

    x = copy.deepcopy(zero)
    for i in range(0, len(index), 4):
        t0 = b[index[i]]
        t1 = b[index[i+1]]
        t2 = b[index[i+2]]
        t3 = b[index[i+3]]


        t1, t2 = t2, t1
        
        print('------')
        print(t0, t1, t2, t3)

        n3 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

        print('++++++')
        print(n0, n1, n2, n3)
        
        
        x[store_index[i+0]] =  n0
        x[store_index[i+1]] =  n1
        x[store_index[i+2]] =  n2
        x[store_index[i+3]] =  n3
    
    print('============')
    for i in x:
        print(i)
    return x 

for index, item in enumerate(b):
    print(index, item)


b = layer01(b)

b = layer23(b)

b = layer45(b)