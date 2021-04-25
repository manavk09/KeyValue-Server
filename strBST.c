#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node{
    char* key;
    char* value;
    struct node* leftChild;
    struct node* rightChild;
} node;

typedef struct BST{
    int totalCount;
    struct node* root;
} BST;

node* newBSTNode(char* key, char* value){
    node* newNode;
    newNode = malloc(sizeof(node));
    char* tempKey = malloc(strlen(key) + 1);
    char* tempVal = malloc(strlen(value) + 1);
    strcpy(tempKey, key);
    strcpy(tempVal, value);
    newNode->key = tempKey;
    newNode->value = tempVal;
    newNode->leftChild = NULL;
    newNode->rightChild = NULL;
    return newNode;
}

node* insertHelper(node* root, char* key, char* value){
    if(root == NULL){
        return newBSTNode(key, value);
    }
    else if(strcmp(key, root->key) == 0){     //If the value already exists, just increase the count by one
        printf("Key already exists\n");
    }
    else if(strcmp(key, root->key) < 0){
        root->leftChild = insertHelper(root->leftChild, key, value);
    }
    else{
        root->rightChild = insertHelper(root->rightChild, key, value);
    }
    return root;
}

void insert(BST* tree, char* key, char* value){
    tree->root = insertHelper(tree->root, key, value);
}

BST* newBST(){
    BST* tree = malloc(sizeof(BST));
    tree->root = NULL;
    tree->totalCount = 0;
    return tree;
}

void printTreeHelper(node* root){
    if(root != NULL){
        printTreeHelper(root->leftChild);
        printf("Key: %s\tValue: %s\n", root->key, root->value);
        printTreeHelper(root->rightChild);
    }
}

void printTree(BST* tree){
    printTreeHelper(tree->root);
}

node* findValueHelper(node* root, char* key){
    if(root == NULL || strcmp(key, root->key) == 0)
        return root;
    else if(strcmp(key, root->key) < 0)                   //If the search value is less than the current node, continue search left
        findValueHelper(root->leftChild, key);
    else                                //If the search value is greater than the current node, continue search right
        findValueHelper(root->rightChild, key);
}

node* findValue(BST* tree, char* key){
    return findValueHelper(tree->root, key);
}

void freeBSTHelper(node* root){
    if(root != NULL){
        freeBSTHelper(root->rightChild);
        freeBSTHelper(root->leftChild);
        free(root->key);
        free(root->value);
        free(root);
    }
}

void freeBST(BST* tree){
    freeBSTHelper(tree->root);
    free(tree);
}

/*
int main(int argc, char **argv)
{
	BST* tree1 = newBST();
    insert(tree1, "key1", "yo1");
    insert(tree1, "key2", "yo2");
    insert(tree1, "key6", "yo6");
    insert(tree1, "yek99", "yo99");
    printTree(tree1);
    printf("%s\n", findValue(tree1, "key6")->value);
}
*/