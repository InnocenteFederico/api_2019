#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mach/vm_region.h>

#define ELEMENTS_STOCK 128
#define NAMES_LENGTH 32
#define RELATIONSHIP_STOCK 16
#define INPUT_BUFFER 1024

/* ****************************** Type definitions *************** */

typedef enum {
    red, black
} nodeColour;

typedef struct {
    char name[NAMES_LENGTH];
} entity;

typedef struct {
    char name[NAMES_LENGTH];
} relationship;

typedef struct EN {
    entity entity;
    nodeColour nodeColour;
    struct RN *relationshipNode[RELATIONSHIP_STOCK];
    struct EN *entityDestinationNode[RELATIONSHIP_STOCK];
    struct EN *father;
    struct EN *leftSon;
    struct EN *rightSon;
} entityNode;

typedef struct RN {
    relationship relationship;
    nodeColour nodeColour;
    int instanceNumber;
    struct RN *father;
    struct RN *leftSon;
    struct RN *rightSon;
} relationshipNode;

typedef entityNode *entityNodePointer;

typedef relationshipNode *relationshipPointer;

/* *********************** Global variables *************** */

entityNodePointer *entityTree;
int entityElements = 0;
int currentMaxEntitySize = ELEMENTS_STOCK;

relationshipPointer *relationshipTree;
int relationshipElements = 0;
int currentMaxRelationshipSize = RELATIONSHIP_STOCK;

entityNode NIL;

/* ******************* Functions prototypes **************** */
void report();

void addent(char newEntity[]);

void delent(char deletedEntity[]);

void addrel(char *originalEntity, char *destinationEntity, char *relationship);

void delrel(char *originalEntity, char *destinationEntity, char *relationship);

entityNode *getEntityGrandparent(entityNode node);

entityNode *getEntityUncle(entityNode node);

void rotateEntityLeft(entityNode *subRoot);

void rotateEntityRight(entityNode *subRoot);

/* ******************** MAIN ********************** */
int main() {
    char inputBuffer[INPUT_BUFFER];
    char *inputToken;

    entityTree = malloc(ELEMENTS_STOCK * sizeof(entityNode));
    relationshipTree = malloc(RELATIONSHIP_STOCK * sizeof(relationshipNode));

    NIL.father = &NIL;
    NIL.leftSon = &NIL;
    NIL.rightSon = &NIL;
    NIL.nodeColour = black;

    *entityTree[0] = NIL;
    *entityTree[0] = NIL;

    fgets(inputBuffer, INPUT_BUFFER, stdin);
    while (strcmp(inputBuffer, "end") != 0) {

        if (strcmp(inputBuffer, "report") == 0)
            report();
        else {
            inputToken = strtok(inputBuffer, " ");

            if (strcmp(inputToken, "addent") == 0)
                addent(strtok(NULL, "\n"));
            else if (strcmp(inputToken, "delent") == 0)
                delent(strtok(NULL, "\n"));
            else if (strcmp(inputToken, "addrel") == 0)
                addrel(strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, "\n"));
            else if (strcmp(inputToken, "delrel") == 0)
                delrel(strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, "\n"));
        }

        fgets(inputBuffer, INPUT_BUFFER, stdin);
    }
    return 0;
}


/* ****************** Functions implementation *************** */

entityNode *getEntityGrandparent(entityNode node) {
    return node.father->father;
}

entityNode *getEntityUncle(entityNode node) {
    if (node.father == getEntityGrandparent(node)->leftSon)
        return getEntityGrandparent(node)->rightSon;
    else
        return getEntityGrandparent(node)->leftSon;
}

void addent(char newEntity[]) {
    entityNode *newEntityFather = &NIL;
    entityNode *newEntitySearch = entityTree[entityElements];
    const entityNode *currentNode = entityTree[entityElements];

    //calcolo il padre del nuovo elemento. Se l'elemento è gia presente, esco dalla funzione
    while (newEntitySearch != &NIL) {
        if (strcmp(newEntity, newEntitySearch->entity.name) == 0)
            return;
        else if (strcmp(newEntity, newEntitySearch->entity.name) < 0) {
            newEntityFather = newEntitySearch;
            newEntitySearch = newEntitySearch->leftSon;
        } else {
            newEntityFather = newEntitySearch;
            newEntitySearch = newEntitySearch->rightSon;
        }
    }

    // Se l'albero è pieno raddoppio la sua dimensione
    while (entityElements >= currentMaxEntitySize) {
        currentMaxEntitySize *= 2;
        entityTree = (entityNode **) realloc(entityTree, currentMaxEntitySize);
    }

    // Creo il nodo. Di default lo coloro di rosso
    strcpy(entityTree[entityElements]->entity.name, newEntity);
    entityTree[entityElements]->nodeColour = red;
    *entityTree[entityElements]->rightSon = NIL;
    *entityTree[entityElements]->leftSon = NIL;
    *entityTree[entityElements]->father = *newEntityFather;

    /*
     * Se il nuovo nodo è la radice dell'albero, viene inserito come radice, e padre e figli vengono settati a NULL.
     * Il nodo viene colorato di nero
     */
    if (entityTree[entityElements]->father == &NIL)
        entityTree[0]->nodeColour = black;
    /*
     * Se il padre del nodo è nero, l'albero è ok.
     * Se il padre e lo zio sono rossi, li setto neri e setto il nonno rosso. A questo punto ricontrollo l'albero salendo fino alla radice
     */
    else if (entityTree[entityElements]->father->nodeColour == red) {
        if (getEntityUncle(*entityTree[entityElements]) != &NIL && getEntityUncle(*entityTree[entityElements])->nodeColour == red) {
            entityNode *checkedNode = entityTree[entityElements];
            do {
                if (checkedNode->father == &NIL)
                    checkedNode->nodeColour = black;
                checkedNode->father->nodeColour = black;
                getEntityUncle(*checkedNode)->nodeColour = black;
                getEntityGrandparent(*checkedNode)->nodeColour = red;
                checkedNode = getEntityGrandparent(*checkedNode);
            } while (getEntityUncle(*checkedNode) != &NIL && checkedNode->father == red && getEntityUncle(*checkedNode)->nodeColour == red);
        }
        /*
         * Il padre è rosso.
         * Se lo zio e nero, si fa una rotazione sinistra portando il nuovo elemento sull'esterno
         */
        else {
            // se è figlio dx di figlio sx
            if (currentNode == currentNode->father->rightSon && currentNode->father == getEntityGrandparent(*currentNode)->leftSon)
                rotateEntityLeft(currentNode->father);
            // Se è figlio sx di figlio dx
            else if (currentNode == currentNode->father->leftSon && currentNode->father == getEntityGrandparent(*currentNode)->rightSon)
                rotateEntityRight(currentNode->father);
            /*
             * Se il padre è rosso e il nonno e lo zio sono neri.
             * Sarà la situazione di nonno->sinistro->sinistro o nonno->destro->destro, perchè in teoria la rotazione è gia stata fatta prima
             */
            if (getEntityGrandparent(*currentNode)->nodeColour == black && getEntityUncle(*currentNode)->nodeColour == black){
                currentNode->father->nodeColour = black;
                getEntityGrandparent(*currentNode)->nodeColour = red;
                if (currentNode == currentNode->father->leftSon && currentNode->father == getEntityGrandparent(*currentNode)->leftSon)
                    rotateEntityRight(currentNode->father);
                // teoricamente qui dovrebbe esserci n=n->father->right && n->father=gramfather(n)
                else
                    rotateEntityLeft(currentNode->father);
            }
        }
    }
}

void delent(char deletedEntity[]){
    entityNode *deletedNode = entityTree[0];
    while (deletedNode != &NIL || strcmp(deletedNode->entity.name, deletedEntity) != 0){
        if (strcmp(deletedNode->entity.name, deletedEntity) < 0 )
            deletedNode = deletedNode->leftSon;
        else
            deletedNode = deletedNode->rightSon;
    }
    if (deletedNode == &NIL)
        return;

    /*
     * Se un nodo ha due figli non foglia, si sostituisce con il valore minimo del sottoalbero sinistro o massimo del sottoalbero destro, per poi procedere all'eliminazioe del valore copiato.
     * Il colore rimane invariato a quello vecchio del nodo eliminato, in modo da non violare le proprietà di colore
     */
    if (deletedNode->leftSon != &NIL && deletedNode->rightSon != &NIL){
        entityNode *substituteNode = deletedNode;
        if ()
    }
}

void rotateEntityLeft(entityNode *subRoot){
    entityNode *temp;

    temp = subRoot->rightSon;
    subRoot->rightSon = subRoot->rightSon->leftSon;
    if (subRoot == subRoot->father->leftSon)
        subRoot->father->leftSon = temp;
    else
        subRoot->father->rightSon = temp;
    temp->father = subRoot->father;
    temp->leftSon = subRoot;
    subRoot->father = temp;
}

void rotateEntityRight(entityNode *subRoot) {
    entityNode *temp;

    temp = subRoot->leftSon;
    subRoot->leftSon = subRoot->leftSon->rightSon;
    if (subRoot == subRoot->father->leftSon)
        subRoot->father->leftSon = temp;
    else
        subRoot->father->rightSon = temp;
    temp->father = subRoot->father;
    temp->rightSon = subRoot;
    subRoot->father = temp;
}