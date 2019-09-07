#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ********* COSTANTI ******** */

#define NAMES_LENGTH 150
#define INPUT_BUFFER 500
#define ELEMENTS_STOCK 128
#define RELATION_STOCK 16
#define RECEIVING_ENTITY_STOCK 32
#define ORIGIN_ENTITY_STOCK 32
#define MAXIMUM_RELATION_STACK 8


/* ********* DEFINIZIONE TIPI *********** */

typedef enum {
    black, red
} colour;

typedef struct ENT {
    char entityName[NAMES_LENGTH];
    colour colour;
    struct ENT *father;
    struct ENT *leftSon;
    struct ENT *rightSon;
} entityNode;

typedef struct REL {
    char relationName[NAMES_LENGTH];
    colour colour;
    struct REC_ENT *receivingList;
    int allocatedReceiver;
    int numberOfReceiver;
    struct REL *father;
    struct REL *leftSon;
    struct REL *rightSon;
} relationNode;

/*
 * questa struttura rappresenta un entità che riceve una relazione. al suo interno è presente una lista di tutte le entità che gli donano quella relazione.
 */
typedef struct REC_ENT {
    entityNode *receiver;
    int allocatedOrigins;
    entityNode **originList;
    int receivingTimes;
} receivingNode;





/* ********** PROTOTIPI ************* */

// funzioni per le rotazioni destre e sinistre degli alberi RB
void leftRotateEntity(entityNode *rotationRoot);

void rightRotateEntity(entityNode *rotationRoot);

void leftRotateRelation(relationNode *rotationRoot);

void rightRotateRelation(relationNode *rotationRoot);

// funzioni per gestire le 5 possibili richieste del progetto
void report();

void addent(char *newEntity);

void delent(char *deletedEntity);

void addrel(char *originEntity, char *destinationEntity, char *relation);

void delrel(char *originEntity, char *destinationEntity, char *relation);

// funzioni per il fixup degli alberi RB
void insertEntityFixUp(entityNode *addedEntity);
void insertRelationFixUp(relationNode *addedRelation);

// funzione per la ricerca di un entità nell'albero
entityNode *searchEntity(char *researchedEntity);


/* ************ VARIABILI GLOBALI *********** */

entityNode *entityTree;
relationNode *relationTree;

entityNode *entityRoot;
relationNode *relationRoot;

int relationElements = 0;
int entityElements = 0;

int currentMaxEntitySize = 0;
int currentMaxRelationSize = 0;

entityNode NIL_ENT;
relationNode NIL_REL;

/* ************ MAIN ************* */
int main() {
    char inputBuffer[INPUT_BUFFER];
    char *inputToken;

    // vettori che contengono i nodi degli alberi
    entityTree = malloc(ELEMENTS_STOCK * sizeof(entityNode));
    currentMaxEntitySize = ELEMENTS_STOCK;
    relationTree = malloc(RELATION_STOCK * sizeof(relationNode));
    currentMaxRelationSize = RELATION_STOCK;

    // costruisce i nodi NIL per le relazioni e le entità
    strcpy(NIL_ENT.entityName, "NIL_ENT");
    NIL_ENT.father = &NIL_ENT;
    NIL_ENT.leftSon = &NIL_ENT;
    NIL_ENT.rightSon = &NIL_ENT;
    NIL_ENT.colour = black;

    strcpy(NIL_REL.relationName, "NIL_REL");
    NIL_REL.father = &NIL_REL;
    NIL_REL.leftSon = &NIL_REL;
    NIL_REL.rightSon = &NIL_REL;
    NIL_REL.colour = black;

    // setta la radice degli aleri
    entityRoot = &NIL_ENT;
    relationRoot = &NIL_REL;

    // raccoglie l'istruzione in input
    char* k = fgets(inputBuffer, INPUT_BUFFER, stdin);

    while (strcmp(inputBuffer, "end\n") != 0 ) {

        if (strcmp(inputBuffer, "report\n") == 0)
            report();
        else {
            inputToken = strtok(inputBuffer, " \"");

            if (strcmp(inputToken, "addent") == 0)
                addent(strtok(NULL, "\""));
            else if (strcmp(inputToken, "delent") == 0)
                delent(strtok(NULL, "\""));
            else if (strcmp(inputToken, "addrel") == 0) {
                char *originEntity = inputToken = strtok(NULL, "\" \"");
                char *destinationEntity = inputToken = strtok(NULL, "\" \"");
                char *relation = inputToken = strtok(NULL, " \"");
                addrel(originEntity, destinationEntity, relation);
            }
            else if (strcmp(inputToken, "delrel") == 0) {
                char *originEntity = inputToken = strtok(NULL, "\" \"");
                char *destinationEntity = inputToken = strtok(NULL, "\" \"");
                char *relation = inputToken = strtok(NULL, " \"");
                delrel(originEntity, destinationEntity, relation);
            }
        }

        k = fgets(inputBuffer, INPUT_BUFFER, stdin);
    }
}

/* **************** FUNZIONI ************* */

/*
 * codice delle rotazioni ricavato dalle slide del corso.
 * nelle slide, x = rotationRoot
 */
void leftRotateEntity(entityNode *rotationRoot) {
    entityNode *y = rotationRoot->rightSon;

    rotationRoot->rightSon = y->leftSon;
    if (y->leftSon != &NIL_ENT)
        y->leftSon->father = rotationRoot;
    y->father = rotationRoot->father;
    if (rotationRoot->father == &NIL_ENT)
        entityRoot = y;
    else if (rotationRoot == rotationRoot->father->leftSon)
        rotationRoot->father->leftSon = y;
    else
        rotationRoot->father->rightSon = y;
    y->leftSon = rotationRoot;
    rotationRoot->father = y;
}

/*
 * Codice scritto sulla base di leftRotate, incertendo left e right
 */
void rightRotateEntity(entityNode *rotationRoot) {
    entityNode *y = rotationRoot->leftSon;
    rotationRoot->leftSon = y->rightSon;

    if (y->rightSon != &NIL_ENT)
        y->rightSon->father = rotationRoot;
    y->father = rotationRoot->father;
    if (rotationRoot->father == &NIL_ENT)
        entityRoot = y;
    else if (rotationRoot == rotationRoot->father->rightSon)
        rotationRoot->father->rightSon = y;
    else
        rotationRoot->father->leftSon = y;
    y->rightSon = rotationRoot;
    rotationRoot->father = y;
}

/*
 * codice delle rotazioni ricavato dalle slide del corso.
 * nelle slide, x = rotationRoot
 */
void leftRotateRelation(relationNode *rotationRoot) {
    relationNode *y = rotationRoot->rightSon;

    rotationRoot->rightSon = y->leftSon;
    if (y->leftSon != &NIL_REL)
        y->leftSon->father = rotationRoot;
    y->father = rotationRoot->father;
    if (rotationRoot->father == &NIL_REL)
        relationRoot = y;
    else if (rotationRoot == rotationRoot->father->leftSon)
        rotationRoot->father->leftSon = y;
    else
        rotationRoot->father->rightSon = y;
    y->leftSon = rotationRoot;
    rotationRoot->father = y;
}

/*
 * Codice scritto sulla base di leftRotate, incertendo left e right
 */
void rightRotateRelation(relationNode *rotationRoot) {
    relationNode *y = rotationRoot->leftSon;
    rotationRoot->leftSon = y->rightSon;

    if (y->rightSon != &NIL_REL)
        y->rightSon->father = rotationRoot;
    y->father = rotationRoot->father;
    if (rotationRoot->father == &NIL_REL)
        relationRoot = y;
    else if (rotationRoot == rotationRoot->father->rightSon)
        rotationRoot->father->rightSon = y;
    else
        rotationRoot->father->leftSon = y;
    y->rightSon = rotationRoot;
    rotationRoot->father = y;
}

/*
 * funzione che aggiunge una nuova entità. controlla prima che l'entità non sia persente, in caso esce
 * aggiunge l'entità e pooi ribilancia l'albero RB.
 * codice ricavato dalle slide del corso dsugli alberi RB
 *
 * y = newEntityFather
 * x = newEntitySearch
 * z = addedNode
 */
void addent(char *newEntity) {
    entityNode *newEntityFather = &NIL_ENT;
    entityNode *newEntitySearch = entityRoot;
    entityNode *addedNode = &entityTree[entityElements];

    if (entityElements == 0) {
        strcpy(addedNode->entityName, newEntity);
        addedNode->father = &NIL_ENT;
        addedNode->leftSon = &NIL_ENT;
        addedNode->rightSon = &NIL_ENT;
        addedNode->colour = black;
        entityElements++;
        entityRoot = addedNode;
        return;
    }

    // Se l'albero è pieno raddoppio la sua dimensione
    while (entityElements >= currentMaxEntitySize) {
        currentMaxEntitySize *= 2;
        entityTree = (entityNode *) realloc(entityTree, currentMaxEntitySize * sizeof(entityNode));
    }

    //calcolo il padre del nuovo elemento. Se l'elemento è gia presente, esco dalla funzione
    while (newEntitySearch != &NIL_ENT) {
        newEntityFather = newEntitySearch;
        if (strcmp(newEntity, newEntitySearch->entityName) == 0)
            return;
        else if (strcmp(newEntity, newEntitySearch->entityName) < 0)
            newEntitySearch = newEntitySearch->leftSon;
        else
            newEntitySearch = newEntitySearch->rightSon;
    }

    // Creo il nodo. Di default lo coloro di rosso
    strcpy(addedNode->entityName, newEntity);
    addedNode->father = newEntityFather;
    if (newEntityFather == &NIL_ENT)
        entityRoot = addedNode;
    else if (strcmp(addedNode->entityName, newEntityFather->entityName) < 0)
        newEntityFather->leftSon = addedNode;
    else
        newEntityFather->rightSon = addedNode;
    addedNode->colour = red;
    addedNode->rightSon = &NIL_ENT;
    addedNode->leftSon = &NIL_ENT;

    insertEntityFixUp(addedNode);

    // aggiorno il numero di elementi che sono salvati nell'albero
    entityElements++;
}

/*
 * Funzione di fixup per l'inserimento di un elemento nell albero
 * Ricavata dal codice nelle slide del corso
 *
 * z = addedEntity
 *
 * todo al momento la funzione è implementata in maniera ricorsiva come sulle slide. gente che ha gia terminato il progetto ha detto che non dovrebbe causare problemi di tempo o memoria, ma in caso si puo otimizzare rendendolo iteratvo
 */
void insertEntityFixUp(entityNode *addedEntity) {
    // x è il padre di addedNode
    entityNode *x;
    // y  il fratello di addedNode
    entityNode *y;

    if (addedEntity == entityRoot)
        entityRoot->colour = black;
    else {
        x = addedEntity->father;
        if (x->colour == red) {
            if (x == x->father->leftSon) {
                y = x->father->rightSon;
                if (y->colour == red) {
                    x->colour = black;
                    y->colour = black;
                    x->father->colour = red;
                    insertEntityFixUp(x->father);
                } else {
                    if (addedEntity == x->rightSon) {
                        addedEntity = x;
                        leftRotateEntity(addedEntity);
                        x = addedEntity->father;
                    }
                    x->colour = black;
                    x->father->colour = red;
                    rightRotateEntity(x->father);
                }
            } else {
                y = x->father->leftSon;
                if (y->colour == red) {
                    x->colour = black;
                    y->colour = black;
                    x->father->colour = red;
                    insertEntityFixUp(x->father);
                } else {
                    if (addedEntity == x->leftSon) {
                        addedEntity = x;
                        rightRotateEntity(addedEntity);
                        x = addedEntity->father;
                    }
                    x->colour = black;
                    x->father->colour = red;
                    leftRotateEntity(x->father);
                }
            }
        }
    }
}

/*
 * Metodo per aggiungere una relazione.
 * Per prima cosa controllo che le due entità esistano, altrimenti termino
 * Poi controllo che esista la relazione.
 * Se esiste, gli aggiungo la coppia, altrimenti creo la relazione e gli aggiungo la coppia
 * la coppia è gestita come un albero, per cui i nodi sono le entità riceventi, ordinate per numero di riceventi e come secondo parametro per ordine alfabetico.
 * todo gli alberi delle entità riceventi non sono momentaneamente alberi rosso neri, ma semplici alberi binari.
 * i mittenti sono a loro volta degli alberi, non rossoneri ma semplicemente binari, ordinati per ordine alfabetico
 */
void addrel(char *originEntity, char *destinationEntity, char *relation) {
    entityNode *destinationEntityNode;
    entityNode *originEntityNode;
    relationNode *checkedNode;
    relationNode *checkedNodeFather;
    relationNode* firstNode = &relationTree[0];
    relationNode *addedNode = &relationTree[relationElements];

    // controllo che le entità esistano. le controllo una alla volta per ottimizzare, in quanto devo salvarmi i risultati
    originEntityNode = searchEntity(originEntity);
    if (originEntityNode == &NIL_ENT)
        return;

    destinationEntityNode = searchEntity(destinationEntity);
    if (destinationEntityNode == &NIL_ENT)
        return;

    // gestisco a parte il caso della prima relazione inserita
    if (relationElements == 0) {
        strcpy(firstNode->relationName, relation);
        firstNode->father = &NIL_REL;
        firstNode->leftSon = &NIL_REL;
        firstNode->rightSon = &NIL_REL;
        firstNode->colour = black;
        relationElements++;
        relationRoot = firstNode;
        firstNode->numberOfReceiver = 1;
        firstNode->receivingList = malloc(RECEIVING_ENTITY_STOCK * sizeof(receivingNode));
        firstNode->allocatedReceiver = RECEIVING_ENTITY_STOCK;
        firstNode->receivingList[0].receivingTimes = 1;
        firstNode->receivingList[0].receiver = destinationEntityNode;
        firstNode->receivingList[0].originList = malloc(ORIGIN_ENTITY_STOCK * sizeof(entityNode*));
        firstNode->receivingList[0].allocatedOrigins = ORIGIN_ENTITY_STOCK;
        firstNode->receivingList[0].originList[0] = originEntityNode;
        return;
    }

    // mi sposto al nodo della relazione, o a suo padre se non esiste
    checkedNode = relationRoot;
    while (checkedNode != &NIL_REL && strcmp(relation, checkedNode->relationName) != 0) {
        checkedNodeFather = checkedNode->father;
        if (strcmp(relation, checkedNode->relationName) < 0)
            checkedNode = checkedNode->leftSon;
        else
            checkedNode = checkedNode->rightSon;
    }

    // todo qesta parte probabilmente puo essere ottimizzata e incorporata con la successiva, per tenere traccia gia dei punti in cui vanno fatte le aggiunte
    // se la relazione esiste, controllo che non contenga gia la coppia mittente-destinatario, nel cui caso termino
    if (checkedNode != &NIL_REL){
        for(int i = 0; i < checkedNode->numberOfReceiver; i++){
            // se ho trovato il ricevente, poi esco dal ciclo perche compare un unica volta
            if (strcmp(destinationEntity, checkedNode->receivingList[i].receiver->entityName) == 0){
                // se ho trovato il mittente cerco il destinatario.
                // effettuo una ricerca binaria, essendo i mittenti ordinati in maniera alfabetica
                int underBound = 0;
                int upperBound = checkedNode->receivingList[i].receivingTimes - 1;
                int med;
                while (underBound <= upperBound){
                    med = (underBound + upperBound)/2;
                    if (strcmp(originEntity, checkedNode->receivingList[i].originList[med]->entityName) == 0)
                        return;
                    else if (strcmp(originEntity, checkedNode->receivingList[i].originList[med]->entityName) < 0)
                        upperBound = med - 1;
                    else
                        underBound = med + 1;
                }
                break;
            }
        }
    }
    // se la relazione non esisteva, la creo
    // codice ricavato dalle slide del corso
    // todo il nodo padre di dove metterlo l'ho gia trovato, la ricerca dovrebbe essere pero molto poco dispendiosa
    else {
        // Se l'albero è pieno raddoppio la sua dimensione
        while (relationElements >= currentMaxRelationSize) {
            currentMaxRelationSize *= 2;
            relationTree = (relationNode *) realloc(relationTree, currentMaxRelationSize * sizeof(relationNode));
        }

        //codice preso dalle slide del corso
        strcpy(addedNode->relationName, relation);
        relationNode *y = &NIL_REL;
        relationNode *x = relationRoot;
        while (x != &NIL_REL) {
            y = x;
            if (strcmp(addedNode->relationName, x->relationName) < 0)
                x = x->leftSon;
            else
                x = x->rightSon;
        }
        addedNode->father = y;
        if (y == &NIL_REL)
            relationRoot = addedNode;
        else if (strcmp(addedNode->relationName, y->relationName) < 0)
            y->leftSon = addedNode;
        else
            y->rightSon = addedNode;
        addedNode->leftSon = &NIL_REL;
        addedNode->rightSon = &NIL_REL;
        addedNode->colour = red;
        addedNode->numberOfReceiver = 0;
        addedNode->receivingList = malloc(RECEIVING_ENTITY_STOCK * sizeof(receivingNode));
        addedNode->allocatedReceiver = RECEIVING_ENTITY_STOCK;

        insertRelationFixUp(addedNode);

        relationElements++;
        checkedNode = addedNode;
    }

    /*
     * a questo punto che la relazione ci fosse o no, è stata creata e viene puntata da checked node. sono inoltre sicuro che non fosse gia esistente e che le sue entità siano monitorate
     * ora inserisco la le entià che la compongono
     */
    int i = 0;
    while (i < checkedNode->numberOfReceiver && destinationEntityNode != checkedNode->receivingList[i].receiver){
        i++;
    }
    // se non ho trovato il ricevente, aggiungo l'elemento sia ai riceventi che alle sue origini
    if (i == checkedNode->numberOfReceiver){
        // se serve, amplio il vettore dei riceventi
        if (i >= checkedNode->allocatedReceiver) {
            checkedNode->receivingList = (receivingNode*) realloc(checkedNode->receivingList,(checkedNode->allocatedReceiver + RECEIVING_ENTITY_STOCK) * sizeof(receivingNode));
            checkedNode->allocatedReceiver += RECEIVING_ENTITY_STOCK;
        }
        checkedNode->numberOfReceiver++;
        checkedNode->receivingList[i].receiver = destinationEntityNode;
        // alloco il vettore delle origini
        checkedNode->receivingList[i].originList = malloc(ORIGIN_ENTITY_STOCK * sizeof(entityNode*));
        checkedNode->receivingList[i].allocatedOrigins = ORIGIN_ENTITY_STOCK;
        checkedNode->receivingList[i].receivingTimes = 1;
        checkedNode->receivingList[i].originList[0] = originEntityNode;
    }
    // altrimenti aggiungo l'origine ai riceventi, aggiorno il totale e mantengo l'ordine alfabetico
    else {
        // se necessario, amplio la dimensione del vettore delle origini
        if (checkedNode->receivingList[i].receivingTimes >= checkedNode->receivingList[i].allocatedOrigins) {
            checkedNode->receivingList[i].originList = (entityNode **) realloc(checkedNode->receivingList[i].originList, (checkedNode->receivingList[i].allocatedOrigins + ORIGIN_ENTITY_STOCK) * sizeof(entityNode *));
            checkedNode->receivingList[i].allocatedOrigins += ORIGIN_ENTITY_STOCK;
        }
        // inserisco l'elemento come ultimo, poi partendo dal fondo riordino. comunque dovrei shuffolare tutto fino in fondo, quindi è ok
        checkedNode->receivingList[i].originList[checkedNode->receivingList[i].receivingTimes] = originEntityNode;
        checkedNode->receivingList[i].receivingTimes ++;
        entityNode *temp;
        for (int j = checkedNode->receivingList[i].receivingTimes - 1; j > 0; j--){
            if (strcmp(checkedNode->receivingList[i].originList[j]->entityName, checkedNode->receivingList[i].originList[j-1]->entityName) < 0){
                temp = checkedNode->receivingList[i].originList[j];
                checkedNode->receivingList[i].originList[j] = checkedNode->receivingList[i].originList[j-1];
                checkedNode->receivingList[i].originList[j-1] = temp;
            }
            else
                break;
        }
    }
    // controllo che i ricevente in causa (i) non debba scalare la classifica per aver aumentato il numero di relazioni
    receivingNode temp;
    for (int j = i; j > 0; j--){
        // se il ricevente ha piu relazioni di quello che lo precede oppure ha lo stesso numero di relazioni ma viene prima alfabeticamente, li inverto
        if ( (checkedNode->receivingList[j].receivingTimes > checkedNode->receivingList[j-1].receivingTimes) || (checkedNode->receivingList[j].receivingTimes == checkedNode->receivingList[j-1].receivingTimes && strcmp(checkedNode->receivingList[j].receiver->entityName, checkedNode->receivingList[j-1].receiver->entityName) < 0)){
            temp = checkedNode->receivingList[j];
            checkedNode->receivingList[j] = checkedNode->receivingList[j-1];
            checkedNode->receivingList[j-1] = temp;
        }
        else
            break;
    }
}

// corregge l'inserimento all'interno dell albero RB delle relazioni
void insertRelationFixUp(relationNode *addedRelation) {
    // x è il padre di addedNode
    relationNode *x;
    // y  il fratello di addedNode
    relationNode *y;

    if (addedRelation == relationRoot)
        entityRoot->colour = black;
    else {
        x = addedRelation->father;
        if (x->colour == red) {
            if (x == x->father->leftSon) {
                y = x->father->rightSon;
                if (y->colour == red) {
                    x->colour = black;
                    y->colour = black;
                    x->father->colour = red;
                    insertRelationFixUp(x->father);
                } else {
                    if (addedRelation == x->rightSon) {
                        addedRelation = x;
                        leftRotateRelation(addedRelation);
                        x = addedRelation->father;
                    }
                    x->colour = black;
                    x->father->colour = red;
                    rightRotateRelation(x->father);
                }
            } else {
                y = x->father->leftSon;
                if (y->colour == red) {
                    x->colour = black;
                    y->colour = black;
                    x->father->colour = red;
                    insertRelationFixUp(x->father);
                } else {
                    if (addedRelation == x->leftSon) {
                        addedRelation = x;
                        rightRotateRelation(addedRelation);
                        x = addedRelation->father;
                    }
                    x->colour = black;
                    x->father->colour = red;
                    leftRotateRelation(x->father);
                }
            }
        }
    }
}

// ricerca un entità nell'albero. Se esiste torna il suo puntatore, altrimenti un puntatore a NIL_ENT
entityNode *searchEntity(char *researchedEntity) {
    entityNode *checkedNode = entityRoot;
    while (checkedNode != &NIL_ENT && strcmp(researchedEntity, checkedNode->entityName) != 0) {
        if (strcmp(researchedEntity, checkedNode->entityName) < 0)
            checkedNode = checkedNode->leftSon;
        else
            checkedNode = checkedNode->rightSon;
    }
    return checkedNode;
}

// fatto per lavorare in maniera iterativa e non con ricorsione
// todo il numero massimo di relazioni dovrebbe essere logN, quindi per ora ho messo un valore abbastanza bassp
void report() {
    relationNode* stack[MAXIMUM_RELATION_STACK];
    int stackedElements = 0;
    int spaceCheck = -1;
    int noneCheck = -1;

    // se l'albero delle relazioni è vuoto, stampa none e termina
    if (relationRoot == &NIL_REL){
        fputs("none\n", stdout);
        //printf("none\n");
        return;
    }

    stack[0] = relationRoot;
    stackedElements = 1;

    relationNode* currentRelation = relationRoot->leftSon;
    while (currentRelation != &NIL_REL || stackedElements != 0){
        // scorro i figli sinistri
        while (currentRelation != &NIL_REL){
            stack[stackedElements] = currentRelation;
            stackedElements++;
            currentRelation = currentRelation->leftSon;
        }
        currentRelation = stack[stackedElements-1];
        stackedElements--;

        // se la relazione non ha delle entità associate, non stampa niente e passa oltre
        // altrimenti stampa e modifica il frag di controllo sulle stampe
        if (currentRelation->numberOfReceiver != 0) {
            if (spaceCheck == -1) {
                fputs("\"", stdout);
                fputs(currentRelation->relationName, stdout);
                fputs("\"", stdout);
                //printf("\"%s\"", currentRelation->relationName);
                spaceCheck = 1;
            }
            else {
                fputs(" \"", stdout);
                fputs(currentRelation->relationName, stdout);
                fputs("\"", stdout);
                //printf(" \"%s\"", currentRelation->relationName);
            }
            int i = 0;
            while (i < currentRelation->numberOfReceiver && currentRelation->receivingList[i].receivingTimes == currentRelation->receivingList[0].receivingTimes) {
                fputs(" \"", stdout);
                fputs(currentRelation->receivingList[i].receiver->entityName, stdout);
                fputs("\"", stdout);
                //printf(" \"%s\"", currentRelation->receivingList[i].receiver->entityName);
                i++;
            }
            printf(" %d;", currentRelation->receivingList[0].receivingTimes);
            noneCheck = 1;
        }
        currentRelation = currentRelation->rightSon;
    }
    if (noneCheck == -1)
        fputs("none", stdout);
        //printf("none");
    //printf ("\n");
    fputs("\n", stdout);
}

void delent(char *deletedEntity) {}

void delrel(char *originEntity, char *destinationEntity, char *relation) {}

