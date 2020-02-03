/*
	Canvas pour algorithmes de jeux à 2 joueurs
	
	joueur 0 : humain
	joueur 1 : ordinateur
			
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Paramètres du jeu
#define LARGEUR_MAX 9 		// nb max de fils pour un noeud (= nb max de coups possibles)

#define TEMPS 5		// temps de calcul pour un coup avec MCTS (en secondes)


// macros
#define AUTRE_JOUEUR(i) (1-(i))
#define min(a, b)       ((a) < (b) ? (a) : (b))
#define max(a, b)       ((a) < (b) ? (b) : (a))


// D'après Wikipédia puissance 4 est joué avec 6 lignes et 7 colonnes
#define LIGNE 6
#define COL 7

#define HUMAIN 0
#define ORDI 1


#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

// Constante C
#define C sqrt(2)

// Critères de fin de partie
typedef enum {NON, MATCHNUL, ORDI_GAGNE, HUMAIN_GAGNE } FinDePartie;

// Definition du type Etat (état/position du jeu)
typedef struct EtatSt {

	int joueur; // à qui de jouer ?

	/* par exemple, pour morpion: */
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
	printf("--------------------------------");
	printf("\n");
	
	for(i=0; i < LEN(etat->plateau); i++) {
		printf(" %d |", i);
		for ( j = 0; j < LEN(etat->plateau[0]); j++)
			printf(" %c |", etat->plateau[i][j]);
		printf("\n");
		printf("--------------------------------");
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
	int j;
	/*
	printf("\n quelle ligne ? ") ;
	scanf("%d",&i);
	*/

	// Lecture clavier de la colonne a jouer
	printf(" quelle colonne ? ") ;
	scanf("%d",&j); 
	
	return nouveauCoup(j);
}

// Modifier l'état en jouant un coup 
// retourne 0 si le coup n'est pas possible
int jouerCoup( Etat * etat, Coup * coup ) {


    // Terminé

	/* par exemple : */
	if ( coup->colonne < 0 || coup->colonne >= LEN(etat->plateau[0]) ||etat->plateau[0][coup->colonne] != ' ' )
		return 0;


	int ligneDispo=-1;
    int compteur = LEN(etat->plateau) -1;
    while(compteur >=0 && ligneDispo== -1){
        if (etat->plateau[compteur][coup->colonne]== ' ')
            ligneDispo = compteur;
        else
            compteur--;
    }

    // Toutes les lignes sont pleines
    if(ligneDispo == -1)
        return 0;

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
	double R;
	
} Noeud;


Noeud *selectionneNoeud(Noeud *pSt);

Noeud *developperNoeud(Noeud *pSt);

FinDePartie simulation(Etat *pSt);

void retropropagation(Noeud *pSt, FinDePartie result);

double bValeur(Noeud *pSt);

Coup *trouverNoeudMeilleurCoup(Noeud *pSt);

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


    // TODO NOMBRE D'iteration max et confidion LEN
    // Si le nœud est terminal ou tous les fils n’ont pas été développés
    if(testFin(noeudRecursif->etat) != NON ||noeudRecursif->nb_enfants != LEN(coups_possibles(noeudRecursif->etat))){
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

double bValeur(Noeud *pSt) {

    // + si parent(i) est un noeud max, - si parent(i) est un noeud min
    int mu=pSt->R/pSt->nb_simus;

    if(pSt->parent->joueur == ORDI)
        mu=-1*(pSt->R+pSt->nb_simus);

    // Nombre de simulation
    int nParent=pSt->parent->nb_simus;
    int n=pSt->nb_simus;



    int res = mu + C * sqrt(log(nParent)/n);
    return res;
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


    if (testFin(pSt->etat) != NON)
        return pSt;
    else{

        Coup ** coups=coups_possibles(pSt->etat);

        // On enleve tous les coups deja developpés
        //TODO
        int i;
        for(i=0;i<LEN(coups);i++){
            int j;
            for(j=0; j<pSt->nb_enfants;j++){

                // Si le coups est deja developpé, on le supprime
                if(coups[i]->colonne == pSt->enfants[j]->coup->colonne)
                    coups[i]=NULL;
            }
        }


        int random = rand() % LEN(coups);

        while(coups[random] == NULL)
            random = rand() % LEN(coups);

        return ajouterEnfant(pSt,coups[random]);
    }
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

    // Tant que la partie n'est pas terminée
    while(testFin(pSt) == NON){

        Coup** listeCoups = coups_possibles(pSt);
        Coup* coup = NULL;

        int k = 0;
        while (listeCoups[k] != NULL)
            k++;
        coup = listeCoups[rand() % k];

        jouerCoup(pSt,coup);
    }

    return testFin(pSt);
}

/*
 * Mettre à jour les B-valeurs de tous les nœuds sur le chemin de la racine au nœud terminal
 * en remontant la récompense r de la position finale
 */
void retropropagation(Noeud *pSt, FinDePartie result) {

    //TODO
    while (pSt != NULL) {
        pSt->nb_simus++;
        switch(result) {
            case ORDI_GAGNE :
                pSt->nb_victoires++;
                pSt->R += 1;
                break;
            case HUMAIN_GAGNE :
                pSt->R += 0;
                break;
            case MATCHNUL :
                pSt->R += 0.5;
                break;
            default:
                break;
        }
        pSt = pSt->parent;
    }
}



// Calcule et joue un coup de l'ordinateur avec MCTS-UCT
// en tempsmax secondes
void ordijoue_mcts(Etat * etat, int tempsmax) {

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
	
	
	meilleur_coup = coups[ rand()%k ]; // choix aléatoire

	/*  TODO :
		- supprimer la sélection aléatoire du meilleur coup ci-dessus
		- implémenter l'algorithme MCTS-UCT pour déterminer le meilleur coup ci-dessous
	 */

	int iter = 0;
	
	do {
		// à compléter par l'algorithme MCTS-UCT...

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

	meilleur_coup = trouverNoeudMeilleurCoup(racine);
	// Jouer le meilleur premier coup
	jouerCoup(etat, meilleur_coup );
	
	// Penser à libérer la mémoire :
	freeNoeud(racine);
	free (coups);
}

Coup *trouverNoeudMeilleurCoup(Noeud *pSt) {

    // TODO MODIFICATION

    Noeud * noeudMeilleurCoup = pSt->enfants[0];
    int i = 1, maxSimus;
    double maxValeurs, valeurCourante;

            maxSimus = noeudMeilleurCoup->nb_simus;

            for (i = 1 ; i < pSt->nb_enfants ; i++) {
                if (maxSimus < pSt->enfants[i]->nb_simus) {
                    noeudMeilleurCoup = pSt->enfants[i];
                    maxSimus = noeudMeilleurCoup->nb_simus;
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
			
			ordijoue_mcts( etat, TEMPS );
			
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
