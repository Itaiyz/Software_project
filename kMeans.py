#print("before")
def kMeans(K, max_iter=200):
    #print("after")
    '''
    f= open(filename, 'r')
    if f.mode != 'r':
        print("failed to reas input")
        return
    print("hi")
    '''
    if type(K)!=int:
        print("wrong type for K")
    if type(max_iter)!=int:
        print("wrong type for max_iter")
    assert type(K)==type(max_iter)==int
    vecs= []
    vec =' '
    while True:
        try:
            vec= input()
        except:
            break
        x= vec.split(",")
        for i in range(len(x)):
            x[i]= float(x[i])
        vecs.append(x)
    #The first value of every list is the centroid
    if K==0 or K>=len(vecs):
        print("invalid K")
        exit(1)
    clusters= [[vecs[i]] for i in range(K)]
    count=0
    #print(len(vecs))
    
    b1= True
    while count<max_iter+1 and b1 :
        clusters= [[clusters[i][0]] for i in range(K)]
        for i in range(K):
            assert(len(clusters[i])==1)
        for vec in vecs:
            min_index=0
            min_dist= l2_norm(vec, clusters[0][0])
            for i,cluster in enumerate(clusters):
                dist= l2_norm(cluster[0], vec)
                if dist<min_dist:
                    min_index=i
                    min_dist= dist
            clusters[min_index].append(vec.copy())
        b1= False
        for cluster in clusters:
            b2=update_center(cluster)
            if b2:
                b1=True
           
            
        count+=1
    assert K==len(clusters)
    res= [clusters[i][0] for i in range(K)]
    row=""
    for v in res:
        for i in range(len(v)-1):
            row+=format(v[i], '.4f' )+ ","
        row+=format(v[-1], '.4f')+'\n'
    print(row)  
    
def update_center(cluster):
     n= len(cluster)
     centroid= []
     d= len(cluster[0])
     tmp= cluster[0]
     sum_vec=[0 for i in range(d)]
     for i in range(1,n):
         for j in range(d):
             sum_vec[j]+= cluster[i][j]
     centroid= [sum_vec[i]/(n-1) for i in range(d)]
     if eq_vecs(sum_vec, tmp):
            return False
     cluster[0]= centroid
     return True



'''
def update_center(cluster):
    
    updates centroid. Returns false if there is no change, or true otherwisw
    
    n= len(cluster)
    centroid= []
    d= len(cluster[0])
    #print(d)
    tmp= cluster[0]
    val=0
    for i in range(d):
        for j in range(n):
            val+= cluster[j][i]
        centroid.append(val/n)
        val=0
    if eq_vecs(centroid, tmp):
        return False
    cluster[0]= centroid
    return True
    

'''
def eq_vecs(vec1, vec2):
    for i in range(len(vec1)):
        if vec1[i]!=vec2[i]:
            return False
    return True

def l2_norm(vec1, vec2):
    #print(type(vec1[0]))
    #print(type(vec2[0]))
    n= len(vec1)
    sum1=0
    for i in range(n):
        sum1+=(vec1[i]-vec2[i])**2
    return sum1**0.5

if __name__ == '__main__':
    import sys
    args= sys.argv[1:]
    print(args)
    #assert type(args[0])==int
    if len(args)==1:
        kMeans(int(args[0]))
    elif len(args)==2:
        #assert type(args[1])==int
        kMeans(int(args[0]),int(args[1]))
    else:
        print("too many or not enough arguments")
        exit(1)





