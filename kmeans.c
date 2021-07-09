#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static double** kMeans(double** vecs,int K, int max_iter, int dim, Py_ssize_t num_of_vecs);
static double l2_norm(double* vec, double* centroid, int dimention);
static void sum_vecs(double* sum_vec, double* vec, int d);
static int eq_vecs(double* vec1, double* vec2, int d);
static double* vecConvert(PyObject* vecPy, Py_ssize_t dim);
static double* copyVec(double* vec, int dim);



static PyObject* fit(PyObject* self, PyObject* args)
{
    PyObject *_vecsPy, *vecPy, *lst, *vec, *num;
    int K,max_iter, dim;
    Py_ssize_t j, i, n;
    double **vecs, **res;
    if(!PyArg_ParseTuple(args, "Oiii:here1", &_vecsPy, &K, &max_iter, &dim))
    {
        return NULL;
    }
    n= PyList_Size(_vecsPy); /*number of datapoints*/
    vecs= (double**)calloc(n, sizeof(double*)); /*array of all datapoints*/
    assert(vecs!=NULL);
    for(i=0; i<n; i++)
    {
        vecPy= PyList_GetItem(_vecsPy, i);
        vecs[i]=vecConvert(vecPy,dim); 
    }
    res= kMeans(vecs, K, max_iter, dim, n); /*array of chosen centroids*/
    
    lst= PyList_New(K); /*convert the result array to a python list*/
    if(!lst || !PyList_Check(lst))
    {
        return NULL;
    }
    for(i=0; i<K; i++)
    {
        vec= PyList_New(dim);
        if(!vec || !PyList_Check(vec))
        {
            return NULL;
        }
        for(j=0; j<dim;j++)
        {
            num= PyFloat_FromDouble(res[i][j]); /*num= python(res[i][j]) */
            assert(type(num)==PyObject*);
            if(!num)
            {
                Py_DECREF(lst);
                return NULL;
            } 
            PyList_SetItem(vec, j, num); /*vec[j]=num */
            
        }
        PyList_SetItem(lst, i, vec); /*lst[i]=vec*/

    }
    free(res);
    return lst;
}
/* convert PyObject vec to C array*/
static double* vecConvert(PyObject* vecPy, Py_ssize_t dim)
{
    Py_ssize_t i;
    PyObject* item;
    double* vec= (double*)calloc(dim, sizeof(double));
    assert(vec!=NULL);
    for(i=0; i<dim; i++)
    {
        item= PyList_GetItem(vecPy, i);
        assert(type(item)==PyObject*);
        if (!PyFloat_Check(item)) {
            printf("%s", " error 1 in convert vec 148\n");
        }
        vec[i]= PyFloat_AsDouble(item); 
        if (vec[i]  == -1 && PyErr_Occurred()){
            printf("%s", " error 2 in convert vec 148\n"); 
        }        
    }
    return vec;
}

static double** kMeans(double** vecs,int K, int max_iter, int dim, Py_ssize_t num_of_vecs)
{
    typedef struct cluster_element { /* implement each cluster as linked_list */
        double* vec; 
        struct cluster_element* next; 
    } cluster_element;

    typedef struct cluster_head { /* linked list of all clusters */
        struct cluster_element* centroid; 
        struct cluster_head* next; /* next cluster */
    } cluster_head;
   
    double** res;
    int i=0; /* loop index */
   /* int ind=0;*/
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
        for (i = 0; i < num_of_vecs; i++) { /* go over all vectors xi */\
            current_cluster = head;
            current_centroid = current_cluster-> centroid; 
            min_dist = l2_norm(vecs[i], current_centroid-> vec, dim); 
            p_min = head; /*pointer to the current closest cluster  */
            current_cluster = current_cluster ->next;
            while (current_cluster != NULL) { /* go over all cluster */
                current_centroid = current_cluster -> centroid;
                dist = l2_norm(vecs[i], current_centroid -> vec, dim);
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
            sum_vec= (double*)calloc(dim, sizeof(double));
            assert(sum_vec!=NULL);
            curr= current_cluster->centroid->next; /*first vector in cluster*/
            
            cluster_len=0;
            while(curr!=NULL) /*go over cluster vevtors */
            {
                sum_vecs(sum_vec, curr->vec, dim); 
                cluster_len+=1;          
                curr=curr->next;    
            }          
            for(i=0; i<dim; i++)
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
            b2= eq_vecs(sum_vec, current_cluster->centroid->vec,dim);
            
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
    res= (double**)calloc(K, sizeof(double*)); /*array of final centroids to return*/
    i=0;
    while(current_cluster!=NULL)
    {
        current_centroid= current_cluster->centroid;
        res[i]=copyVec(current_centroid->vec,dim);

        free(current_centroid->vec);
        free(current_centroid);
        tmp1= current_cluster;
        current_cluster= current_cluster->next;
        i++; 
        free(tmp1);
    }
    for(i=0; i < num_of_vecs; i++)
    {
        if(*(vecs+i)!=NULL)
        {
            free(*(vecs+i));
        }
    }
    free(vecs);
    return res;
}

static double* copyVec(double* vec, int dim)
{
    int i=0;
    double* res= (double*)calloc(dim, sizeof(double));
    for(i=0; i<dim; i++)
    {
        res[i]= vec[i];
    }
    return res;
}

static int eq_vecs(double* vec1, double* vec2, int dim)
{
    /*
    * Return 1 if vecs aren't the same, or 0 otherwise
    */
   int i;
    for(i=0; i<dim; i++)
    {
        if(*(vec1+i)!=*(vec2+i))
        {
            return 1;
        }
    }
    return 0;
}

static void sum_vecs(double* sum_vec, double* vec, int dim)
{
    int i;
    assert(vec!=NULL);
    assert(sum_vec!=NULL);
    for(i=0; i<dim; i++)
    {
        *(sum_vec+i)+= *(vec+i);
    }
}

static double l2_norm(double* vec, double* centroid, int dimention) {
    double sum1 = 0;
    int i;
    for (i=0; i < dimention; i++){
        sum1 += (vec[i] - centroid[i])*(vec[i] - centroid[i]);
    }
    return sum1;
}

static PyMethodDef _methods[]= {
    {"fit", 
    (PyCFunction) fit,
    METH_VARARGS,
    PyDoc_STR("Running kMeans")},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef _moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    _methods
};

PyMODINIT_FUNC
PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&_moduledef);
    if (!m) {
        return NULL;
    }
    return m;
}