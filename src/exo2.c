#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "exo2.h"


int monsem_init(monsem_t * monstr, unsigned int val){
	monsem_t *monsem=(monsem_t *) malloc(sizeof(monsem_t));
	int retfun, errnum=0;
	if(monsem==NULL)
		errnum=1;
	monsem->nbinto=val;
	retfun=pthread_cond_init(&monsem->iment, NULL);
	if(retfun!=0)
		errnum=1;
	retfun=pthread_mutex_init(&monsem->texpaf, NULL);
	if(retfun!=0)
		errnum=1;
	return errnum;
}

int monsem_wait(monsem_t *monstr){
	int retfun, errnum=0;
	retfun=pthread_mutex_lock(&monstr->texpaf);
	if(retfun!=0)
		errnum=1;
	while(monstr->nbinto==0){
		retfun=pthread_cond_wait(&monstr->iment, &monstr->texpaf);
		if(retfun!=0){
			errnum=1;
			break;
		}
	}
	monstr->nbinto--;
	retfun=pthread_mutex_unlock(&monstr->texpaf);
	if(retfun!=0)
		errnum=1;
	return errnum;
}

int monsem_post(monsem_t *monstr){
	int retfun, errnum;
	retfun=pthread_mutex_lock(&monstr->texpaf);
	if(retfun!=0)
		errnum=1;
	monstr->nbinto++;
	retfun=pthread_mutex_unlock(&monstr->texpaf);
	if(retfun!=0)
		errnum=1;
	retfun=pthread_cond_signal(&monstr->iment);
	if(retfun!=0)
		errnum=1;
	return errnum;
}

int monsem_getvalue(monsem_t *monstr, int *value){
	*value=monstr->nbinto;
	return 0;
}


