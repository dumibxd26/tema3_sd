#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."


FileTree createFileTree(char* rootFolderName) {
    // TODO
}

void freeTree(FileTree fileTree) {
    // TODO
}


void ls(TreeNode* currentNode, char* arg) {
    // TODO
}


void pwd(TreeNode* treeNode) {
    // TODO
}


TreeNode* cd(TreeNode* currentNode, char* path) {
    // TODO
}


void tree(TreeNode* currentNode, char* arg) {
    // TODO
}


void mkdir(TreeNode* currentNode, char* folderName) {
    // TODO
}


void rmrec(TreeNode* currentNode, char* resourceName) {
    // TODO
}


void rm(TreeNode* currentNode, char* fileName) {
    // TODO
}


void rmdir(TreeNode* currentNode, char* folderName) {
    // TODO
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
    // TODO
}


void cp(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

void mv(TreeNode* currentNode, char* source, char* destination) {
    // TODO
}

