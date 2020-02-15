/*
	Canvas pour algorithmes de jeux à 2 joueurs
	
	joueur 0 : humain
	joueur 1 : ordinateur
			
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <float.h>

// Paramètres du jeu
#define LARGEUR_MAX 9 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 1		// temps de calcul pour un coup avec MCTS (en secondes)


// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))


// D'après Wikipédia puissance 4 est joué avec 6 lignes et 7 colonnes
#define LIGNE 6
#define COL 7

#define HUMAIN 0
#define ORDI 1


// Taille d'un tableau
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

// Constante C
#define C sqrt(2)

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

    int joueur; // à qui de jouer ?

    // Plateau puissance 4
    char plateau[LIGNE][COL];

} Etat;

// Definition du type Coup
typedef struct {

    // Choix de la colonne, la gravité determinera la ligne
    int colonne;

} Coup;



// Copier un état 
Etat * copieEtat( Etat * src ) {
    Etat * etat = (Etat *)malloc(sizeof(Etat));

    etat->joueur = src->joueur;

    // Sizeof

    /* par exemple : */
    // Remplacement des trois par la taille du puissance 4
    int i,j;
    for (i=0; i< LEN(etat->plateau); i++)
    for ( j=0; j<LEN(etat->plateau[0]); j++)
    etat->plateau[i][j] = src->plateau[i][j];



    return etat;
}

// Etat initial 
Etat * etat_initial( void ) {
    Etat * etat = (Etat *)malloc(sizeof(Etat));


    /* par exemple : */
    // remplacement des trois par la taille du tableau
    int i,j;
    for (i=0; i< LEN(etat->plateau); i++)
    for ( j=0; j<LEN(etat->plateau[0]); j++)
    etat->plateau[i][j] = ' ';

    return etat;
}


void afficheJeu(Etat * etat) {

    /* par exemple : */
    int i,j;
    printf("   |");
    for ( j = 0; j < LEN(etat->plateau[0]); j++)
    printf(" %d |", j);
    printf("\n");
    printf("---------------------------------");
    printf("\n");

    for(i=0; i < LEN(etat->plateau); i++) {
        printf(" %d |", i);
        for ( j = 0; j < LEN(etat->plateau[0]); j++)
        printf(" %c |", etat->plateau[i][j]);
        printf("\n");
        printf("---------------------------------");
        printf("\n");
    }
}


// Nouveau coup
Coup * nouveauCoup(int j ) {
    Coup * coup = (Coup *)malloc(sizeof(Coup));

    // Un coup n'a qu'une colonne, à la difference du morpion

    /* par exemple : */
    //coup->ligne = i;
    coup->colonne = j;

    return coup;
}

// Demander à l'humain quel coup jouer 
Coup * demanderCoup () {

    /* par exemple : */
    int j=-1;
    /*
    printf("\n quelle ligne ? ") ;
    scanf("%d",&i);
    */

    // Lecture clavier de la colonne a jouer

    // On vérifie que le coup est dans la zone de jeu
    while(j < 0 || j >= COL) {
        printf(" quelle colonne ? ");
        scanf("%d", &j);
    }

    return nouveauCoup(j);
}

// Modifier l'état en jouant un coup 
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {


    // variable pour trouver la prochaine ligne dispo
    int ligneDispo=-1;

    // Compteur de ligne, on commence tout en bas du tableau
    int compteur = LEN(etat->plateau) -1;


    // Si on a pas trouve la ligne et qu'on est pas arrive tout en haut du tableau
    while(ligneDispo== -1 && compteur >=0 ){

        // On vérifie que la ligne est libre
        if (etat->plateau[compteur][coup->colonne]== ' ')
            ligneDispo = compteur;

            // Sinon on remonte d'une ligne
        else
            compteur--;
    }

    // si toutes les lignes sont pleines
    if(ligneDispo == -1)
        return 0; // On retourne que le coups est impossible

    // Sinon on met le bon signe pour le joueur
    etat->plateau[ligneDispo][coup->colonne] = etat->joueur ? 'O' : 'X';

    // à l'autre joueur de jouer
    etat->joueur = AUTRE_JOUEUR(etat->joueur);

    return 1;


}

// Retourne une liste de coups possibles à partir d'un etat 
// (tableau de pointeurs de coups se terminant par NULL)
Coup ** coups_possibles( Etat * etat ) {

    Coup ** coups = (Coup **) malloc((1+LARGEUR_MAX) * sizeof(Coup *) );

    int k = 0;


    /* par exemple */
    int i;
    // Parcours toutes les colonnes et regarde quelles lignes n'est pas pleines
    for(i=0; i < LEN(etat->plateau[0]); i++) {
        if ( etat->plateau[0][i] == ' ' ) {
            coups[k] = nouveauCoup(i);
            k++;
        }
    }
    /* fin de l'exemple */

    coups[k] = NULL;

    return coups;
}


// Definition du type Noeud 
typedef struct NoeudSt {

    int joueur; // joueur qui a joué pour arriver ici
    Coup * coup;   // coup joué par ce joueur pour arriver ici

    Etat * etat; // etat du jeu

    struct NoeudSt * parent;
    struct NoeudSt * enfants[LARGEUR_MAX]; // liste d'enfants : chaque enfant correspond à un coup possible
    int nb_enfants;	// nb d'enfants présents dans la liste

    // POUR MCTS:
    int nb_victoires;
    int nb_simus;

    // Récompense
    double R;

} Noeud;


Noeud *selectionneNoeud(Noeud *pSt);

Noeud *developperNoeud(Noeud *pSt);

FinDePartie simulation(Etat *pSt);

void retropropagation(Noeud *pSt, FinDePartie result);

double bValeur(Noeud *pSt);
Noeud *trouverNoeud(Noeud *pSt,bool max);

int nombre_coups_possible(Etat *pSt);

Coup ** decalageTab(Coup **coups,int indice);

// Créer un nouveau noeud en jouant un coup à partir d'un parent
// utiliser nouveauNoeud(NULL, NULL) pour créer la racine
Noeud * nouveauNoeud (Noeud * parent, Coup * coup ) {

    Noeud * noeud = (Noeud *)malloc(sizeof(Noeud));

    if ( parent != NULL && coup != NULL ) {
        noeud->etat = copieEtat ( parent->etat );
        jouerCoup ( noeud->etat, coup );
        noeud->coup = coup;
        noeud->joueur = AUTRE_JOUEUR(parent->joueur);
    }
    else {
        noeud->etat = NULL;
        noeud->coup = NULL;
        noeud->joueur = 0;
    }
    noeud->parent = parent;
    noeud->nb_enfants = 0;

    // POUR MCTS:
    noeud->nb_victoires = 0;
    noeud->nb_simus = 0;

    // On initialise les recompense a 0
    noeud->R=0;


    return noeud;
}

// Ajouter un enfant à un parent en jouant un coup
// retourne le pointeur sur l'enfant ajouté
Noeud * ajouterEnfant(Noeud * parent, Coup * coup) {
    Noeud * enfant = nouveauNoeud (parent, coup ) ;
    parent->enfants[parent->nb_enfants] = enfant;
    parent->nb_enfants++;
    return enfant;
}

void freeNoeud ( Noeud * noeud) {
    if ( noeud->etat != NULL)
        free (noeud->etat);

    while ( noeud->nb_enfants > 0 ) {
        freeNoeud(noeud->enfants[noeud->nb_enfants-1]);
        noeud->nb_enfants --;
    }
    if ( noeud->coup != NULL)
        free(noeud->coup);

    free(noeud);
}


// Test si l'état est un état terminal 
// et retourne NON, MATCHNUL, ORDI_GAGNE ou HUMAIN_GAGNE
FinDePartie testFin( Etat * etat ) {

    /* par exemple	*/

    // ICI K vaut 4 car il faut aligner 4 jetons pour gagner

    // tester si un joueur a gagné
    int i,j,k,n = 0;

    // On parcourt tout le tableau
    for ( i=0;i < LEN(etat->plateau); i++) {
        for(j=0; j < LEN(etat->plateau[0]); j++) {
            if ( etat->plateau[i][j] != ' ') {
                n++;	// nb coups joués

                // lignes
                k=0;
                while ( k < 4 && i+k < LEN(etat->plateau) && etat->plateau[i+k][j] == etat->plateau[i][j] )
                k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // colonnes
                k=0;
                while ( k < 4 && j+k < LEN(etat->plateau[0]) && etat->plateau[i][j+k] == etat->plateau[i][j] )
                k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                // diagonales
                k=0;
                while ( k < 4 && i+k < LEN(etat->plateau) && j+k < LEN(etat->plateau[0]) && etat->plateau[i+k][j+k] == etat->plateau[i][j] )
                k++;
                if ( k == 4)
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;

                k=0;
                while ( k < 4 && i+k < LEN(etat->plateau) && j-k >= 0 && etat->plateau[i+k][j-k] == etat->plateau[i][j] )
                k++;
                if ( k == 4 )
                    return etat->plateau[i][j] == 'O'? ORDI_GAGNE : HUMAIN_GAGNE;
            }
        }
    }

    // et sinon tester le match nul
    if ( n == LEN(etat->plateau)*LEN(etat->plateau[0]) )
    return MATCHNUL;

    return NON;
}
/*
 * Sélectionne récursivement à partir de la racine le nœud avec la plus grande B-valeur
 * jusqu’à arriver à un nœud terminal
 * ou un dont tous les fils n’ont pas été développés
 */
Noeud *selectionneNoeud(Noeud *pSt) {

    Noeud * noeudRecursif = pSt;


    // Si le nœud est terminal ou tous les fils n’ont pas été développés
    if(testFin(noeudRecursif->etat) != NON ||noeudRecursif->nb_enfants != nombre_coups_possible(noeudRecursif->etat)){
        return noeudRecursif;
    }
        // Sinon on choisi le noeud avec la plus grande B-valeur
    else{

        // On initialise les valeurs max avec les premiers enfants
        Noeud * noeudMax = noeudRecursif->enfants[0];
        double bValeurMax = bValeur(noeudMax);


        // Variable temporaire
        double varBvaleur;
        Noeud * noeud;
        int i;
        // Boucle pour parcourire tous les enfants
        for(i =1;i<noeudRecursif->nb_enfants;i++){
            // On récupere le noeud
            noeud = noeudRecursif->enfants[i];
            // la bvaleur
            varBvaleur = bValeur(noeud);

            // Si la nouvelle bValeur est plus grande, on change les max
            if(max(varBvaleur,bValeurMax) == varBvaleur){
                noeudMax=noeud;
                bValeurMax=varBvaleur;
            }
        }

        // On continue jusqu'a trouver un cas terminal
        return selectionneNoeud(noeudMax);
    }
}

// Compte le nombre de coups possible pour un etat
int nombre_coups_possible(Etat *pSt) {

    int compteur = 0;

    for(int i=0; i < LEN(pSt->plateau[0]); i++)

    // si il y a encore de la place, on augmente le compteur
    if ( pSt->plateau[0][i] == ' ' )
        compteur++;

    return compteur;
}

// Calcule la b-Valeur pour un noeud
double bValeur(Noeud *pSt) {


    // si le noeud n'a pas encore de simulation
    if (pSt->nb_simus==0 )
        return DBL_MAX;

    // moyenne de la récompense r(s) sur l’ensemble des simulations s passant par i
    double moyenne;

    // + si parent(i) est un nœud Max, − si Min
    if (pSt->parent->joueur == ORDI)
        moyenne= -1*((double)pSt->R/pSt->nb_simus);
    else
        moyenne = (double)pSt->R/pSt->nb_simus;

    return moyenne+C*sqrt(log(pSt->parent->nb_simus)/pSt->nb_simus);

}

/*
 * Développer un fils choisi aléatoirement parmi les fils non développés
 */
Noeud *developperNoeud(Noeud *pSt) {

    /*
     * Expansion: si cette feuille n'est pas finale,
     * créer un enfant (ou plusieurs) en utilisant les règles du jeu
     * et choisir l'un des enfants.
     *
     * WIKIPEDIA
     */


    // Si la feuille est finale
    if (testFin(pSt->etat) != NON)
        return pSt;

    // On récupert la liste des fils possibles
    Coup ** coups = coups_possibles(pSt->etat);



    // On parcourt la liste et on supprime tous les noeuds deja developpés
    int k=0;
    while(k<LEN(coups)) {
        int i;
        for (i = 0 ; i < pSt->nb_enfants ; i++) {
            // Si le noeud est deja developpé, on le supprime et on decale le tableau
            if (coups[k]->colonne == pSt->enfants[i]->coup->colonne) {
                coups=decalageTab(coups,k);
                i=i-1;
            }
        }

        k++;

    }

    // On choisit un coup au hasard
    int random = rand() % k;

    // Si le coup est NULL, on tire un nouveau noeud
    while(coups[random] == NULL)
        random = rand() % k;

    // Si le noeud est correct, on ajoute un nouveau noeud
    Noeud * noeud = ajouterEnfant(pSt,coups[random]);

    // On libere la liste
    free(coups);

    // On retourne le noeud qu'on a choisit de developper
    return noeud;
}

// Decalage de droite vers la gauche
Coup ** decalageTab(Coup **coups,int indice) {


    int i = indice;
    while (coups[i] != NULL) {
        coups[i] = coups[i+1];
        i++;
    }

    return coups;

}


/*
 *  Simuler la fin de la partie avec une marche aléatoire
 */
FinDePartie simulation(Etat *pSt) {

    // TODO CHOISIR COUP GAGNANT
    /*
     * Simulation: simuler une exécution d'une partie au hasard depuis cet enfant,
     * jusqu'à atteindre une configuration finale
     *
     * WIKIPEDIA
     */

    // Tant que la partie n'est pas termine, on continue la simulation
    while (testFin(pSt) == NON) {

        // On recupere la liste des coups possible
        Coup** listeCoups = coups_possibles(pSt);


        // Le coup a jouer
        Coup* nouveauCoup = NULL;


        int k = nombre_coups_possible(pSt);

        nouveauCoup = listeCoups[rand() % k];

        jouerCoup(pSt, nouveauCoup);


        free(listeCoups);
    }

    return testFin(pSt);
}

/*
 * Mettre à jour les B-valeurs de tous les nœuds sur le chemin de la racine au nœud terminal
 * en remontant la récompense r de la position finale
 */
void retropropagation(Noeud *pSt, FinDePartie result) {


    // On remonte l'arbre pour mettre a jour les noeuds
    while (pSt != NULL) {

        // On augment le nombre de simulation du noeud
        pSt->nb_simus++;


        // On regarde qui gagne la partie
        switch(result) {

            // Si l'ordi gagne alors la récompense alors on augmente la récompense
            case ORDI_GAGNE :
                pSt->nb_victoires++;
                pSt->R += 1;
                break;

                // Si l'humain gagne ou match nul, on n'augmente pas la récompense
            default:
                break;
        }

        // On remonte l'arbre
        pSt = pSt->parent;
    }
}



// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax, bool estMax) {

    clock_t tic, toc;
    tic = clock();
    int temps;

    Coup ** coups;
    Coup * meilleur_coup ;

    // Créer l'arbre de recherche
    Noeud * racine = nouveauNoeud(NULL, NULL);
    racine->etat = copieEtat(etat);

    // créer les premiers noeuds:
    coups = coups_possibles(racine->etat);
    int k = 0;
    Noeud * enfant;
    while ( coups[k] != NULL) {
        enfant = ajouterEnfant(racine, coups[k]);
        k++;
    }


    //meilleur_coup = coups[ rand()%k ]; // choix aléatoire

    /*
        - supprimer la sélection aléatoire du meilleur coup ci-dessus
        - implémenter l'algorithme MCTS-UCT pour déterminer le meilleur coup ci-dessous
     */

    int iter = 0;

    do {
        // à compléter par l'algorithme MCTS-UCT...


        // ALGO MCTS-UCT

        // On choisit le noeud
        Noeud * noeud = selectionneNoeud(racine);

        // Expansion du noeud
        enfant = developperNoeud(noeud);


        // Simule l'execution d'une partie
        // on copie l'etat
        Etat * nouvelleEtat = copieEtat(enfant->etat);

        FinDePartie result = simulation(nouvelleEtat);


        // Rétropropagation (Backpropagation)
        retropropagation(enfant,result);


        toc = clock();
        temps = (int)( ((double) (toc - tic)) / CLOCKS_PER_SEC );
        iter ++;
    } while ( temps < tempsmax);

    Noeud * noeud_coup = trouverNoeud(racine,estMax);
    meilleur_coup = noeud_coup->coup;
    // Jouer le meilleur premier coup
    jouerCoup(etat, meilleur_coup );

    printf("Probabilité de victoire de l'ordinateur : %.2f \n",(double)noeud_coup->nb_victoires/noeud_coup->nb_simus * 100);
    printf("Nombre de simulations réalisées : %d \n",racine->nb_simus);

    // Penser à libérer la mémoire :
    freeNoeud(racine);
    free (coups);
}

Noeud * trouverNoeud(Noeud *pSt,bool max) {


    // TODO ROBUSTE NE FONCTIONNE PAS

    Noeud*noeudMeilleurCoup = NULL;


    // Si la regle est ROBUSTE
    if (!max) {

        // soit le nombre de simulations N(i) (règle "robuste")

        // On initialise le premier noeud max
        int maxSimulation = noeudMeilleurCoup->nb_simus;

        noeudMeilleurCoup = pSt->enfants[0];

        // On parcourt tous les noeuds
        for (int i = 1; i < pSt->nb_enfants; i++) {

            // On regarde le nombre de simulation
            if (max(maxSimulation,pSt->enfants[i]->nb_simus) == pSt->enfants[i]->nb_simus) {

                // Si le nombre de simulation est plus grand, alors on met a jour le max
                noeudMeilleurCoup = pSt->enfants[i];
                maxSimulation = noeudMeilleurCoup->nb_simus;
            }
        }
    }

        // Si la regle est MAX
    else{

        // soit la récompense attendue μ(i) (règle "max")


        double maxRatio;

        noeudMeilleurCoup = pSt->enfants[0];


        // On initialise les premiers max, avec le premie enfant
        if (noeudMeilleurCoup->nb_simus ==0) {
            maxRatio = 0;
        }
        else {
            maxRatio = (double)noeudMeilleurCoup->R / noeudMeilleurCoup->nb_simus;
        }


        // On parcourt tous les enfants
        for (int i = 1 ; i < pSt->nb_enfants ; i++) {

            double noeudCalcule;

            // Si il n'y a pas encore eu de simulation sur le noeud
            if (pSt->enfants[i]->nb_simus == 0)
                noeudCalcule = 0;

                // Sinon on calcule le ration récompense, nombre de simulation
            else
                noeudCalcule = (double)pSt->enfants[i]->R / pSt->enfants[i]->nb_simus;

            // S'il faut mettre a jour les max
            if (maxRatio < noeudCalcule) {

                noeudMeilleurCoup = pSt->enfants[i];
                maxRatio = noeudCalcule;
            }
        }
    }

    return noeudMeilleurCoup;
}


int main(void) {

    Coup * coup;
    FinDePartie fin;

    // initialisation
    Etat * etat = etat_initial();

    // Choisir qui commence :
    printf("Qui commence (0 : humain, 1 : ordinateur) ? ");
    scanf("%d", &(etat->joueur) );

    // boucle de jeu
    do {
        printf("\n");
        afficheJeu(etat);

        if ( etat->joueur == 0 ) {
            // tour de l'humain

            do {
                coup = demanderCoup();
            } while ( !jouerCoup(etat, coup) );

        }
        else {
            // tour de l'Ordinateur

            ordijoue_mcts( etat, TEMPS,true );


        }

        fin = testFin( etat );
    }	while ( fin == NON ) ;

    printf("\n");
    afficheJeu(etat);

    if ( fin == ORDI_GAGNE )
        printf( "** L'ordinateur a gagné **\n");
    else if ( fin == MATCHNUL )
        printf(" Match nul !  \n");
    else
        printf( "** BRAVO, l'ordinateur a perdu  **\n");
    return 0;
}
