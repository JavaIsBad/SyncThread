/**
 * \file monsem.c <source code>
 * \author Raphaël SCHIMCHOWITSCH, Maxime SCHMITT
 */

#include <pthread.h>
#include <stdlib.h>
#include "monsem.h"


int monsem_init(monsem_t * monstr, unsigned const int val){
    int retfun;
    monstr->nbinto=val;
    retfun=pthread_cond_init(&monstr->iment, NULL);
    if(retfun==0){
        retfun=pthread_mutex_init(&monstr->texpaf, NULL);
    }
    if(retfun!=0)
        monstr->usable = INIT_USABLE_FALSE;
    else
        monstr->usable = INIT_USABLE_TRUE;
    return retfun;
}

int monsem_wait(monsem_t *monstr){
    int retfun;
    if( monstr->usable == INIT_USABLE_TRUE ){
        retfun=pthread_mutex_lock(&monstr->texpaf);
        while(monstr->nbinto==0 && retfun==0){
            retfun=pthread_cond_wait(&monstr->iment, &monstr->texpaf);
        }
        if(retfun==0){
            monstr->nbinto--;
            retfun=pthread_mutex_unlock(&monstr->texpaf);
        }
    }
    else{
        retfun=-1;
        errno=EINVAL;
    }
    return retfun;
}

int monsem_post(monsem_t *monstr){
    int retfun;
    if( monstr->usable == INIT_USABLE_TRUE ){
        retfun=pthread_mutex_lock(&monstr->texpaf);
        if(retfun==0){
            if(monstr->nbinto == MONSEM_VALUE_MAX){
                retfun=-1;
                errno=EOVERFLOW;
            }
            else{
                monstr->nbinto++;
                retfun=pthread_cond_signal(&monstr->iment);
                if(retfun==0)
                    retfun=pthread_mutex_unlock(&monstr->texpaf);
            }
        }
    }
    else{
        retfun=-1;
        errno=EINVAL;
    }
    return retfun;
}

int monsem_getvalue(monsem_t *monstr, int *value){
    int retval;
    if( monstr->usable == INIT_USABLE_TRUE ){
        *value=monstr->nbinto;
        retval=0;
    }
    else{
        retval=-1;
        errno=EINVAL;
    }
}

int monsem_destroy(monsem_t *monstr){
    int retval;
    if (monstr->usable == INIT_USABLE_TRUE){
        monstr->usable = INIT_USABLE_FALSE;
        retval=pthread_cond_destroy(&monstr->iment);
        if(retval == 0)
            retval=pthread_mutex_destroy(&monstr->texpaf);
    }
    else{
        retval=-1;
        errno=EINVAL;
    }
    return retval;
}
