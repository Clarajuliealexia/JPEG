# Ensimag 1A 2018-2019 - Projet C : décodeur JPEG

Bienvenue sur le dépôt du projet C de Clara Bourgoin, Afnane Kotbi et Carla Puech.
Vous y trouverez:

- tout le contenu des sources (dont les modules fournis initialement qui ont été réécrit) qui compilent correctement et décodant le plus grand nombre d'image jpeg fournis aux départ dans le répertoire src.
- toutes les déclarations des prototypes de nos fonctions respectives dans le repertoire include. (e.g. outils.h pour les fonctions dans ./src/outils.c)
- le repertoire obj où se stockent les executables après avoir lancé la commande make.
- le repertoire bin où se stocke l'executable de jpeg2ppm après avoir lancé la commande make.
- notre Makefile et un Makefile_progressif pour la version progressive (à renommer pour tester)
- le répertoire images contient : 
    - un répertoire Essais qui contient plein d'images farfelues illustrant notre progression et nos problèmes rencontrés lors de ce projet.
    - un répertoire Images_progressives contenant toutes les images jpeg fournies au départ mais en version progressive (e.g. invader.jpeg devient invader_prog.jpeg).
    - un répertoire Test_echantillonnage contenant des images échantillonnées pour tester plus de cas que les cas basiques d'échantillonnage. (les 6 derniers chiffres de leur nom correspond aux facteurs hi*vi pour les composantes Y, Cb et Cr).

## Implémentations réalisées

Le sommaire d'un parcours présente la liste des différents types d'image jpeg que notre jpeg2ppm décode.

<details>
<summary> Cliquez ici pour dévoiler en détail tout les différents types d'image jpeg que notre jpeg2ppm décode. </summary>

1. Niveaux de gris (1 composante)
1. Taille 8 × 8 (un seul bloc)
1. Taille supérieure à un bloc
1. Troncature à droite
1. Troncature à gauche
1. Couleur (3 composantes YCbCr)
1. Troncature à droite et à gauche en même temps
1. Échantillonnage vertical
1. Échantillonnage horizontal
1. Échantillonnage vertical et horizontal en même temps
1. de trés grandes images
1. des échantillonnages moins courants (e.g. 2*1,2*1,1*1 ou 4*1,2*1,4*1 ou 2*2,2*2,2*1) 


</details>

Cependant nous n'avons pas pu réaliser entièrement la fonction permettant de décoder les images au format progressif (marche pour invader_prog.jpeg et la première étape pour toutes les images en nuances de gris puis fait une segmentation fault)

 
# Authors
* **Clara Bourgoin**
* **Afnane Kotbi**
* **Carla Puech**
