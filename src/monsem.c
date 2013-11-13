/**
 * \file monsem.c source code
 * \author Raphaël SCHIMCHOWITSCH, Maxime SCHMITT
 */

#include <pthread.h>
#include <stdlib.h>
#include "monsem.h"


int monsem_init(monsem_t * sem, unsigned const int value){
    int retfun;
    sem->nbinto=value;
    retfun=pthread_cond_init(&sem->iment, NULL);
    if(retfun==0){
        retfun=pthread_mutex_init(&sem->texpaf, NULL);
    }
    if(retfun!=0)
        sem->usable = INIT_USABLE_FALSE;
    else
        sem->usable = INIT_USABLE_TRUE;
    return retfun;
}

int monsem_wait(monsem_t *sem){
    int retfun;
    if( sem->usable == INIT_USABLE_TRUE ){
        retfun=pthread_mutex_lock(&sem->texpaf);
        while(sem->nbinto==0 && retfun==0){
            retfun=pthread_cond_wait(&sem->iment, &sem->texpaf);
        }
        if(retfun==0){
            sem->nbinto--;
            retfun=pthread_mutex_unlock(&sem->texpaf);
        }
    }
    else{
        retfun=-1;
        errno=EINVAL;
    }
    return retfun;
}

int monsem_post(monsem_t *sem){
    int retfun;
    if( sem->usable == INIT_USABLE_TRUE ){
        retfun=pthread_mutex_lock(&sem->texpaf);
        if(retfun==0){
            if(sem->nbinto == MONSEM_VALUE_MAX){
                retfun=-1;
                errno=EOVERFLOW;
            }
            else{
                sem->nbinto++;
                retfun=pthread_cond_signal(&sem->iment);
                if(retfun==0)
                    retfun=pthread_mutex_unlock(&sem->texpaf);
            }
        }
    }
    else{
        retfun=-1;
        errno=EINVAL;
    }
    return retfun;
}

int monsem_getvalue(monsem_t *sem, int *value){
    int retval;
    if( sem->usable == INIT_USABLE_TRUE ){
        *value=sem->nbinto;
        retval=0;
    }
    else{
        retval=-1;
        errno=EINVAL;
    }
    return retval;
}

int monsem_destroy(monsem_t *sem){
    int retval;
    if (sem->usable == INIT_USABLE_TRUE){
        sem->usable = INIT_USABLE_FALSE;
        retval=pthread_cond_destroy(&sem->iment);
        if(retval == 0)
            retval=pthread_mutex_destroy(&sem->texpaf);
    }
    else{
        retval=-1;
        errno=EINVAL;
    }
    return retval;
}
