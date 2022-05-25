#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."
#define MAX_COMMAND 300
#ifndef __UTILS_H_
#define __UTILS_H_

/* useful macro for handling error codes */
#define DIE(assertion, call_description)  \
    do                                    \
    {                                     \
        if (assertion)                    \
        {                                 \
            fprintf(stderr, "(%s, %d): ", \
                    __FILE__, __LINE__);  \
            perror(call_description);     \
            exit(errno);                  \
        }                                 \
    } while (0)

#endif /* __UTILS_H_ */

// [START] Folder functions
FolderContent *createFolderContent()
{
    // Alloc memory for folder list payload
    FolderContent *folder_content =
        (FolderContent *)malloc(sizeof(FolderContent));
    DIE(!folder_content, "Cant alloc memory.\n");
    // Create the list of contents itself
    folder_content->children = createList();

    return folder_content;
}
// [END] Folder functions

// [START] Tree  functions
TreeNode *createTreeNode(char *name, TreeNode *parent, char *file_content)
{
    // Function used to create nodes for folders and also for files
    // file_content is NULL for folders

    // Alloc memory for the new tree node we create
    TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
    DIE(!new_node, "Cant alloc memory.\n");

    // Fill the fields of the new node
    new_node->name = name;
    // Decide on the type of node we create
    if (!file_content)
        new_node->type = FOLDER_NODE;
    else
        new_node->type = FILE_NODE;
    new_node->parent = parent;
    // Decide on the type of node we create
    if (!file_content)
        new_node->content = (FolderContent *)createFolderContent();
    else
        new_node->content = (FileContent *)createFileContent(file_content);

    return new_node;
}
// [END] Tree functions

// [START] File operator functions
FileContent *createFileContent(char *text)
{
    FileContent *file_content = (FileContent *)malloc(sizeof(FileContent));
    DIE(!file_content, "Cant alloc memory.\n");

    file_content->text = text;
    return file_content;
}

FileTree createFileTree(char *rootFolderName)
{
    // Function to establish fyle system root

    TreeNode *root = createTreeNode(rootFolderName, NULL, NULL);

    FileTree file_system;
    file_system.root = root;

    return file_system;
}
// [END] File functions

// [START] List functions
List *createList()
{
    // Simple function to create a list
    List *list = (List *)malloc(sizeof(List));
    DIE(!list, "Cant alloc memory.\n");

    list->head = NULL;
    return list;
}

void addToList(List *list, ListNode *node)
{
    // Simple function to add first in a list
    node->next = list->head;
    list->head = node;
}

ListNode *createNode(void *data)
{
    // Simple function to alloc memory for a new node in a list
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    DIE(!node, "Cant alloc memory.\n");

    node->info = data;
    node->next = NULL;

    return node;
}

void removeNodeFromList(TreeNode *currentNode, char *folderName)
{
    // Simple function to remove a file in a list of files
    FolderContent *content = (FolderContent *)currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    if (strcmp(curr->info->name, folderName) == 0)
    {
        // if we find the file we are looking for as first folder we delete it
        // then exit from function
        directories_list->head = curr->next;
        cleanNode(curr);

        return;
    }

    while (curr->next != NULL)
    {
        if (strcmp(curr->next->info->name, folderName) == 0)
        {
            // We delete a folder with the name given if we find it after
            ListNode *aux = curr->next;
            curr->next = curr->next->next;
            cleanNode(aux);

            return;
        }
        curr = curr->next;
    }
}
// [END] List functions

// [START] Search functions
TreeNode *getDirectory(TreeNode *currentNode, char *path)
{
    // Separate input as linux standard
    char *token = strtok(path, "/");

    while (token)
    {
        // Go back case
        if (strcmp(token, "..") == 0)
        {
            if (currentNode->parent == NULL)
                return NULL;
            else
                currentNode = currentNode->parent;
        }
        else
        {
            // Search for the file name
            ListNode *aux_node = searchForFile(currentNode, token);

            // If we find it we get the pointer for return afterwards
            if (aux_node == NULL)
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

ListNode *searchForFile(TreeNode *currentNode, char *fileName)
{
    FolderContent *content = currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    // Search iteratively in between files in a directory
    while (curr != NULL)
    {
        if (strcmp(curr->info->name, fileName) == 0)
            return curr;
        curr = curr->next;
    }

    return NULL;
}
// [END] Search functions

// [START] Commands
void ls(TreeNode *currentNode, char *arg)
{
    if (arg[0] != 0)
    {
        FolderContent *content = (FolderContent *)currentNode->content;
        List *directories_list = content->children;
        ListNode *curr = directories_list->head;

        // Loop through directories
        while (curr != NULL)
        {
            if (strcmp(curr->info->name, arg) == 0)
            {
                // ls case for file just print its content
                if (curr->info->type == FILE_NODE)
                {
                    FileContent *content = curr->info->content;
                    printf("%s: %s\n", arg, content->text);
                    return;
                }
                else
                {
                    // It's directory
                    // To print the info in the next node
                    currentNode = curr->info;
                    break;
                }
            }
            curr = curr->next;
        }

        // Error handling
        if (curr == NULL)
        {
            printf("ls: cannot access '%s': No such file or directory\n", arg);
            return;
        }
    }

    // Print each files name
    FolderContent *content = (FolderContent *)currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    while (curr != NULL)
    {
        printf("%s\n", curr->info->name);
        curr = curr->next;
    }
}

void pwd(TreeNode *treeNode)
{
    // Root case for pwd
    if (strcmp(treeNode->name, "root") == 0)
    {
        printf("root");
        return;
    }

    // Pwd case for other files
    pwd(treeNode->parent);
    printf("/%s", treeNode->name);
}

TreeNode *cd(TreeNode *currentNode, char *path)
{
    // We will use strok on the main string so we duplicate memory first
    char *error_handling_string = strdup(path);

    // Get directory and loop through it
    TreeNode *aux = getDirectory(currentNode, path);

    // If dir doesnt exist or its a file
    if (aux == NULL || aux->type == FILE_NODE)
    {
        printf("cd: no such file or directory: %s\n", error_handling_string);
        free(error_handling_string);
        return currentNode;
    }

    free(error_handling_string);
    // Return directory we found
    return aux;
}

void mkdir(TreeNode *currentNode, char *folderName)
{
    // Case for directory which already exists
    if (searchForFile(currentNode, folderName) != NULL)
    {
        printf("mkdir: cannot create directory '%s': File exists\n",
               folderName);
        free(folderName);
        return;
    }

    // Else create the new directory and its content
    TreeNode *new_directory = createTreeNode(folderName,
                                             currentNode, NULL);
    ListNode *new_node = createNode(new_directory);

    FolderContent *content = (FolderContent *)currentNode->content;
    List *directory_list = (List *)content->children;

    // Add the new node to the directories list
    addToList(directory_list, new_node);
}

void tree(TreeNode *currentNode, char *arg)
{
    // Duplicate memory because arg will be modified
    char *error_handling_string = strdup(arg);
    // Search for the desired directory
    TreeNode *aux = getDirectory(currentNode, arg);

    // Error for non conform type
    if (aux == NULL || aux->type == FILE_NODE)
    {
        printf("%s [error opening dir] \n\n0 directories, 0 files\n\n",
               error_handling_string);
        free(error_handling_string);
        return;
    }
    free(error_handling_string);

    FolderContent *content = (FolderContent *)aux->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;
    int nr_of_folders = 0;
    int nr_of_files = 0;

    /*
        If in dir1 we have dir2 dir3 dir4
        Tree prints:
        dir2
        dir3
        dir4

        we exclude the root of the tree
    */

    while (curr != NULL)
    {
        // Print in arborescent order
        PrintRecursively(curr->info, 0, &nr_of_folders, &nr_of_files);
        curr = curr->next;
    }
    // Print statistics
    printf("%d directories, %d files\n", nr_of_folders, nr_of_files);
}

void rmrec(TreeNode *currentNode, char *resourceName)
{
    // Search for the file we wand to delete
    ListNode *aux_node = searchForFile(currentNode, resourceName);
    // Error for not found file
    if (aux_node == NULL)
    {
        printf("rmrec: failed to remove '%s': No such file or directory\n",
               resourceName);
        return;
    }
    FolderContent *content = (FolderContent *)aux_node->info->content;

    // Clean the content
    cleanListRec(content->children);

    // Remove the node from its position
    FolderContent *content_parent = (FolderContent *)currentNode->content;

    List *directories_list = content_parent->children;
    ListNode *curr = directories_list->head;
    ListNode *aux = NULL;

    // Remove content from head of list directory
    if (strcmp(directories_list->head->info->name, resourceName) == 0)
    {
        directories_list->head = directories_list->head->next;
        cleanNode(curr);
        return;
    }
    // Remove the other nodes
    while (curr->next != NULL)
    {
        if (strcmp(curr->next->info->name, resourceName) == 0)
        {
            aux = curr->next;
            curr->next = curr->next->next;
            cleanNode(aux);
            return;
        }
        curr = curr->next;
    }
}

void rm(TreeNode *currentNode, char *fileName)
{
    ListNode *aux = searchForFile(currentNode, fileName);
    // Error handling for non existing or non conform type
    if (aux == NULL)
    {
        printf("rm: failed to remove '%s': No such file or directory\n",
               fileName);
        return;
    }
    if (aux->info->type == FOLDER_NODE)
    {
        printf("rm: cannot remove '%s': Is a directory", fileName);
        return;
    }
    // Remove the node from the list of files
    removeNodeFromList(currentNode, fileName);
}

void rmdir(TreeNode *currentNode, char *folderName)
{
    ListNode *aux = searchForFile(currentNode, folderName);
    // Error handling for non conform type or non exting folder
    if (aux == NULL)
    {
        printf("rmdir: failed to remove '%s': No such file or directory\n",
               folderName);
        return;
    }

    if (aux->info->type == FILE_NODE)
    {
        printf("rmdir: failed to remove '%s': Not a directory\n",
               folderName);
        return;
    }

    FolderContent *content = (FolderContent *)aux->info->content;

    if (content->children->head != NULL)
    {
        printf("rmdir: failed to remove '%s': Directory not empty\n",
               folderName);
        return;
    }

    // Remove node from the list of directories
    removeNodeFromList(currentNode, folderName);
}

void touch(TreeNode *currentNode, char *fileName, char *fileContent)
{
    // Exit if file already exists
    if (searchForFile(currentNode, fileName) != NULL)
    {
        free(fileName);
        free(fileContent);
        return;
    }

    // Create the new file
    TreeNode *new_file = createTreeNode(fileName, currentNode, fileContent);

    // Create new additional node
    ListNode *new_node = createNode(new_file);
    FolderContent *content = (FolderContent *)currentNode->content;

    // Get the content from dir and add to the list the new file
    List *directory_list = (List *)content->children;
    addToList(directory_list, new_node);
}

void cp(TreeNode *currentNode, char *source, char *destination)
{
    // Duplicate strings we will modify
    char *aux_destination = strdup(destination);
    char *aux_source = strdup(source);

    // Get directories
    TreeNode *source_file = getDirectory(currentNode, aux_source);
    TreeNode *destination_directory = getDirectory(currentNode,
                                                   aux_destination);

    // Free the duplicated memory
    free(aux_destination);
    free(aux_source);

    // Case for cp in the same dir
    if (source_file == destination_directory)
        return;

    // Error for directory found not being a directory
    if (source_file->type == FOLDER_NODE)
    {
        printf("cp: -r not specified; omitting directory '%s'\n",
               source);
        return;
    }

    // If destination is NULL we create the directory untill last step of path
    // and put there the new file
    if (destination_directory == NULL)
    {
        // Make a copy of destination
        char *save_destination = strdup(destination);
        // Separate path
        char *create_file_name = get_string_name(destination);

        // Separate destination
        // What's left from the destination
        TreeNode *dir_for_named_file = getDirectory(currentNode, destination);

        // Case for non existing dir
        if (dir_for_named_file == NULL)
        {
            printf("cp: failed to access '%s': Not a directory\n",
                   save_destination);
            free(save_destination);
            free(create_file_name);
            return;
        }

        // Create the new file in the path
        char *copy = strdup(((FileContent *)(source_file->content))->text);
        FolderContent *content = (FolderContent *)dir_for_named_file->content;
        TreeNode *new_file = createTreeNode(create_file_name,
                                            destination_directory, copy);
        ListNode *new_node = createNode(new_file);

        // Add to the content list the new node
        addToList(content->children, new_node);

        free(save_destination);
        return;
    }

    if (destination_directory->type == FILE_NODE)
    {
        // Only copy the text
        // Must keep in mind that copied text, if inexistent is a string
        // with 0 on its first position
        free(((FileContent *)(destination_directory->content))->text);
        char *copy = strdup(((FileContent *)(source_file->content))->text);
        ((FileContent *)(destination_directory->content))->text = copy;
        return;
    }
    else
    {
        FolderContent *tmp = (FolderContent *)destination_directory->content;
        FolderContent *content = tmp;
        char *copy = strdup(((FileContent *)(source_file->content))->text);
        TreeNode *new_file = createTreeNode(strdup(source_file->name),
                                            destination_directory, copy);
        ListNode *new_node = createNode(new_file);

        addToList(content->children, new_node);
    }
}

void mv(TreeNode *currentNode, char *source, char *destination)
{
    // Creating a copy for strings that will be destroyed
    char *aux_destination = strdup(destination);
    char *aux_source = strdup(source);

    // Searching for directories with the names given
    TreeNode *source_file = getDirectory(currentNode, aux_source);
    TreeNode *destination_directory = getDirectory(currentNode,
                                                   aux_destination);

    free(aux_destination);
    free(aux_source);

    // Case for mv in the same directory
    if (source_file == destination_directory)
        return;

    // Case for non existing destination
    if (destination_directory == NULL)
    {
        // Make a copy of the destination
        char *save_destination = strdup(destination);
        // Get the last part of the destination
        char *create_file_name = get_string_name(destination);
        // Get what is left from the destination
        TreeNode *dir_for_named_file = getDirectory(currentNode, destination);

        // If it source dir is NULL
        if (dir_for_named_file == NULL)
        {
            printf("mv: failed to access '%s': Not a directory\n",
                   save_destination);
            free(save_destination);
            free(create_file_name);
            return;
        }
        free(save_destination);

        // Get content pointer
        FolderContent *content = (FolderContent *)dir_for_named_file->content;
        TreeNode *parent = source_file->parent;
        FolderContent *parent_content = parent->content;
        List *parent_list = parent_content->children;

        // If we find a file with that name on the lsit head we replace it
        if (strcmp(parent_list->head->info->name, source_file->name) == 0)
        {
            ListNode *aux_node = parent_list->head;
            parent_list->head = parent_list->head->next;

            free(aux_node->info->name);
            aux_node->info->name = create_file_name;

            addToList(content->children, aux_node);
            return;
        }

        // We search for the file in the rest of the list
        ListNode *curr = parent_list->head;
        while (curr->next != NULL)
        {
            if (strcmp(curr->next->info->name, source_file->name) == 0)
            {
                ListNode *aux_node = curr->next;
                curr->next = curr->next->next;

                free(aux_node->info->name);
                aux_node->info->name = create_file_name;
                addToList(content->children, aux_node);

                return;
            }
            curr = curr->next;
        }

        return;
    }

    // Case for destination last part being a file
    // Remove the file from source and replace it in destination
    if (destination_directory->type == FILE_NODE)
    {
        char *source_text = ((FileContent *)source_file->content)->text;

        free(((FileContent *)destination_directory->content)->text);

        ((FileContent *)destination_directory->content)->text =
            strdup(source_text);

        rm(source_file->parent, source_file->name);

        return;
    }

    // Basic case
    FolderContent *tmp = (FolderContent *)destination_directory->content;
    FolderContent *destination_content = tmp;
    List *destination_list = destination_content->children;
    TreeNode *parent = source_file->parent;
    FolderContent *content_parent = (FolderContent *)parent->content;
    List *directories_list = content_parent->children;
    ListNode *curr = directories_list->head;

    // If we find name as head file we move there
    if (strcmp(curr->info->name, source_file->name) == 0)
    {
        directories_list->head = curr->next;
        curr->next = NULL;

        addToList(destination_list, curr);

        return;
    }

    // Search for the file to replace in the rest of the list
    while (curr->next != NULL)
    {
        if (strcmp(curr->next->info->name, source_file->name) == 0)
        {
            ListNode *aux = curr->next;
            curr->next = curr->next->next;

            aux->next = NULL;

            addToList(destination_list, aux);

            return;
        }
        curr = curr->next;
    }
}

void PrintRecursively(TreeNode *currentNode, int indent, int *nr_of_folders,
                      int *nr_of_files)
{
    // Helper function to print in tree command
    for (int i = 0; i < indent; i++)
        printf("    ");
    printf("%s\n", currentNode->name);

    // Print only folders
    if (currentNode->type == FOLDER_NODE)
    {
        (*nr_of_folders)++;

        FolderContent *content = (FolderContent *)currentNode->content;
        List *directories_list = content->children;
        ListNode *curr = directories_list->head;

        while (curr != NULL)
        {
            PrintRecursively(curr->info, indent + 1, nr_of_folders,
                             nr_of_files);
            curr = curr->next;
        }
    }
    else
        (*nr_of_files)++;
}
// [END] Commands

// [START] Clean functions
void cleanNode(ListNode *node)
{
    // Function which frees a node from the list
    // Type cases
    if (node->info->type == FOLDER_NODE)
    {
        FolderContent *content = (FolderContent *)node->info->content;
        List *directory_list = (List *)content->children;
        free(directory_list);
        free(content);
    }
    else if (node->info->type == FILE_NODE)
    {
        FileContent *content = (FileContent *)node->info->content;
        free(content->text);
        free(content);
    }
    free(node->info->name);
    free(node->info);
    free(node);
}

void cleanListRec(List *list)
{
    // Function to free a list of folders
    ListNode *curr = list->head;
    ListNode *prev;

    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
        // If its a file we just free it
        if (prev->info->type == FILE_NODE)
            cleanNode(prev);
        else
        {
            // Else enter recursevely and go until we find a file
            FolderContent *content = (FolderContent *)prev->info->content;
            List *directory_list = (List *)content->children;

            if (directory_list->head != NULL)
                cleanListRec(directory_list);

            cleanNode(prev);
        }
    }
}
// [END] Clean functions

// [START] String functions
void reverse_string(char *string, int elements)
{
    // Simple function to reverse a string
    for (int i = 0; i < elements / 2; i++)
    {
        int aux = string[i];
        string[i] = string[elements - i - 1];
        string[elements - i - 1] = aux;
    }
}

char *get_string_name(char *path)
{
    // Simple function to separate path
    int pos = strlen(path) - 1;

    char *new_string = malloc(MAX_COMMAND);
    DIE(!new_string, "Cant alloc memory.\n");

    int el = 0;

    while (pos != 0 && path[pos] != '/')
    {
        new_string[el] = path[pos];
        el++;
        path[pos] = '\0';
        pos--;
    }
    if (pos)
        path[pos] = '\0';

    new_string[el] = '\0';

    reverse_string(new_string, el);

    return new_string;
}
// [END] String operator functions

// Free data structure function
void freeTree(FileTree fileTree)
{
    // Function to free the entire tree with additional files lists
    TreeNode *root = fileTree.root;
    FolderContent *content = root->content;

    List *directories_list = content->children;
    cleanListRec(directories_list);
    free(directories_list);

    free(root->content);
    free(root->name);
    free(root);
}
