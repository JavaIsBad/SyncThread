
/**
 * \file monsem.h
 * \author Maxime SCHMITT, Raphaël SCHIMCHOWITSCH
 */

#ifndef __EXO2_H
#define __EXO2_H

#include <pthread.h>
#include <limits.h>
#include <errno.h>

/**
 * \brief Sert au test lors d'un increment
 */
#define MONSEM_VALUE_MAX UINT_MAX
/**
 * \brief Sert a rien
 */
#define MONSEM_FAILED ((monsem_t *) 0)
/**
 * \brief Sert a initialiser lors de la creation
 */
#define INIT_USABLE_TRUE 1
/**
 * \brief Sert a rendre la semaphore inutilisable
 */
#define INIT_USABLE_FALSE 0

/**
 * \struct monsem_t
 * \brief Une structure permetant de faire une semaphore personnalisee
 */

typedef struct{
    pthread_mutex_t texpaf; //!< Le mutex servant a l'acces
    unsigned int nbinto;    //!< La valeur de la semaphore
    char usable;            //!< Variable servant au test d'utilisation
    pthread_cond_t iment;   //!< Condition pour liberer un thread si la semaphore est bloquante
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

int monsem_init(monsem_t * sem, unsigned const int value);

/**
 * \brief Decremente la semaphore et attend passivement si elle est bloquante
 * \param sem Une semaphore initialisee
 * \return 0 si succes, autre chose si erreur
 * Errno est positionne le cas echeant
 * voir les erreurs dans le man de pthread_cond_wait et pthread_mutex_unlock/lock
 * EINVAL est positionne si la semaphore n'est pas valide
 */

int monsem_wait(monsem_t *sem);

/**
 * \brief Incremente la semaphore
 * \param sem Une semaphore
 * \return 0 si succes, autre chose sinon
 * Errno est positionne le cas echeant
 * voir les erreurs dans le man de pthread_cond_signal et pthread_mutex_unlock/lock
 * EOVERFLOW est positionne si la semaphore est deja a sa valeur maximmale
 * EINVAL est positionne si la semaphore n'est pas valide
 */

int monsem_post(monsem_t *sem);

/**
 * \brief Retourne la valeure de la semaphore
 * \param sem La semaphore dont il faut retourner la valeur
 * \param value Un pointeur vers un entier dans lequel la valeur sera sauvegardee
 * \return 0 si succes, autre chose sinon
 * Errno est positionne le cas echeant
 * EINVAL est positionne si la semaphore n'est pas valide
 */

int monsem_getvalue(monsem_t *sem, int *value);

/**
 * \brief Detruit la semaphore. Si la semaphore est en utilisation le resultat peut etre exotique
 * \param sem La semaphre a detruire
 * \return 0 si succes, autre chose sinon
 * Errno est positionne le cas echeant
 * EINVAL est positionne si la semaphore n'est pas valide
 * Voir le man pthread_cond_destroy et pthread_mutex_destroy pour les autres erreurs
 */

int monsem_destroy(monsem_t *sem);

#endif //__EXO2_H
