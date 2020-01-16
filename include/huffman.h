#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include "outils.h"
#include "bitstream.h"

struct huff_table{
  struct node *tete;
  uint8_t nb_code;
};

struct node{
  int8_t valeur;
  struct node *left;
  struct node *right;
  bool plein;
};


/*initialise un noeud
met les voisins à NULL et la valeur à valeur*/
struct node *init_noeud(int8_t valeur);

/*Cette fonction permet de créer des noeuds dans la table de huffman
si le noeud n'est pas une feuille, sa valeur vaut -1
sinon sa valeur est valeur
on commence par remplir le fils gauche puis le fils droit
la hauteur sert à savoir si on a atteint une feuille ou pas*/
void ajouter_noeud(struct node *noeud, uint8_t hauteur, uint8_t valeur);

/* Crée la table de huffman à partir des symboles présents et du nombre de
symboles par hauteur */
void creer_table(struct huff_table *table, uint8_t *nb_code, uint8_t *tab_symbole);

/* Affiche un arbre binaire en mode parcours profondeur
on aura le noeud, son fils gauche puis son fils droit*/
void print_arbre(struct node *noeud);

/* Affiche une table de Huffman : utilise la fonction précédente en partant de
la tete */
void print_table(struct huff_table *table);

/* Récupère les valeurs dans l'entete DHT pour créer une table de Huffman */
struct huff_table *load_huffman_table (struct bitstream *stream,
                                             uint16_t *nb_byte_read);

/* Répond à la question : le noeud est-t-il une feuille ?*/
bool est_feuille(struct node *noeud);

/* Lecture dans le stream pour décoder la valeur codée
on va à droite si on a un 1 et à gauche sinon
on se déplace dans l'arbre jusqu'à ce qu'on soit sur une feuille*/
int8_t decode(struct node *noeud, struct bitstream *stream);

/* décode en partant de la tete de la table de Huffman*/
int8_t next_huffman_value (struct huff_table *table,
                                 struct bitstream *stream);
/* Libère les noeuds*/
void free_noeud(struct node *noeud);

/* Libère la table de Huffman*/
void free_huffman_table (struct huff_table *table);

#endif
