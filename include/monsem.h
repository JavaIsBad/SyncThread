#ifndef __EXO2_H
#define __EXO2_H

#include <pthread.h>
#include <limits.h>
#include <errno.h>

#define MONSEM_VALUE_MAX UINT_MAX
#define MONSEM_FAILED ((monsem_t *) 0)
#define INIT_USABLE_TRUE 1
#define INIT_USABLE_FALSE 0

/**
 * \typedef monsem_t
 * \brief Une structure permetant de faire une semaphore personnalisee
 */

typedef struct{
    pthread_mutex_t texpaf; //<- Le mutex servant a l'acces
    unsigned int nbinto;    //<- La valeur de la semaphore
    char usable;            //<- Variable servant au test d'utilisation
    pthread_cond_t iment;   //<- Condition pour liberer un thread si la semaphore est bloquante
}
monsem_t;

/**
 * \brief Initialise une semaphore binaire
 */

static const monsem_t MONSEM_INITIALIZER={ PTHREAD_MUTEX_INITIALIZER, 1u, INIT_USABLE_TRUE, PTHREAD_COND_INITIALIZER };

/**
 * \brief Initialise une semaphore avec une certaine valeur
 * \param sem La semaphore a initialiser
 * \param value La valeur a attribuer a la semaphore
 * \return 0 si succes, autre chose si erreur
 *
 * Errno est positionne le cas echeant
 * voir les erreurs dans le man de pthread_cond_init et pthread_mutex_init
 */

int monsem_init(monsem_t * sem, unsigned int value);

/**
 * \brief Decremente la semaphore et attend passivement si elle est bloquante
 * \param sem Une semaphore initialisee
 * \return 0 si succes, autre chose si erreur
 *
 * Errno est positionne le cas echeant
 * voir les erreurs dans le man de pthread_cond_wait et pthread_mutex_unlock/lock
 * EINVAL est positionne si la semaphore n'est pas valide
 */

int monsem_wait(monsem_t *sem);

int monsem_post(monsem_t *sem);
int monsem_getvalue(monsem_t *sem, int *value);
int monsem_destroy(monsem_t *sem);

#endif //__EXO2_H
