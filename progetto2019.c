#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ********* COSTANTI ******** */

#define NAMES_LENGTH 100
#define INPUT_BUFFER 350
#define ELEMENTS_STOCK 516
#define RELATION_STOCK 16
#define RECEIVING_ENTITY_STOCK 32
#define ORIGIN_ENTITY_STOCK 32
#define MAXIMUM_RELATION_STACK 8
#define FREE_ENTITY_STOCK 256


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
void deleteEntityFixUp(entityNode* x);

// funzione per la ricerca di un entità nell'albero
entityNode *searchEntity(char *researchedEntity);

// funzioni ausiliarie delent
entityNode* treeSuccessorEntity (entityNode* x);
entityNode* treeMinimumEntity (entityNode* x);


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

entityNode** freeEntityPosition;
int freeEntityElements = 0;
int freeEntityAllocated = 0;

/* ************ MAIN ************* */
int main() {
    char inputBuffer[INPUT_BUFFER];
    char *inputToken;

    // vettori che contengono i nodi degli alberi
    entityTree = malloc(ELEMENTS_STOCK * sizeof(entityNode));
    currentMaxEntitySize = ELEMENTS_STOCK;
    relationTree = malloc(RELATION_STOCK * sizeof(relationNode));
    currentMaxRelationSize = RELATION_STOCK;

    freeEntityPosition = malloc(FREE_ENTITY_STOCK * sizeof(entityNode*));
    freeEntityAllocated = FREE_ENTITY_STOCK;

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
    entityNode *addedNode;

    if (entityElements == 0) {
        addedNode = &entityTree[0];
        strcpy(addedNode->entityName, newEntity);
        addedNode->father = &NIL_ENT;
        addedNode->leftSon = &NIL_ENT;
        addedNode->rightSon = &NIL_ENT;
        addedNode->colour = black;
        entityElements++;
        entityRoot = addedNode;
        return;
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

    // todo se scendo sotto tot potrebbe essere buono diminuire lo spazio allocato
    if (freeEntityElements > 0){
        addedNode = freeEntityPosition[freeEntityElements - 1];
        freeEntityElements--;
    }
    else {
        // Se l'albero è pieno raddoppio la sua dimensione
        if (entityElements >= currentMaxEntitySize) {
            currentMaxEntitySize *= 2;
            entityTree = (entityNode *) realloc(entityTree, currentMaxEntitySize * sizeof(entityNode));
        }
        addedNode = &entityTree[entityElements];
        entityElements++;
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
    relationNode *addedNode;

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

        addedNode = &relationTree[relationElements];

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
    // controllo che il ricevente in causa (i) non debba scalare la classifica per aver aumentato il numero di relazioni
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

void delrel(char *originEntity, char *destinationEntity, char *relation) {
    // non serve controllare che le entità esistano, poichè se non esistono non saranno comprese nella relazione
    relationNode *deletedRelation;
    receivingNode *deletedReceiver;
    int originPosition;

    // cerco la relazione assegnata. se non esiste, termino
    deletedRelation = relationRoot;
    while (deletedRelation != &NIL_REL && strcmp(deletedRelation->relationName, relation) != 0 ){
        if (strcmp(relation, deletedRelation->relationName) < 0)
            deletedRelation = deletedRelation->leftSon;
        else
            deletedRelation = deletedRelation->rightSon;
    }
    if (deletedRelation == &NIL_REL)
        return;

    // cerco il ricevente nella lista. non essendo ordinata alfabeticamente, devo scorrere tutto. se non la trovo termino
    int i = 0;
    while ( i < deletedRelation->numberOfReceiver && strcmp(deletedRelation->receivingList[i].receiver->entityName, destinationEntity) != 0){
        i++;
    }
    if (i == deletedRelation->numberOfReceiver)
        return;
    else {
        deletedReceiver = &deletedRelation->receivingList[i];
    }

    // faccio una ricerca binaria per trovare il mittente. se non lo trovo, termino.
    int bot = 0;
    int top = deletedReceiver->receivingTimes - 1;
    int findFlag = -1;
    int mid = 0;
    while (findFlag == -1 && bot <= top){
        mid = (bot + top) / 2;
        if (strcmp(deletedReceiver->originList[mid]->entityName, originEntity) == 0 )
            findFlag = 1;
        else{
            if (strcmp(originEntity, deletedReceiver->receiver[mid].entityName) < 0)
                top = mid - 1;
            else
                bot = mid + 1;
        }
    }
    if (findFlag == -1)
        return;
    else
        originPosition = mid;

    // rimuovo l'origine dalla lista, scalo tutte le origini rimanenti oltre quella eliminata di uno a sinistra per mantenere l'ordine alfabetico
    // abbasso il numero di origini per quel ricevente di uno, e se necessario rialloco il vettore dei riceventi per diminuirlo di dimensione
    // per rimuoverlo mi basta coprirlo con la scalata dei sucessivi e la riduzione di uno del totale
    for (int j = originPosition; j < deletedReceiver->receivingTimes - 1; j++){
        deletedReceiver->originList[j] = deletedReceiver->originList[j+1];
    }
    deletedReceiver->receivingTimes--;

    // devo ricalcolare la classifica dei riceventi se necessario, abbassando quello che ha appena perso un mittente
    int k;
    for (k = i; k < deletedRelation->numberOfReceiver - 1; k++){
        if (deletedRelation->receivingList[k].receivingTimes < deletedRelation->receivingList[k+1].receivingTimes || ( deletedRelation->receivingList[k].receivingTimes == deletedRelation->receivingList[k+1].receivingTimes && strcmp(deletedRelation->receivingList[k].receiver->entityName, deletedRelation->receivingList[k+1].receiver->entityName) > 0 ) ){
            receivingNode temp = deletedRelation->receivingList[k];
            deletedRelation->receivingList[k] = deletedRelation->receivingList[k+1];
            deletedRelation->receivingList[k+1] = temp;
        }
        else
            break;
    }

    // se il mittente rimane senza origini, lo elimino e diminuisco i numero di mittenti nella relazione.
    // devo liberare la memoria allocata per i mittenti e scalare tutti i riceventi successivi
    if (deletedReceiver->receivingTimes == 0){
        free(deletedReceiver->originList);
        for (int j = k; j < deletedRelation->numberOfReceiver - 1 ; j++){
            deletedRelation->receivingList[j] = deletedRelation->receivingList[j+1];
        }
        deletedRelation->numberOfReceiver--;
        if (deletedRelation->numberOfReceiver < deletedRelation->allocatedReceiver / 3 && deletedRelation->allocatedReceiver > RECEIVING_ENTITY_STOCK)
            deletedRelation->receivingList = (receivingNode*) realloc(deletedRelation->receivingList, deletedRelation->allocatedReceiver / 2);
    }
    // riduco la dimensione solo se scendo sotto il terzo degli elementi allocati effettivamente presenti (riduco di metà)
    // faccio un terzo e non la metà per evitare che una cancellazione e inserimento successivi e ripetuti mi facciano fare troppe realloc
    // todo se la memoria allocata è minore di uno stock non riduco, perchè non dovrebbe essere eccessivamente rilevane e qualora arrivasse a dimensioni piccole mi causerebbe realloc continue
    // todo valutare se ridurre quando scendo sotto il quarto per mantenere abbastanza spazi liberi per aggiungere nuove origini (analogo sopra)
    else if (deletedReceiver->receivingTimes < deletedReceiver->allocatedOrigins / 3 && deletedReceiver->allocatedOrigins > ORIGIN_ENTITY_STOCK){
        deletedReceiver->originList = (entityNode**) realloc(deletedReceiver->originList, deletedReceiver->allocatedOrigins / 2);
    }
}

/*
 * Per cancellare un entità bisogna innanzitutto verificare che questa esista, altrimenti termina. se esiste ci salvo un puntatore
 * a questo punto scorro tutte le relazioni, e per ogni relazione verifico tutti i riceventi.
 * se il ricevente è l'entità stessa, libero la memoria associata all sue oigini, cancello il ricevente e scalo tutto quello che c'è dopo avanti di uno.
 * se il ricevente è un altra entità, faccio una ricerca binaria per verificare se possiede l'entità eliminata come origine.
 * se così è, la cancello e sposto tutto quello che c'è dopo avanti di uno per mantenere l'ordine alfabetico, e diminuisco di uno il numero di mittenti.
 * se serve, rimpicciolisco lo spazio allocato per i mittenti
 * a questo punto devo cancellare l'entità dalle relazioni:
 * mi salvo un puntatore esterno all'entità e eseguo l'algoritmo di eliminazione per gli alberi rosso neri.
 * a questo punto, per riempire il buco in memoria lasciato dall'entità eliminata, la sostituisco tramite il puntatore esterno con l'ultima entità salvata nella lista.
 * diminuisco il numero di entità salvate di uno e, se serve, riduco il numero di entità allocate
 *
 */
void delent(char *deletedEntity) {
    entityNode *deletedEntityNode = entityRoot;

    // verifico che l'entità esista, se non esiste termino
    while (deletedEntityNode != &NIL_ENT && strcmp(deletedEntityNode->entityName, deletedEntity) != 0) {
        if (strcmp(deletedEntity, deletedEntityNode->entityName) < 0)
            deletedEntityNode = deletedEntityNode->leftSon;
        else
            deletedEntityNode = deletedEntityNode->rightSon;
    }
    if (deletedEntityNode == &NIL_ENT)
        return;

    // scorro le relazioni per eliminare le entità
    for (int i = 0; i < relationElements; i++) {
        // per ogni relazione controllo tutte le entità riceventi
        for (int j = 0; j < relationTree[i].numberOfReceiver; j++) {
            receivingNode* checkedReceiver = &relationTree[i].receivingList[j];
            // se l'entità ricevente è quella eliminata, elimino tutto e libero la memoria
            if (checkedReceiver->receiver == deletedEntityNode) {
                // libero la memoria dedicata ai mittenti
                free(checkedReceiver->originList);
                // sposto di uno in indietro tutti i riceventi rimanenti
                for (int k = j; k < relationTree[i].numberOfReceiver - 1; ++k) {
                    relationTree[i].receivingList[k] = relationTree[i].receivingList[k + 1];
                }
                relationTree[i].numberOfReceiver--;
                // diminuisco di uno j per far controllare il prossimo nodo, che altrimenti sarebbe saltato
                j--;
                // se serve riduco la memoria allocata
                if (relationTree[i].numberOfReceiver < relationTree[i].allocatedReceiver / 3 &&
                    relationTree[i].allocatedReceiver > RECEIVING_ENTITY_STOCK) {
                    relationTree[i].receivingList = (receivingNode *) realloc(relationTree[i].receivingList,
                                                                              relationTree[i].allocatedReceiver / 2);
                }
            }
            // se non è l'entità eliminata, faccio una ricerca binaria tra i mittenti. se lo trovo, sposto tutto ciò che c'è oltre indietro di uno
            // se il mittente era unico, devo cancellare il ricevente e eventualmente riallocare
            else {
                int bot = 0;
                int top = checkedReceiver->receivingTimes - 1;
                int mid = 0;
                int foundFlag = -1;
                while (foundFlag == -1 && bot <= top) {
                    mid = (bot + top) / 2;
                    if (checkedReceiver->originList[mid] == deletedEntityNode)
                        foundFlag = 1;
                    else if (strcmp(deletedEntity, checkedReceiver->originList[mid]->entityName) < 0)
                        top = mid - 1;
                    else
                        bot = mid + 1;
                }
                // se non l'ho trovato non faccio niente, altrimenti lo elimino
                if (foundFlag == 1) {
                    for (int k = mid; k < checkedReceiver->receivingTimes - 1; k++) {
                        checkedReceiver->originList[k] = checkedReceiver->originList[k + 1];
                    }
                    checkedReceiver->receivingTimes--;
                    // se serve rialloco lo spazio
                    if (checkedReceiver->receivingTimes < checkedReceiver->allocatedOrigins / 3 &&
                        checkedReceiver->allocatedOrigins > ORIGIN_ENTITY_STOCK)
                        checkedReceiver->originList = (entityNode **) realloc(checkedReceiver->originList,
                                                                             checkedReceiver->allocatedOrigins / 2);

                    // se non sono rimasti piu mittenti, devo cancellare il ricevente
                    if (checkedReceiver->receivingTimes == 0) {
                        free(checkedReceiver->originList);
                        for (int k = j; k < relationTree[i].numberOfReceiver - 1; k++) {
                            relationTree[i].receivingList[k] = relationTree[i].receivingList[k + 1];
                        }
                        relationTree[i].numberOfReceiver--;
                        j--;
                        // evntualmente diminuisco lo spazio destinato ai riceevnti
                        if (relationTree[i].numberOfReceiver < relationTree[i].allocatedReceiver / 3 &&
                            relationTree[i].allocatedReceiver > RECEIVING_ENTITY_STOCK)
                            relationTree[i].receivingList = (receivingNode *) realloc(relationTree[i].receivingList,
                                                                                      relationTree[i].allocatedReceiver /
                                                                                      2);
                    }
                    // se invece sono rimasti dei mittenti, devo far scalare il ricevente nella nuova posizione ordinata
                    // todo riscalando ora le posizioni vado a ricontrollare le cose scalate. facendo tutto in fondo però impiego piu tempo a riordinare.
                    // todo controllare in caso ci siano problemi di tempo
                    else {
                        int moveFlag = -1;
                        for (int k = j; k < relationTree[i].numberOfReceiver - 1; k++){
                            if (relationTree[i].receivingList[k].receivingTimes < relationTree[i].receivingList[k+1].receivingTimes || (relationTree[i].receivingList[k].receivingTimes == relationTree[i].receivingList[k+1].receivingTimes && strcmp(relationTree[i].receivingList[k].receiver->entityName, relationTree[i].receivingList[k+1].receiver->entityName) > 0)){
                                receivingNode temp = relationTree[i].receivingList[k];
                                relationTree[i].receivingList[k] = relationTree[i].receivingList[k+1];
                                relationTree[i].receivingList[k+1] = temp;
                                moveFlag = 1;
                            }
                            else
                                break;
                        }
                        if (moveFlag == 1)
                            j--;
                    }
                }
            }
        }
    }

    // a questo punto devo eliminare l'entità
    // codice preso da rb-delete delle slide
    entityNode *y;
    entityNode *x;
    if (deletedEntityNode->leftSon == &NIL_ENT || deletedEntityNode->rightSon == &NIL_ENT)
        y = deletedEntityNode;
    else
        y = treeSuccessorEntity(deletedEntityNode);
    if (y->leftSon != &NIL_ENT)
        x = y->leftSon;
    else
        x = y->rightSon;
    x->father = y->father;
    if (y->father == &NIL_ENT)
        entityRoot = x;
    else if (y == y->father->leftSon)
        y->father->leftSon = x;
    else
        y->father->rightSon = x;
    if (y != deletedEntityNode)
        strcpy(deletedEntityNode->entityName, y->entityName);
    if (y->colour == black)
        deleteEntityFixUp(x);

    // y è la cella che è stata liberata
    if (freeEntityElements >= freeEntityAllocated - 1){
          freeEntityAllocated += FREE_ENTITY_STOCK;
          freeEntityPosition = (entityNode**) realloc(freeEntityPosition, freeEntityAllocated * sizeof(entityNode*));
    }
    freeEntityPosition[freeEntityElements] = y;
    freeEntityElements++;
}

// presa dalle slide
entityNode* treeSuccessorEntity (entityNode* x) {
    if (x->rightSon != &NIL_ENT)
        return treeMinimumEntity(x->rightSon);
    entityNode* y = x->father;
    while (y != &NIL_ENT && x == y->rightSon){
        x = y;
        y = y->father;
    }
    return y;
}

// presa dalle slide
entityNode* treeMinimumEntity (entityNode* x){
    while (x->leftSon != &NIL_ENT)
        x = x->leftSon;
    return x;
}

// presa dalle slide del corso
void deleteEntityFixUp(entityNode* x){
    entityNode* w;

    if (x->colour == red || x->father == &NIL_ENT){
        x->colour = black;
    }
    else if (x == x->father->leftSon){
        w = x->father->rightSon;
        if (w->colour == red){
            w->colour = black;
            x->father->colour = red;
            leftRotateEntity(x->father);
            w = x->father->rightSon;
        }
        if (w->leftSon->colour == black && w->rightSon->colour == black){
            w->colour = red;
            deleteEntityFixUp(x->father);
        }
        else {
            if (w->rightSon->colour == black){
                w->leftSon->colour = black;
                w->colour = red;
                rightRotateEntity(w);
                w = x->father->rightSon;
            }
            w->colour = x->father->colour;
            x->father->colour = black;
            w->rightSon->colour = black;
            leftRotateEntity(x->father);
        }
    }
    else{
        w = x->father->leftSon;
        if (w->colour == red){
            w->colour = black;
            x->father->colour = red;
            rightRotateEntity(x->father);
            w = x->father->leftSon;
        }
        if (w->rightSon->colour == black && w->leftSon->colour == black){
            w->colour = red;
            deleteEntityFixUp(x->father);
        }
        else {
            if (w->leftSon->colour == black){
                w->rightSon->colour = black;
                w->colour = red;
                leftRotateEntity(w);
                w = x->father->leftSon;
            }
            w->colour = x->father->colour;
            x->father->colour = black;
            w->leftSon->colour = black;
            rightRotateEntity(x->father);
        }
    }
}