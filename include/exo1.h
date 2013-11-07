/**
 * TP4 - Exercice 1 <Header>
 * \author SCHMITT Maxime, SCHIMCHOWITSCH Raphaël
 */
#ifndef __EXO_1_H
#define __EXO_1_H

/**
 */

int randouze(unsigned int *seed);

static inline void erreur(const char* err);

void *producteur(void *arg);

void *consommateur(void *arg);

void ctrlC(int signum);

void ajoutbuff(int nbr);

int retirerbuff();

int main(int argc, char **argv);

#endif // _ EXO_1_H
