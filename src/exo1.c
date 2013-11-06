/**
 * TP4 - Exercice 1 <Code source>
 * \author SCHMITT Maxime, SCHIMCHOWITSCH Raphaël
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>

#define tailleTampax 10

struct nblu_somme{
	unsigned int nblu;
	unsigned long int total;
};

long int tampax[tailleTampax];
int fini=1;
sem_t lecture, ecriture;

int randouze(unsigned int *seed){
	return (int) ((((float)rand_r(seed))/RAND_MAX)*255);
}


void *producteur(void *arg){
	struct nblu_somme *lu;
	lu=(struct nblu_somme*) malloc(sizeof(struct nblu_somme));
	if(lu==NULL)
		erreur("malloc");
	pthread_barrier_t *startingBlock= (pthread_barrier_t*) arg;
	pthread_barrier_wait(startingBlock);
	int seed=(unsigned int) pthread_self();
	while(!fini){
		p(mut)
		ajoutTampax(randouze(&seed));
		v(mut)
	}
	return NULL;
}

void *consommateur(void *arg){
	struct nblu_somme *lu;
	lu=(struct nblu_somme*) malloc(sizeof(struct nblu_somme));
	if(lu==NULL)
		erreur("malloc");
	pthread_barrier_t *startingBlock= (pthread_barrier_t*) arg;
	pthread_barrier_wait(startingBlock);
	return lu;
}

void ctrlC(int signum){
	fini=0;
	ajoutTampax(-1);
}

void ajoutTampax(int nbr){
	
}

int retirerTampax(){
}

int main (int argc, char ** argv){
	int nbprod=0, nbconso=0;
	unsigned int prodAproduit=0, consoAmanger=0;
	unsigned long int sommeProd=0, sommeMiam=0; 
	int i, retourFonction;
	struct nblu_somme *lu;
	pthread_t *prod, *conso;
	pthread_barrier_t startingBlock;
	
	if(argc != 3){
		fprintf(stderr,"Utilisation : %s nbr_producteurs nbr_consommateurs\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	nbprod=atoi(argv[1]);
	nbconso=atoi(argv[2]);
	
	retourFonction=pthread_barrier_init(&startingBlock, NULL, 1+nbprod+nbconso);
	if(retourFonction!=0){
		perror("pthread_barrier_init");
		exit(EXIT_FAILURE);
	}
	
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
		if(retourFonction!=0){
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}
	for(i=0; i<nbconso; i++){
		retourFonction=pthread_create(&conso[i], NULL, consommateur, &startingBlock);
		if(retourFonction!=0){
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}
	pthread_barrier_wait(&startingBlock);
	
	for(i=0; i<nbconso; i++){
		retourFonction=pthread_join(prod[i], (void**) &lu);
		if(retourFonction!=0){
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
		consoAmanger+=lu->nblu;
		sommeMiam+=lu->total;
		free(lu);
	}
	
	for(i=0; i<nbprod; i++){
		retourFonction=pthread_join(conso[i], (void**) &lu);
		if(retourFonction!=0){
			perror("pthread_join");
			exit(EXIT_FAILURE);
		}
		prodAproduit+=lu->nblu;
		sommeProd+=lu->total;
		free(lu);
	}
	
	printf("Les %d producteurs ont produit %d\
 valeurs pour une somme de %ld.\n\
Les %d consommateurs ont consommés %d\
 valeurs pour une somme de %ld.\n",
	 nbprod, prodAproduit, sommeProd,
	 nbconso, consoAmanger, sommeMiam);
	 
	return 0;
}
