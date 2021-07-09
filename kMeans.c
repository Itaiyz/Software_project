#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void kMeans(int K, int max_iter);
int main(int argc, char** argv);
int get_dim();
void convertToLst(double* vec, char* row);
double l2_norm(double* vec, double* centroid, int dimention);
void sum_vecs(double* sum_vec, double* vec, int d);
int eq_vecs(double* vec1, double* vec2, int d);
void print_vec(double* vec, int d);
int isZero(double* vec, int d);
int is_int(char* s);
#define MAX_ITER 200
#define MAXWORD 1000
#define kMeansd(K) (kMeans(K, 200))

int main(int argc, char** argv)
{    
    if (argc == 1) {
        printf("%s", "invalid input");
        exit(1);
    }
    if (!is_int(argv[1])) {
            printf("%s", "invalid input");
            exit(1);
    }
    else {
    if(argc==2)
    {
        /*atoi converts string to int
        kMeansd is a macro for the default value */ 
        kMeansd(atoi(argv[1]));
        
    }
    else
    {
        if (!is_int(argv[2])) {
            printf("%s", "invalid input");
            exit(1);
        }
        else {
        kMeans(atoi(argv[1]), atoi(argv[2]));
        }
    }
    }
    return 0;
}

int is_int(char* s) { /*check if a string is an integet, return 1 if it is*/
    while (*s != '\0') {
        if ((*s < '0') || (*s > '9')) {
            return 0;
        }
        s++;
    }
    return 1;
}

int get_dim() {
    int dim=0;
    char c;
    scanf("%c", &c);
        while (c != '\n'){
            if (c == ',') {
                dim++;
            }
            c = getchar();
        }
            if (dim>0) {
                dim++;
            }
    rewind(stdin);
    return dim;
}


void kMeans(int K, int max_iter)
{
    typedef struct cluster_element { /* implement each cluster as linked_list */
        double* vec; 
        struct cluster_element* next; 
    } cluster_element;

    typedef struct cluster_head { /* linked list of all clusters */
        struct cluster_element* centroid; 
        struct cluster_head* next; /* next cluster */
    } cluster_head;
   
    double** vecs; /* array of all vectors */
    double** temp; /*array for resizing vecs */
    double* vec; /*temporary vector array */
    
    int i=0,j,size; /* loop index */
    int d; /*dim */
    int ind=0;
    int b1 = 1, b2; 
    double min_dist = 0, dist = 0; 
    int cluster_len;
    cluster_element* tmp, *curr;
    double* sum_vec;
    double* vec_to_free;
    cluster_head* tmp1;
    struct cluster_head *current_cluster, *new_cluster, *head, *p_min;
    struct cluster_element *current_element, *current_centroid, *new_element, *element_to_free;
    int count_centroids=0;
    int counter = 0;
    /*variables for the scanf loop */
    double coordinate;
    char c;
    d = get_dim();
    vecs= (double**)calloc(MAX_ITER, sizeof(double*)); /*MAX_ITER is an initial input size, if exceeded- we copy it */
    assert(vecs!=NULL);
    size= MAX_ITER;
        
    vec=(double*)calloc(d, sizeof(double));
    assert(vec!=NULL);
    while (scanf("%lf%c", &coordinate, &c) == 2) { /* loop until EOF */
        if (c != '\n') {
            *(vec+i) = coordinate;
            i++;
        }
        else {
            *(vec+i) = coordinate;
            i = 0;
        if(ind>= size) /*size of vecs will be |double*|*# of values */
        {
            temp= (double**)calloc(2*size, sizeof(double*)); /*allocating array twice its size */
            assert(temp!=NULL);
            for(j=0; j<size; j++)
            {
                *(temp+j)= *(vecs+j);
            }

            free(vecs);
            vecs=temp;
            temp=NULL;
            size*=2;
        }
        
        vecs[ind]= vec; /* changed it from vecs[ind++]= vec; */
        ind++;
        vec=(double*)calloc(d, sizeof(double));
        assert(vec!=NULL);
        }
        
    }
    free(vec);
    if(size!=ind)
    {
        size=ind;
    }
    /*check if k < n, n is the number of input points*/
    if ((K >= size) || (K == 0)) {
        printf("invalid input!");
        exit(1);
    }
   
    /* until here - create vecs: array of all the input vectors 
    from here - create list of clusters and initialize centroids
    */

    /* initialize 2D linked list */

    current_cluster = (cluster_head*)malloc(sizeof(cluster_head));
    assert(current_cluster != NULL);
    head = current_cluster; /* pointer to the beginning of the clusters list */

    for (i=0; i<K-1; i++) { /* initialize all clusters */
        current_element = (cluster_element*)malloc(sizeof(cluster_element));
        assert(current_element != NULL);
        current_element ->vec = *(vecs+i);
        current_element-> next = NULL;
        current_cluster->centroid = current_element;

        new_cluster = (cluster_head*)malloc(sizeof(cluster_head));
        assert(new_cluster != NULL);
        current_cluster->next = new_cluster;
        current_cluster = new_cluster;
    }
    current_element = (cluster_element*)malloc(sizeof(cluster_element));
    assert(current_element != NULL);
    current_element ->vec = *(vecs+i);
    current_element-> next = NULL;
    current_cluster->centroid = current_element;
    current_cluster->next = NULL;
    
    while (counter < max_iter+1 && b1) {
        counter++;
           /* assign each xi to the closest cluster */
        for (i = 0; i < size; i++) { /* go over all vectors xi */\
            current_cluster = head;
            current_centroid = current_cluster-> centroid; 
            min_dist = l2_norm(vecs[i], current_centroid-> vec, d); 
            p_min = head; /*pointer to the current closest cluster  */
            current_cluster = current_cluster ->next;
            while (current_cluster != NULL) { /* go over all cluster */
                current_centroid = current_cluster -> centroid;
                dist = l2_norm(vecs[i], current_centroid -> vec, d);
                if (dist < min_dist) { 
                    p_min = current_cluster;
                    min_dist = dist;
                }
                current_cluster = current_cluster-> next;               
            }
            /* assign vec xi to min_dist cluster */
            new_element = (cluster_element*)malloc(sizeof(cluster_element)); 
            assert(new_element != NULL);
            new_element ->vec = *(vecs+i);
            current_centroid = p_min-> centroid; /* the first element of the closest cluster */
            count_centroids++;
           
            if (current_centroid-> next == NULL) {
                current_centroid-> next = new_element;
                new_element-> next = NULL;
            } 
            else {
                new_element -> next = current_centroid-> next;
                current_centroid-> next = new_element;
            }

        }
        /* update centroid */
        b1 = 0;
        current_cluster = head;
        while (current_cluster!= NULL) { /* go over clusters list*/
            sum_vec= (double*)calloc(d, sizeof(double));
            assert(sum_vec!=NULL);
            curr= current_cluster->centroid->next; /*first vector in cluster*/
            
            cluster_len=0;
            while(curr!=NULL) /*go over cluster vevtors */
            {
             
                sum_vecs(sum_vec, curr->vec, d); 
                cluster_len+=1;          
                curr=curr->next;    
            }          
            
            for(i=0; i<d; i++)
            {
                if(cluster_len!=0)
                {
                *(sum_vec+i)= *(sum_vec+i)/cluster_len;
                }
            }

            if (sum_vec == NULL) {
                printf("%s", "sum vec is NULL");
                exit(1);
            }
              if (current_cluster->centroid->vec == NULL) {
                printf("%s", "centroid is NULL");
                exit(1);
            }
            b2= eq_vecs(sum_vec, current_cluster->centroid->vec,d);
            
            if(b2)
            {
                b1=1;
            }
            vec_to_free = current_cluster->centroid->vec;
            current_cluster->centroid->vec = sum_vec;
            if (counter != 1) {
                free(vec_to_free);
            } 
            element_to_free = current_cluster->centroid->next;
            current_cluster-> centroid->next= NULL;
            if (element_to_free != NULL) {
                while (element_to_free != NULL) {
                    tmp = element_to_free;
                    element_to_free = element_to_free-> next;
                    free(tmp);
                }

            }
            current_cluster= current_cluster->next;
        }
    }
    current_cluster= head; 
    while(current_cluster!=NULL)
    {
        current_centroid= current_cluster->centroid;
        print_vec(current_centroid->vec, d);
        if(current_cluster-> next!=NULL) {
            printf("%c", '\n');
        }
       
        free(current_centroid->vec);
        free(current_centroid);
        tmp1= current_cluster;
        current_cluster= current_cluster->next;
        free(tmp1);
    }
    for(i=0; i<size; i++)
    {
        if(*(vecs+i)!=NULL)
        {
            free(*(vecs+i));

        }
        
    }
    free(vecs);
    
}
void print_vec(double* vec, int d)
{
    int i=0; 
    for(i=0; i<d; i++)
    {
        if(i!=d-1)
        {
             printf("%.4f%c", *(vec+i), ',');
        }
        else 
        {
            printf("%.4f", *(vec+i));
        }
       
    }
}
int isZero(double* vec, int d)
{
    /* return 1 if vec is a zero vector, or 1 otherwise
    */
    int i; 
    for(i=0; i<d; i++)
    {
        if(vec[i]!=0)
        {
            return 0;
        }
    }
    return 1;
}
int eq_vecs(double* vec1, double* vec2, int d)
{
    /*
    * Return 1 if vecs aren't the same, or 0 otherwise
    */
   int i;
    for(i=0; i<d; i++)
    {
        if(*(vec1+i)!=*(vec2+i))
        {
            return 1;
        }

    }
    return 0;
}

void sum_vecs(double* sum_vec, double* vec, int d)
{
    int i;
    assert(vec!=NULL);
    assert(sum_vec!=NULL);
    for(i=0; i<d; i++)
    {
        *(sum_vec+i)+= *(vec+i);
        
    }

}

double l2_norm(double* vec, double* centroid, int dimention) {
    double sum1 = 0;
    int i;
    for (i=0; i < dimention; i++){
        sum1 += (vec[i] - centroid[i])*(vec[i] - centroid[i]);
    }
    return sum1;
}
