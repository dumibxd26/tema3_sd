#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

ListNode *searchForFile(TreeNode* currentNode, char* fileName)
{
    FolderContent *content = currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    while(curr != NULL)
    {
        if(strcmp(curr->info->name, fileName) == 0)
            return curr;

        curr = curr->next;
    }

    return NULL;
}

List *createList()
{
    List *list = (List *)malloc(sizeof(List));
    list->head = NULL;
    return list;
}

FolderContent *createFolderContent()
{
    FolderContent *folder_content = (FolderContent *)malloc(sizeof(FolderContent));
    folder_content->children = createList();
    return folder_content;
}

ListNode *createNode(void *data)
{
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    node->info = data;
    node->next = NULL;
    return node;
}

// Function used to create a directory(a tree node)
TreeNode *createDirectory(char *name, TreeNode *parent)
{
    TreeNode *new_directory = (TreeNode *)malloc(sizeof(TreeNode));
    new_directory->name = strdup(name);
    new_directory->type = FOLDER_NODE;
    new_directory->parent = parent;
    new_directory->content = (FolderContent *)createFolderContent();
    return new_directory;
}

FileContent *createFileContent(char *text)
{
    FileContent *file_content = (FileContent *)malloc(sizeof(FileContent));
    file_content->text = strdup(text);
    return file_content;
}

TreeNode *createFile(char *name, char *file_content, TreeNode *parent)
{
    TreeNode *new_file = (TreeNode *)malloc(sizeof(TreeNode));
    new_file->name = strdup(name);
    new_file->type = FILE_NODE;
    new_file->parent = parent;
    new_file->content = (FileContent *)createFileContent(file_content);
    return new_file;
}

void addToList(List *list, ListNode *node)
{
    if(list->head == NULL)
    {
        list->head = node;
        return;
    }

    ListNode *curr = list->head;
    while(curr->next != NULL)
        curr = curr->next;

    curr->next = node;
}

FileTree createFileTree(char* rootFolderName) {
    
    FileTree file_system;

    TreeNode *root = createDirectory(rootFolderName, NULL);
    free(rootFolderName); // Ptc il dau cu strdup in main

    file_system.root = root;

    return file_system;
}

void freeTree(FileTree fileTree) 
{
    TreeNode *root = fileTree.root;
    FolderContent *content = root->content;

    List *directories_list = content->children;
    cleanListRec(directories_list);

    free(directories_list); // poate modific in cleanlistrec
    free(root->content);
    free(root->name);
    free(root);
}

void ls(TreeNode* currentNode, char* arg) {
    
    if(arg != NULL)
    {
        FolderContent *content = (FolderContent *)currentNode->content;
        List *directories_list = content->children;
        ListNode *curr = directories_list->head;

        while(curr != NULL)
        {
            if(strcmp(curr->info->name, arg) == 0)
            {
                if(curr->info->type == FILE_NODE)
                {
                    FileContent *content = curr->info->content;
                    printf("%s: %s\n", arg, content->text);
                    return;
                }
                else
                {
                    currentNode = curr->info;  // To print the info in the next node
                    break;
                }
            }
            curr = curr->next;
        }
    }

    FolderContent *content = (FolderContent *)currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    while(curr != NULL)
    {
        printf("%s\n", curr->info->name);
        curr = curr->next;
    }

}


void pwd(TreeNode* treeNode)  // Done
{
    if(strcmp(treeNode->name, "root") == 0)
    {
        printf("root");
        return;
    }

    pwd(treeNode->parent);
    printf("/%s", treeNode->name);
}

TreeNode *getDirectory(TreeNode* currentNode, char* path) 
{

    char *token = strtok(path, "/");

    while(token)
    {
        if(strcmp(token, "..") == 0)
        {
            if(currentNode->parent == NULL)
                return NULL;
            else
                currentNode = currentNode->parent;
        }
        else
        {
            ListNode *aux_node = searchForFile(currentNode, token);

            if(aux_node == NULL)
                return NULL;
            else
            {
                TreeNode *aux = aux_node->info;
                currentNode = aux;
            }
        }
        token = strtok(NULL, "/");
    }

    return currentNode;
}

TreeNode* cd(TreeNode* currentNode, char* path) {
    
    char *error_handling_string = strdup(path);

    TreeNode *aux = getDirectory(currentNode, path);

    if(aux == NULL)
    {
        printf("cd: no such file or directory: %s\n", error_handling_string); // Bcs we strtok it
        return currentNode;
    }

    return aux;  // Presupunand ca mereu se ajunge la un director

}

void PrintRecursively(TreeNode* currentNode, int indent)
{
    for(int i = 0; i < indent; i++)
        printf("\t");

    printf("%s\n", currentNode->name);

    if(currentNode->type == FOLDER_NODE)
    {
        FolderContent *content = (FolderContent *)currentNode->content;
        List *directories_list = content->children;
        ListNode *curr = directories_list->head;

        while(curr != NULL)
        {   
            PrintRecursively(curr->info, indent + 1);
            curr = curr->next;
        }
    }
}

void tree(TreeNode* currentNode, char* arg) {
    
    char *error_handling_string = strdup(arg);

    TreeNode *aux = getDirectory(currentNode, arg);

    if(aux == NULL || aux->type == FILE_NODE)
    {
        printf("%s [error opening dir] \n\n0 directories, 0 files\n\n", error_handling_string);
        return;
    }
    
    PrintRecursively(aux, 0);
}

void mkdir(TreeNode* currentNode, char* folderName) {

    if(searchForFile(currentNode, folderName) != NULL)
    {
        printf("mkdir: cannot create directory '%s': File exists\n", folderName);
        return;
    }

    TreeNode *new_directory = createDirectory(folderName, currentNode);
    ListNode *new_node = createNode(new_directory);

    FolderContent *content = (FolderContent *)currentNode->content;

    List* directory_list = (List *)content->children;

    addToList(directory_list, new_node);
}

void cleanNode(ListNode *node)
{
    if(node->info->type == FOLDER_NODE)
    {
        FolderContent *content = (FolderContent *)node->info->content;
        List* directory_list = (List *)content->children;
        free(directory_list);
        free(content);
    }
    else if(node->info->type == FILE_NODE)
    {
        FileContent *content = (FileContent *)node->info->content;
        free(content->text);
        free(content);
    }

    free(node->info->name);
    free(node->info);
    free(node);
}

void cleanListRec(List* list)
{
    ListNode *curr = list->head;
    ListNode *prev;

    while(curr != NULL)
    {
        prev = curr;
        curr = curr->next;
        if(prev->info->type == FILE_NODE)
            cleanNode(prev);
        else
        {
            FolderContent *content = (FolderContent *)prev->info->content;
            List* directory_list = (List *)content->children;
            
            if(directory_list->head != NULL)
                cleanListRec(directory_list);
            
            cleanNode(prev);
        }
    }
}

void rmrec(TreeNode* currentNode, char* resourceName)
{
    
    ListNode *aux_node = searchForFile(currentNode, resourceName);

    if(aux_node == NULL)
    {
        printf("rmrec: failed to remove %s: No such file or directory\n", resourceName);
        return;
    }

    FolderContent *content = (FolderContent *)aux_node->info->content;
  
    cleanListRec(content->children);

    // Remove the node from its position

    FolderContent *content_parent = (FolderContent *)currentNode->content;

    List *directories_list = content_parent->children;
    ListNode *curr = directories_list->head;
    ListNode *aux = NULL;

    if (strcmp(directories_list->head->info->name, resourceName) == 0)
    {
        directories_list->head = directories_list->head->next;
        cleanNode(curr);
        return;
    }

    while(curr->next != NULL)
    {
        if(strcmp(curr->next->info->name, resourceName) == 0)
        {
            aux = curr->next;
            curr->next = curr->next->next;
            cleanNode(aux);
            return;
        }
      curr = curr->next;
    }
}

void removeNodeFromList(TreeNode* currentNode, char* folderName) 
{
    FolderContent *content = (FolderContent *)currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    if(strcmp(curr->info->name, folderName) == 0)
    {
        directories_list->head = curr->next;

        cleanNode(curr);
        return;
    }

    while(curr->next != NULL)
    {
        if(strcmp(curr->next->info->name, folderName) == 0)
        {
            ListNode *aux = curr->next;
            curr->next = curr->next->next;

            cleanNode(aux);
            return;
        }
       curr = curr->next;
    }

}

void rm(TreeNode* currentNode, char* fileName) {
    
    char *aux_filename = strdup(fileName);
    ListNode *aux = searchForFile(currentNode, aux_filename);

    if(aux == NULL)
    {
        printf("rm: failed to remove %s: No such file or directory.\n", fileName);
        return;
    }
    if(aux->info->type == FOLDER_NODE)
    {
        printf("rm: cannot remove %s: Is a directory.", fileName);
        return;
    }

    removeNodeFromList(currentNode, fileName);
}


void rmdir(TreeNode* currentNode, char* folderName)
{
    char *aux_foldername = strdup(folderName);
    ListNode *aux = searchForFile(currentNode, aux_foldername);

    if(aux == NULL)
    {
        printf("rmdir: failed to remove %s: No such file or directory.\n", folderName);
        return;
    }
    
    if(aux->info->type == FILE_NODE)
    {
        printf("rmdir: failed to remove %s: Not a directory.\n", folderName);
        return;
    }

    FolderContent *content = (FolderContent *)aux->info->content;

    if(content->children->head != NULL)
    {
        printf("rmdir: failed to remove %s: Directory not empty.\n", folderName);
        return;
    }

    removeNodeFromList(currentNode, folderName);
}


void touch(TreeNode* currentNode, char* fileName, char* fileContent) {
    
    TreeNode *new_file = createFile(fileName, fileContent, currentNode);
    ListNode *new_node = createNode(new_file);

    FolderContent *content = (FolderContent *)currentNode->content;

    List* directory_list = (List *)content->children;

    addToList(directory_list, new_node);
}


void cp(TreeNode* currentNode, char* source, char* destination) {
    
    char *aux_destination = strdup(destination);
    char *aux_source = strdup(source);
    TreeNode *source_file = getDirectory(currentNode, aux_source);
    TreeNode *destination_directory = getDirectory(currentNode, aux_destination);

    if(source_file->type == FOLDER_NODE)
    {
        printf("cp: -r not specified; omitting directory %s>\n", source);
        return;
    }

    if(destination_directory == NULL)
    {
        printf("cp: failed to access %s: Not a directory\n", source);
        return;
    }

    if(destination_directory->type == FILE_NODE)
    {
        free(destination_directory->content);
        destination_directory->content = strdup(source_file->content);
        return;
    }
    else
    {
        FolderContent *content = (FolderContent *)destination_directory->content;
        TreeNode *new_file = createFile(source, source_file->content, destination_directory);
        ListNode *new_node = createNode(new_file);

       addToList(content->children, new_node);
    }

}

void mv(TreeNode* currentNode, char* source, char* destination)
{
    char *aux_destination = strdup(destination);
    char *aux_source = strdup(source);
    TreeNode *source_file = getDirectory(currentNode, aux_source);
    TreeNode *destination_directory = getDirectory(currentNode, aux_destination);

    if(destination_directory == NULL)
    {
        printf("mv: failed to access %s: Not a directory\n", source);  // Nu mentioneza ce fac in this case
        return;
    }

    // FolderContent *content = (FolderContent *)destination_directory->content;
    // TreeNode *new_file = createFile(source, source_file->content, destination_directory);
    // ListNode *new_node = createNode(new_file);

    // addToList(content->children, new_node);
      
    FolderContent *destination_content = (FolderContent *)destination_directory->content;
    List *destination_list = destination_content->children;

    TreeNode *parent = source_file->parent;

    FolderContent *content_parent = (FolderContent *)parent->content;
    List *directories_list = content_parent->children;
    ListNode *curr = directories_list->head;

    if(strcmp(curr->info->name, source) == 0)
    {
        directories_list->head = curr->next;
            
        addToList(destination_list, curr);
        return;
    }
    
    while(curr->next != NULL)
    {
        if(strcmp(curr->next->info->name, source) == 0)
        {
            ListNode *aux = curr->next;
            curr->next = curr->next->next;
            addToList(destination_list, aux);
            return;
        }
        curr = curr->next;
    }

}

