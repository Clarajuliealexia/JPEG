#include "../include/huffman.h"

struct node *init_noeud(int8_t valeur){
  struct node *noeud;
  noeud = malloc(sizeof(struct node));
  noeud->left = NULL;
  noeud->right = NULL;
  noeud->valeur = valeur;
  noeud-> plein = false;
  return noeud;
}

void ajouter_noeud(struct node *noeud, uint8_t hauteur, uint8_t valeur){
  /*Cette fonction permet de créer des noeuds dans la table de huffman
  si le noeud n'est pas une feuille, sa valeur vaut -1
  sinon sa valeur est valeur
  on commence par remplir le fils gauche puis le fils droit
  la hauteur sert à savoir si on a atteint une feuille ou pas*/
    if (hauteur == 0){
      noeud->valeur = valeur;
      noeud->plein = true;
      return;
    }
    else {
      if (noeud->left != NULL){
        // le noeud gauche existe
        if (noeud->left->plein == false){
          // si le noeud gauche n'est pas plein on le rempli
          ajouter_noeud(noeud->left, hauteur - 1, valeur);
        }
        else if (noeud->right != NULL){
          // si on a un noeud droit on le rempli
          ajouter_noeud(noeud->right, hauteur -1, valeur);
          // le noeud est rempli si son noeud droit est rempli
          // le gauche est déjà forcément rempli par la condition au dessus
          noeud->plein = noeud->right->plein;
        }
        else {
          // si le noeud droit n'existe pas on le crée
          // on initialise sa valeur à -1
          noeud->right = init_noeud(-1);
          // on ajoute notre valeur
          ajouter_noeud(noeud->right, hauteur -1, valeur);
          noeud->plein = noeud->right->plein;
        }
      }
      else {
        // le noeud gauche n'existe pas on le crée et on le rempli
        // initialisation de la valeur à -1
        noeud->left = init_noeud(-1);
        ajouter_noeud(noeud->left, hauteur -1, valeur);
      }
    }
}

void creer_table(struct huff_table *table, uint8_t nb_code[16], uint8_t tab_symbole[table->nb_code]){
  int compteur = 0;
  for (uint8_t i = 0; i < 16; i++){
    uint8_t hauteur = i + 1;
    for (uint8_t j = 0; j < nb_code[i]; j++){
      ajouter_noeud(table->tete, hauteur, tab_symbole[compteur]);
      compteur += 1;
    }
  }
}


void print_arbre(struct node *noeud){
  if (noeud != NULL){
    fprintf(stderr, "%x \n", noeud->valeur);
    print_arbre(noeud->left);
    print_arbre(noeud->right);
  }
}

void print_table(struct huff_table *table){
  print_arbre(table->tete);
}


struct huff_table *load_huffman_table (struct bitstream *stream,
                                             uint16_t *nb_byte_read){
  /*on crée et on alloue la table de huffman*/
  struct huff_table *table;
  table = malloc(sizeof(struct huff_table));
  table->nb_code = 0;
  /*table de taille 16 qui donne le nombre d'éléments par hauteur*/
  uint8_t tab_nombre[16];
  for (int i = 0; i < 16; i++){
   uint32_t nombre = 0;
   read_bitstream (stream, 8, &nombre, false);
   tab_nombre[i] = (uint8_t)nombre;
   nb_byte_read += 8;
   table->nb_code += tab_nombre[i];
 }
 uint8_t tab_symbole[table->nb_code];
 for (uint8_t i = 0; i < table->nb_code; i++){
   uint32_t symbole = 0;
   read_bitstream (stream, 8, &symbole, false);
   tab_symbole[i] = (uint8_t)symbole;
   nb_byte_read += 8;
 }
 // initialisation de la tete
 // les valeurs sont initialisées à -1 si le noeud n'est pas une feuille
 table->tete = init_noeud(-1);
 creer_table(table, tab_nombre, tab_symbole);
 return table;
}


bool est_feuille(struct node *noeud){
  if (noeud == NULL){
    return true;
  }
  return (noeud->left == NULL && noeud->right == NULL);
}

int8_t decode(struct node *noeud, struct bitstream *stream){
  uint32_t lecture = 0;
  while (est_feuille(noeud) == 0){
    read_bitstream (stream, 1, &lecture, true);
    if (lecture == 0){
      noeud = noeud->left;
    }
    else {
      noeud = noeud->right;
    }
  }
  if (noeud->valeur == -1){
    fprintf(stderr, "Erreur de valeur dans huffman \n");
    return -1;
  }
  return noeud->valeur;
}


int8_t next_huffman_value (struct huff_table *table,
                                 struct bitstream *stream){
  return decode(table->tete, stream);
}


void free_noeud(struct node *noeud){
 if (noeud != NULL){
   free_noeud(noeud->left);
   free_noeud(noeud->right);
   free(noeud);
 }
}

void free_huffman_table (struct huff_table *table){
  free_noeud(table->tete);
  free(table);
}
