import copy 

logN = 6
N = 64

a = list(range(N))
kkk = [0]*N


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

def print_array(str, a):
    print(str)
    for _,i in enumerate(a):
        print(i, _)

def layer01(r):
    # index =    [0, 8, 4, 12, 
    #             1, 9, 5, 13,
    #             2, 10, 6, 14,
    #             3, 11, 7, 15] 
    index = [] 
    
    F = N//32
    for i in range(0, N//16, 1):
        for j in range(0, 4*F, F):

            _a = i + j*F 

            index.append(_a)
        
    print("index:", index)

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
        n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

        print('++++++')
        print(n0, n1, n2, n3)
        
        x[store_index[i+0]] =  n0
        x[store_index[i+1]] =  n1
        x[store_index[i+2]] =  n2
        x[store_index[i+3]] =  n3
    
    print('============')
    for _,i in enumerate(x):
        print(i, _)
    return x 


def layer23(r):
    index =    [0, 2, 1, 3,
                4, 6, 5, 7,
                8, 10, 9, 11,
                12, 14, 13, 15] 
    
    index = [] 
    F = N//16
    for i in range(0, N//4, F):
        for j in range(0, 4, 1):
            _a = i + j*F//4
            # _b = i + 2*F 
            # _c = i + F 
            # _d = i + 3*F
            
            index.append(_a)
            # index.append(_b)
            # index.append(_c)
            # index.append(_d)
        
    # print("index:", index)

    # for i in range(0, F//8, 1)

    print("index:", index)
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
        n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

        print('++++++')
        print(n0, n1, n2, n3)
        
        x[store_index[i+0]] =  n0
        x[store_index[i+1]] =  n1
        x[store_index[i+2]] =  n2
        x[store_index[i+3]] =  n3
    
    print('============')
    for _,i in enumerate(x):
        print(i, _)
    return x 

def layer45(r):
    index =    [0, 4, 8, 12,
                1, 5, 9, 13, 
                2, 6, 10, 14,
                3, 7, 11, 15
                ] 

    index = [] 
    
    F = N//32
    for i in range(0, N//16, 1):
        for j in range(0, 4*F, F):

            _a = i + j*F 

            index.append(_a)
        
    print("index:", index)

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
        n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
        n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

        print('++++++')
        print(n0, n1, n2, n3)
        
        
        x[store_index[i+0]] =  n0
        x[store_index[i+1]] =  n1
        x[store_index[i+2]] =  n2
        x[store_index[i+3]] =  n3
    
    print('============')
    for _,i in enumerate(x):
        print(i, _)
    return x 






def forward_ntt(r):

    for s in range(2, logN, 2):
        for j in range(0, 1 << s, 1):
            for k in range(0, N//4, 1 << s):
                addr = k + j
                print('------', k, addr)

                # t0 = r[addr+0]
                # t1 = r[addr+1]
                # t2 = r[addr+2]
                # t3 = r[addr+3]

                # t1, t2 = t2, t1
                
                # print(t0, t1, t2, t3)

                # n3 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
                # n2 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
                # n1 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]
                # n0 =  [t0.pop()] + [t1.pop()] + [t2.pop()] + [t3.pop()]

                # print('++++++')
                # print(n0, n1, n2, n3)
                
                
                # r[addr+0] =  n0
                # r[addr+1] =  n1
                # r[addr+2] =  n2
                # r[addr+3] =  n3
            print("++++", j)

        print('____', 1<<s)
        print_array(s, r)


print('============')
for _,i in enumerate(b):
    print(i, _)


print("\nLevel 0,1")
b = layer01(b)
print("\nLevel 2,3")
b = layer23(b)
print("\nLevel 4,5")
b = layer45(b)


# r = b 

# print_array("r", r)
# forward_ntt(r)