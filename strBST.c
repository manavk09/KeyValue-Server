#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct node{
    char* key;
    char* value;
    struct node* leftChild;
    struct node* rightChild;
} node;

typedef struct BST{
	pthread_mutex_t lock;
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
    pthread_mutex_lock(&tree->lock);
    tree->root = insertHelper(tree->root, key, value);
    tree->totalCount++;
    pthread_mutex_unlock(&tree->lock);
}

BST* newBST(){
    BST* tree = malloc(sizeof(BST));
    tree->root = NULL;
    tree->totalCount = 0;
    pthread_mutex_init(&tree->lock, NULL);
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

node* findMin(node* root){
    if(root == NULL)
        return NULL;
    else if(root->leftChild != NULL)
        return findMin(root->leftChild);
    return root;
}

node* deleteValueHelper(node* root, char* key, BST* tree){
    if(root == NULL){
        printf("Key not found\n");
        return NULL;
    }
    else if(strcmp(key, root->key) < 0)                             //If the search value is less than the current node, continue search left
        root->leftChild = deleteValueHelper(root->leftChild, key, tree);
    else if(strcmp(key, root->key) > 0)                               //If the search value is greater than the current node, continue search right
        root->rightChild = deleteValueHelper(root->rightChild, key, tree);
    else{           //If the item is found
        if(root->leftChild == NULL && root->rightChild == NULL){
            free(root->key);
            free(root->value);
            free(root);
            tree->totalCount--;
            return NULL;
        }

        else if(root->leftChild == NULL || root->rightChild == NULL){
            node* newParent;
            if(root->leftChild == NULL){
                newParent = root->rightChild;
            }
            else{
                newParent = root->leftChild;
            }
            free(root->key);
            free(root->value);
            free(root);
            tree->totalCount--;
            return newParent;
        }
        else{
            node* newParent;
            newParent = findMin(root->rightChild);
            free(root->key);
            free(root->value);

            char* tempKey = malloc(strlen(newParent->key) + 1);
            char* tempVal = malloc(strlen(newParent->value) + 1);
            strcpy(tempKey, newParent->key);
            strcpy(tempVal, newParent->value);

            root->key = tempKey;
            root->value = tempVal;
            root->rightChild = deleteValueHelper(root->rightChild, newParent->key, tree);
        }
    }
    return root;
}

void deleteValue(BST* tree, char* key){
    pthread_mutex_lock(&tree->lock);
    tree->root = deleteValueHelper(tree->root, key, tree);
    pthread_mutex_unlock(&tree->lock);
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
    insert(tree1, "key2", "yo2");
    insert(tree1, "key4", "yo4");
    insert(tree1, "key3", "yo3");
    insert(tree1, "key6", "yo6");
    insert(tree1, "key5", "yo5");
    insert(tree1, "key1", "yo1");
    insert(tree1, "yek99", "yo99");
    printf("Tree count: %d\n", tree1->totalCount);
    printf("deleting key3\n");
    deleteValue(tree1, "key3");
    printTree(tree1);
    printf("Tree count after one delete: %d\n", tree1->totalCount);
    freeBST(tree1);
    
}
*/