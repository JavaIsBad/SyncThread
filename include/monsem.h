#ifndef __EXO2_H
#define __EXO2_H

#include <pthread.h>
#include <limits.h>
#include <errno.h>

#define MONSEM_VALUE_MAX UINT_MAX
#define MONSEM_FAILED ((monsem_t *) 0)
#define INIT_USABLE_TRUE 1
#define INIT_USABLE_FALSE 0

typedef struct{
    pthread_mutex_t texpaf;
    unsigned int nbinto;
    char usable;
    pthread_cond_t iment;
}
monsem_t;

static const monsem_t MONSEM_INITIALIZER={ PTHREAD_MUTEX_INITIALIZER, 1u, INIT_USABLE_TRUE, PTHREAD_COND_INITIALIZER};

int monsem_init(monsem_t *, unsigned int);
int monsem_wait(monsem_t *);
int monsem_post(monsem_t *);
int monsem_getvalue(monsem_t *, int *);


#endif //__EXO2_H
