#ifndef __EXO2_H
#define __EXO2_H

typedef struct{
	pthread_mutex_t texpaf;
	unsigned int nbinto;
	pthread_cond_t iment;
}
monsem_t;

int monsem_init(monsem_t *, unsigned int);
int monsem_wait(monsem_t *);
int monsem_post(monsem_t *);
int monsem_getvalue(monsem_t *, int *);


#endif //__EXO2_H
