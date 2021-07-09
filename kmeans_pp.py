import mykmeanssp as myk
import numpy as np

def kmeans_pp(K, input, max_iter=300):
    #check if input is valid
    if K <= 0:
        print("invalid K")
        exit(1)
    n = len(input) #num of vecs in input 
    if K>=n:
        print("invalid K")
        exit(1)
    if max_iter < 0:
        print("max iter invalid")
        exit(1)

    indexes = [input[i][0] for i in range(n)] #list of observations's indices
    n = int(max(indexes)) + 1 
    vecs= [0 for i in range(n)] #list of vecs located in their original indexes
    for i in range(n):
        ind=int(input[i][0])
        vecs[ind]= list(input[i][1:])
        for j in range(len(vecs[ind])):
            vecs[ind][j]= float(vecs[ind][j])

    np.random.seed(0)
    ind=int(np.random.choice(sorted(indexes)))#sample a random index
    miu_1= vecs[ind] #get miu1 acoording to the index we sampled
    res_ind= [ind] #list of chosen miu indicies
    Z=1
    D= [0 for i in range(n)]
    miu= [miu_1] #list of chosen miu vectors
    while Z<K: 
        
        for i,vec in enumerate(vecs): #go over xi (vecs)
            if vec!=0:
                min_dist= np.linalg.norm(np.array(miu_1)-np.array(vec))**2 
                for j in range(1,Z): #go over the centroids we found so far
                    dist= np.linalg.norm(np.array(vec)-np.array(miu[j]))**2
                    if dist < min_dist:
                        min_dist = dist
                D[i]= min_dist
        
        assert len(D)==n
        Z+=1
        S= sum(D)
        prob=[(D[i]/S) for i in range(n)] #list of probability for each xi
        ind=int(np.random.choice(a=n, p=prob))
        miu.append(vecs[ind])
        res_ind.append(ind)
    
    inds_to_print=""
    for i in range(len(res_ind)-1):
        inds_to_print += str(res_ind[i]) + ","
    print(inds_to_print, end = "")
    print(res_ind[-1])

    for i in range(K): # change the output list vecs - k initial centroids first
        temp1= vecs[i]
        vecs[i]= vecs[res_ind[i]]
        vecs[res_ind[i]]=temp1
    return vecs,res_ind

def merge_data(file1, file2):
    import pandas as pd
    input1= pd.read_csv(file1, header=None)
    input2= pd.read_csv(file2, header=None)
    n= len(input1.columns)
    headers= ["index"]
    for i in range(1,n):
        headers.append("x")
    input1.columns= headers
    input2.columns= headers
    import numpy as np
    input= np.array(pd.merge(input1,input2, on="index"))
    n= len(input)
    return np.array(input)

if __name__ == '__main__':
    import sys
    args= sys.argv[1:]
    is_int = True
    try:
        int(args[0])
    except ValueError:
        is_int = False
    if not(is_int):
        print("invalid type of K")
        exit(1)
    if len(args)==3:
        input= merge_data(args[1], args[2])
        vecs, res_ind=kmeans_pp(int(args[0]), input)
        res=myk.fit(vecs, int(args[0]), 300, len(vecs[0]))
    elif len(args)==4:
        try:
            int(args[1])
        except ValueError:
            is_int = False
        if not(is_int):
            print("invalid type of max_iter")
            exit(1)
        #assert type(args[1])==int
        input= merge_data(args[2], args[3])
        vecs, res_ind=kmeans_pp(int(args[0]), input, int(args[1]))
        res=myk.fit(vecs, int(args[0]),int(args[1]), len(vecs[0]))
    else:
        print("too many or not enough arguments")
        exit(1)
    rounded_res= [np.round(decimals=4, a=res[i]) for i in range(len(res))]
    row=""
    for i in range(len(rounded_res)):
        for j in range(len(rounded_res[i])-1):
            row+= str(rounded_res[i][j])+","
        row+= str(rounded_res[i][-1])+ '\n'
    print(row)
    
