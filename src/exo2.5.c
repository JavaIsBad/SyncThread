/**
 * TP4 - Exercice 1 <Code source>
 * \author SCHMITT Maxime, SCHIMCHOWITSCH Raphaël
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include "exo2.h"
#include "exo1.h"

#define VRAI 0==0
#define FAUX !VRAI

#define taillebuff 10

struct nblu_somme{
    unsigned int nblu;
    unsigned long int total;
};

long int buff[taillebuff];
unsigned int debutBuff, finBuff;
int fini=FAUX;
monsem_t lecture, ecriture, AccesDenied, AccesDenied2;

int randouze(unsigned int *seed){
    return (int) ((((float)rand_r(seed))/RAND_MAX)*255);
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
    pthread_barrier_t *startingBlock= (pthread_barrier_t*) arg;
    pthread_barrier_wait(startingBlock);
    int seed=(unsigned int) pthread_self();
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
    pthread_barrier_t *startingBlock= (pthread_barrier_t*) arg;
    pthread_barrier_wait(startingBlock);
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
    int fin;
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

void setHandler(void (*handler)(int)){
    struct sigaction s;
    int retfunct;
    s.sa_handler=handler;
    s.sa_flags=0;
    retfunct=sigfillset(&s.sa_mask);
    if(retfunct !=0)
        erreur("sigfillset");
    retfunct=sigaction(SIGINT, &s, NULL);
    if(retfunct !=0)
        erreur("sigaction");
}

int main (int argc, char ** argv){
    int nbprod=0, nbconso=0;
    unsigned int prodAproduit=0, consoAmanger=0;
    unsigned long int sommeProd=0, sommeMiam=0; 
    int i, retourFonction;
    struct nblu_somme *lu;
    pthread_t *prod, *conso;
    pthread_barrier_t startingBlock;

    setHandler(ctrlC);

    if(argc != 3){
        fprintf(stderr,"Utilisation : %s nbr_producteurs nbr_consommateurs\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    nbprod=atoi(argv[1]);
    nbconso=atoi(argv[2]);

    monsem_init(&AccesDenied, 1);
    monsem_init(&AccesDenied2, 1);
    monsem_init(&lecture, 0);
    monsem_init(&ecriture, taillebuff);

    retourFonction=pthread_barrier_init(&startingBlock, NULL, 1+nbprod+nbconso);
    if(retourFonction!=0)
        erreur("pthread_barrier_init");

    prod=(pthread_t *) malloc(nbprod*sizeof(pthread_t));
    conso=(pthread_t *) malloc(nbconso*sizeof(pthread_t));
    if(prod==NULL || conso==NULL){
        perror("Malloc");
        if(prod!=NULL)
            free(prod);
        if(conso!=NULL)
            free(conso);
        exit(EXIT_FAILURE);
    }

    for(i=0; i<nbprod; i++){
        retourFonction=pthread_create(&prod[i], NULL, producteur, &startingBlock);
        if(retourFonction!=0)
            erreur("pthread_create");
    }

    for(i=0; i<nbconso; i++){
        retourFonction=pthread_create(&conso[i], NULL, consommateur, &startingBlock);
        if(retourFonction!=0)
            erreur("pthread_create");
    }

    pthread_barrier_wait(&startingBlock);


    for(i=0; i<nbprod; i++){
        retourFonction=pthread_join(prod[i], (void**) &lu);
        if(retourFonction!=0)
            erreur("pthread_join");
        prodAproduit+=lu->nblu;
        sommeProd+=lu->total;
        free(lu);
    }

    printf("\nFin prod\n");
    free(prod);

    ajoutbuff(-1);

    for(i=0; i<nbconso; i++){
        retourFonction=pthread_join(conso[i], (void**) &lu);
        if(retourFonction!=0)
            erreur("pthread_join");
        consoAmanger+=lu->nblu;
        sommeMiam+=lu->total;
        free(lu);
    }

    printf("Fin conso\n");
    free(conso);

    printf("Les %d producteurs ont produit %d\
            valeurs pour une somme de %ld.\n\
            Les %d consommateurs ont consommés %d\
            valeurs pour une somme de %ld.\n",
            nbprod, prodAproduit, sommeProd,
            nbconso, consoAmanger, sommeMiam);

    return 0;
}
