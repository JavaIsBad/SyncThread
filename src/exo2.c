/**
 * \file exo2.c
 * TP4 - Exercice 2 Code source
 * \author SCHMITT Maxime, SCHIMCHOWITSCH Raphaël
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include "exo1.h"
#include "monsem.h"

#ifdef VRAI
#undef VRAI
#endif
#define VRAI 0==0
#ifdef FAUX
#undef FAUX
#endif
#define FAUX !VRAI

#define taillebuff 10

long int buff[taillebuff];
unsigned int debutBuff, finBuff;
int fini=FAUX;
monsem_t lecture, ecriture, AccesDenied, AccesDenied2;

int randouze(unsigned int *seed){
    return (int) (((float)rand_r(seed)/RAND_MAX)*255);
}

static inline void erreur(const char* err){
    perror(err);
    exit(EXIT_FAILURE);
}

void *producteur(void *arg){
    int ajout;
    struct nblu_somme *lu;
    lu=(struct nblu_somme*) malloc(sizeof(struct nblu_somme));
    if(lu==NULL)
        erreur("malloc");
    unsigned int seed=(unsigned int) pthread_self();
    lu->nblu=0;
    lu->total=0;
    while(!fini){
        ajout=randouze(&seed);
        ajoutbuff(ajout);
        lu->total+=ajout;
        lu->nblu+=1;
    }
    return lu;
}

void *consommateur(void *arg){
    int nblu;
    struct nblu_somme *lu;
    lu=(struct nblu_somme*) malloc(sizeof(struct nblu_somme));
    if(lu==NULL)
        erreur("malloc");
    lu->nblu=0;
    lu->total=0;
    while(1){
        nblu=retirerbuff();
        if(nblu==-1){
            break;
        }
        lu->nblu++;
        lu->total+=nblu;
    }
    return lu;
}

void ctrlC(int signum){
    fini=VRAI;
}

void ajoutbuff(int nbr){
    monsem_wait(&ecriture);
    monsem_wait(&AccesDenied);
    buff[finBuff]=nbr;
    finBuff=(finBuff+1)%taillebuff;
    monsem_post(&AccesDenied);
    monsem_post(&lecture);
}

int retirerbuff(){
    int nbr;
    monsem_wait(&lecture);
    monsem_wait(&AccesDenied2);
    nbr=buff[debutBuff];
    if(nbr!=-1)
        debutBuff=(debutBuff+1)%taillebuff;
    else
        monsem_post(&lecture);
    monsem_post(&AccesDenied2);
    monsem_post(&ecriture);
    return nbr;
}

int setHandler(void (*handler)(int), int signum){
    struct sigaction s;
    int retfunct;
    s.sa_handler=handler;
    s.sa_flags=0;
    retfunct=sigfillset(&s.sa_mask);
    if(retfunct==0)
        retfunct=sigaction(signum, &s, NULL);
    return retfunct;
}

int main (int argc, char ** argv){
    int nbprod=0, nbconso=0;
    unsigned int prodAproduit=0, consoAmanger=0;
    unsigned long int sommeProd=0, sommeMiam=0; 
    int i, retval;
    struct nblu_somme *lu;
    pthread_t *prod, *conso;

    retval=setHandler(ctrlC, SIGINT);
    if(retval!=0)
        erreur("Impossible de creer le handler pour ^C");

    if(argc != 3){
        fprintf(stderr,"Utilisation : %s nbr_producteurs nbr_consommateurs\n0<=nbr_producteurs et 1<=nbr_consommateurs",argv[0]);
        exit(EXIT_FAILURE);
    }
    nbprod=atoi(argv[1]);
    nbconso=atoi(argv[2]);
    if(nbconso==0 || nbconso<0 || nbprod<0){
        errno=EINVAL;
        erreur("Le nombre de consommateurs doit etre d'au moins 1 et il doit y avoir au minimum 0 producteurs");
    }

    retval=monsem_init(&AccesDenied, 1);
    retval+=monsem_init(&AccesDenied2, 1);
    retval+=monsem_init(&lecture, 0);
    retval+=monsem_init(&ecriture, taillebuff);
    if(retval!=0)
        erreur("monsem_init");

    prod=(pthread_t *) malloc(nbprod*sizeof(pthread_t));
    conso=(pthread_t *) malloc(nbconso*sizeof(pthread_t));
    if(prod==NULL || conso==NULL){
        perror("malloc");
        if(prod!=NULL)
            free(prod);
        if(conso!=NULL)
            free(conso);
        exit(EXIT_FAILURE);
    }

    // Creation des producteurs

    for(i=0; i<nbprod; i++){
        retval=pthread_create(&prod[i], NULL, producteur, NULL);
        if(retval!=0)
            erreur("pthread_create");
    }

    // Creation des consommateurs

    for(i=0; i<nbconso; i++){
        retval=pthread_create(&conso[i], NULL, consommateur, NULL);
        if(retval!=0)
            erreur("pthread_create");
    }

    // Attente de la fin des producteurs

    for(i=0; i<nbprod; i++){
        retval=pthread_join(prod[i], (void**) &lu);
        if(retval!=0)
            erreur("pthread_join");
        prodAproduit+=lu->nblu;
        sommeProd+=lu->total;
        free(lu);
    }

    printf("\nFin prod\n");
    free(prod);

    // Les producteurs ont fini d ajouter la derniere valeur si le ctrlC survient pendant un ajout

    // Ici le thread principale joue le role de producteur pour ajouter la valeur d arret des consommateurs
    ajoutbuff(-1);

    for(i=0; i<nbconso; i++){
        retval=pthread_join(conso[i], (void**) &lu);
        if(retval!=0)
            erreur("pthread_join");
        consoAmanger+=lu->nblu;
        sommeMiam+=lu->total;
        free(lu);
    }

    printf("Fin conso\n");
    free(conso);

    retval=monsem_destroy(&AccesDenied);
    retval+=monsem_destroy(&AccesDenied2);
    retval+=monsem_destroy(&lecture);
    retval+=monsem_destroy(&ecriture);
    if(retval!=0)
        erreur("monsem_destroy");

    printf("Les %d producteurs ont produit %d\
            valeurs pour une somme de %ld.\n\
            Les %d consommateurs ont consommés %d\
            valeurs pour une somme de %ld.\n",
            nbprod, prodAproduit, sommeProd,
            nbconso, consoAmanger, sommeMiam);

    return 0;
}
