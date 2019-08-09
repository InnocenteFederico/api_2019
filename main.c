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
entityNode *entityRoot;
int entityElements = 0;
int currentMaxEntitySize = ELEMENTS_STOCK;

relationshipPointer *relationshipTree;
relationshipNode *relationshipRoot;
int relationshipElements = 0;
int currentMaxRelationshipSize = RELATIONSHIP_STOCK;

entityNode NIL_ENT;
relationshipNode NIL_REL;

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

void transplantEntity(entityNode *firstSubRoot, entityNode *secondSubRoot);

entityNode *entitySubMinimum (entityNode *subRoot);

entityNode copyEntityNode(entityNode *copiedNode);

/* ******************** MAIN ********************** */
int main() {
    char inputBuffer[INPUT_BUFFER];
    char *inputToken;

    entityTree = malloc(ELEMENTS_STOCK * sizeof(entityNode));
    relationshipTree = malloc(RELATIONSHIP_STOCK * sizeof(relationshipNode));

    NIL_ENT.father = &NIL_ENT;
    NIL_ENT.leftSon = &NIL_ENT;
    NIL_ENT.rightSon = &NIL_ENT;
    NIL_ENT.nodeColour = black;

    //todo inizializzare NIL_REL e rel root

    *entityTree[0] = NIL_ENT;
    *relationshipTree[0] = NIL_REL;

    entityRoot = entityTree[0];
    relationshipRoot = relationshipTree[0];

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
    entityNode *newEntityFather = &NIL_ENT;
    entityNode *newEntitySearch = entityRoot;
    const entityNode *addedNode = entityTree[entityElements];

    //calcolo il padre del nuovo elemento. Se l'elemento è gia presente, esco dalla funzione
    while (newEntitySearch != &NIL_ENT) {
        newEntityFather = newEntitySearch;
        if (strcmp(newEntity, newEntitySearch->entity.name) == 0)
            return;
        else if (strcmp(newEntity, newEntitySearch->entity.name) < 0)
            newEntitySearch = newEntitySearch->leftSon;
        else
            newEntitySearch = newEntitySearch->rightSon;
    }

    // Se l'albero è pieno raddoppio la sua dimensione
    while (entityElements >= currentMaxEntitySize) {
        currentMaxEntitySize *= 2;
        entityTree = (entityNode **) realloc(entityTree, currentMaxEntitySize);
    }

    // Creo il nodo. Di default lo coloro di rosso
    strcpy(entityTree[entityElements]->entity.name, newEntity);
    entityTree[entityElements]->nodeColour = red;
    *entityTree[entityElements]->rightSon = NIL_ENT;
    *entityTree[entityElements]->leftSon = NIL_ENT;
    *entityTree[entityElements]->father = *newEntityFather;

    // Inserimento
    if (addedNode->father == &NIL_ENT)
        entityRoot = entityTree[entityElements];
    else if (strcmp(newEntity, newEntityFather->entity.name) < 0)
        newEntityFather->leftSon = entityTree[entityElements];
    else
        newEntityFather->rightSon = entityTree[entityElements];

    entityNode *temp;
    entityNode *fixupCheck = entityTree[entityElements];
    // Fixing dell'albero
    while (fixupCheck->father->nodeColour == red){
        if (fixupCheck->father == getEntityGrandparent(*fixupCheck)->leftSon){
            temp = getEntityGrandparent(*fixupCheck)->rightSon;
            if (temp->nodeColour == red){
                fixupCheck->father->nodeColour = black;
                temp->nodeColour = black;
                getEntityGrandparent(*fixupCheck)->nodeColour = red;
                fixupCheck = getEntityGrandparent(*fixupCheck);
            }
            else if (fixupCheck == fixupCheck->father->rightSon){
                fixupCheck = fixupCheck->father;
                rotateEntityLeft(fixupCheck);
            }
            // todo check qua, pag 236 libro, l'else non c'è ma credo vada aggiunto
            else {
                fixupCheck->father->nodeColour = black;
                getEntityGrandparent(*fixupCheck)->nodeColour = red;
                rotateEntityRight(getEntityGrandparent(*fixupCheck));
            }
        }
        else {
            temp = getEntityGrandparent(*fixupCheck)->leftSon;
            if (temp->nodeColour == red){
                fixupCheck->father->nodeColour = black;
                temp->nodeColour = black;
                getEntityGrandparent(*fixupCheck)->nodeColour = red;
                fixupCheck = getEntityGrandparent(*fixupCheck);
            }
            else if (fixupCheck == fixupCheck->father->leftSon){
                fixupCheck = fixupCheck->father;
                rotateEntityLeft(fixupCheck);
            }
            else {
                fixupCheck->father->nodeColour = black;
                getEntityGrandparent(*fixupCheck)->nodeColour = red;
                rotateEntityRight(getEntityGrandparent(*fixupCheck));
            }
        }
    }
    entityTree[0]->nodeColour = black;
    entityElements++;
}

void delent(char deletedEntity[]){
    entityNode *deletedNode = entityRoot;
    entityNode savedNode;
    entityNode *pSavedNode = &savedNode;
    entityNode *temp;
    nodeColour savedNodeOriginalColor;

    // Find the node to delete
    while ( deletedNode != &NIL_ENT && strcmp(deletedEntity, deletedNode->entity.name) != 0 ){
        if (strcmp(deletedEntity, deletedNode->entity.name) < 0)
            deletedNode = deletedNode->leftSon;
        else
            deletedNode = deletedNode->rightSon;
    }
    if (deletedNode == &NIL_ENT)
        return;

    savedNode = copyEntityNode(deletedNode);
    savedNodeOriginalColor = savedNode.nodeColour;

    if (deletedNode->leftSon == &NIL_ENT){
        temp = deletedNode->rightSon;
        transplantEntity(deletedNode, deletedNode->rightSon);
    }
    else if (deletedNode->rightSon == &NIL_ENT){
        temp = deletedNode->leftSon;
        transplantEntity(deletedNode, deletedNode->leftSon);
    }
    else {
        savedNode = copyEntityNode(entitySubMinimum(deletedNode->rightSon));
        savedNodeOriginalColor = savedNode.nodeColour;
        temp = savedNode.rightSon;
        if (savedNode.father == deletedNode)
            temp->father = deletedNode;
        else {
            transplantEntity(pSavedNode, pSavedNode->rightSon);
            savedNode.rightSon = deletedNode->rightSon;
            savedNode.rightSon->father = pSavedNode;
        }
        transplantEntity(deletedNode, pSavedNode);
        pSavedNode->leftSon = deletedNode->leftSon;
        pSavedNode->leftSon->father = pSavedNode;
        pSavedNode->nodeColour = deletedNode->nodeColour;
    }
    // Fixing
    if (savedNodeOriginalColor == black){
        
    }

}

void rotateEntityLeft(entityNode *subRoot){
    entityNode *temp;

    temp = subRoot->rightSon;
    subRoot->rightSon = temp->leftSon;
    if (temp->leftSon != &NIL_ENT)
        temp->leftSon->father = subRoot;
    temp->father = subRoot->father;
    if (subRoot->father == &NIL_ENT)
        entityRoot = temp;
    else if (subRoot == subRoot->father->leftSon)
        subRoot->father->leftSon = temp;
    else
        subRoot->father->rightSon = temp;
    temp->leftSon = subRoot;
    subRoot->father = temp;
}

void rotateEntityRight(entityNode *subRoot) {
    entityNode *temp;

    temp = subRoot->leftSon;
    subRoot->leftSon = temp->rightSon;
    if (temp->rightSon != &NIL_ENT)
        temp->rightSon->father = subRoot;
    temp->father = subRoot->father;
    if (subRoot->father == &NIL_ENT)
        entityRoot = temp;
    else if (subRoot == subRoot->father->rightSon)
        subRoot->father->rightSon = temp;
    else
        subRoot->father->leftSon = temp;
    temp->rightSon = subRoot;
    subRoot->father = temp;
}

void transplantEntity(entityNode *firstSubRoot, entityNode *secondSubRoot){
    if (firstSubRoot->father == &NIL_ENT)
        entityRoot = secondSubRoot;
    else if (firstSubRoot == firstSubRoot->father->leftSon)
        firstSubRoot->father->leftSon = secondSubRoot;
    else
        firstSubRoot->father->rightSon = secondSubRoot;
    secondSubRoot->father = firstSubRoot->father;
}

entityNode *entitySubMinimum (entityNode *subRoot){
    while (subRoot->leftSon != &NIL_ENT)
        subRoot = subRoot->leftSon;
    return subRoot;
}

entityNode copyEntityNode(entityNode *copiedNode){
    entityNode copy;
    copy.rightSon = copiedNode->rightSon;
    copy.leftSon = copiedNode->leftSon;
    copy.entity = copiedNode->entity;
    copy.father = copiedNode->father;
    copy.nodeColour = copiedNode->nodeColour;
    return copy;
}