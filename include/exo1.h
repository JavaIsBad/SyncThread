/**
 * TP4 - Exercice 1 <Header>
 * \author SCHMITT Maxime, SCHIMCHOWITSCH Raphaël
 */
#ifndef __EXO_1_H
#define __EXO_1_H

/**
 * \struct nblu_somme
 * \brief Une structure permettant de sauvegarder localement le nombre de valeurs lues et leurs somme
 */

struct nblu_somme{
    unsigned int nblu;          //<- Le nombre de valeurs lue dans un thread
    unsigned long int total;    //<- La somme de toutes les valeurs lues
};

/**
 * \brief Crée une valeur comprise entre zero et 255
 * \param seed La graine qui permet de creer le nombre aleatoire
 * \return Un nombre entre 0 et 255 inclus
 */

int randouze(unsigned int *seed);

/**
 * \brief Affiche le message d'erreur err et quite le programme
 * \param err Le message d'erreur a afficher
 */

static inline void erreur(const char* err);

/**
 * \brief La fonction de production, produit des valeurs dans un buffer
 * \arg arg Non utilise (faites vous plaisire)
 */

void *producteur(void *arg);

/**
 * \brief La fonction de consommateurs, consomme des valeurs dans un buffer
 * \arg arg Non utilise (faites vous plaisire)
 */

void *consommateur(void *arg);

/**
 * \brief La fonction qui sera appele en pressant ^C
 * \prarm signum Non utilise (faites vous plaisire)
 */

void ctrlC(int signum);

/**
 * \brief Ajoute dans le buffer la valeur nbr en derniere position
 * Cette fonction est bloquante si le buffer est plein
 * \param nbr Le nombre a ajouter dans le buffer
 */

void ajoutbuff(int nbr);

/**
 * \brief Retire la premiere valeur du buffer et la renvoie
 * Cette fonction est bloquante si le buffer est vide
 * Si cette fonction lit -1 elle ne la retire pas du buffer
 * \return L'entire qui vient d'être retiré du buffer
 */

int retirerbuff(void);

/**
 * \brief Affecte la fonction handler pour traiter le signal signum
 * \param handler La fonction qui sera appelee quand le signal signum sera recu
 * \param signum Le numero du signal a qui affecter la fonction
 * \return 0 si succes, autre chose sinon
 *
 * errno est positionné en cas d'erreur
 *
 * EINVAL signum n'est pas valide
 * EFAULT handler pointe vers une adresse non valide
 */

int setHandler(void (*handler)(int), int signum);

/**
 * \brief La fonction main
 * \param argc Le nombre d'arguments
 * \param argv Le tableau contenant les arguments
 * \return EXIT_SUCCESS si succes
 */

int main(int argc, char **argv);

#endif // __EXO1_H
