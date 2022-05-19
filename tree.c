#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define TREE_CMD_INDENT_SIZE 4
#define NO_ARG ""
#define PARENT_DIR ".."

ListNode *searchForFile(TreeNode *currentNode, char *fileName)
{
    FolderContent *content = currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    while (curr != NULL)
    {
        //    printf("\n\nletsgo: %s %s %s %d\n\n\n\n", currentNode->name, curr->info->name, fileName, strcmp(curr->info->name, fileName) == 0);

        if (strcmp(curr->info->name, fileName) == 0)
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
    new_directory->name = name;
    new_directory->type = FOLDER_NODE;
    new_directory->parent = parent;
    new_directory->content = (FolderContent *)createFolderContent();
    return new_directory;
}

FileContent *createFileContent(char *text)
{
    FileContent *file_content = (FileContent *)malloc(sizeof(FileContent));
    file_content->text = text;
    return file_content;
}

TreeNode *createFile(char *name, char *file_content, TreeNode *parent)
{
    TreeNode *new_file = (TreeNode *)malloc(sizeof(TreeNode));
    new_file->name = name;
    new_file->type = FILE_NODE;
    new_file->parent = parent;
    new_file->content = (FileContent *)createFileContent(file_content);
    return new_file;
}

void addToList(List *list, ListNode *node)
{
    node->next = list->head;
    list->head = node;
}

FileTree createFileTree(char *rootFolderName)
{

    FileTree file_system;

    TreeNode *root = createDirectory(rootFolderName, NULL);

    file_system.root = root;

    return file_system;
}

void freeTree(FileTree fileTree)
{
    TreeNode *root = fileTree.root;
    FolderContent *content = root->content;

    List *directories_list = content->children;
    cleanListRec(directories_list);
    free(directories_list);

    free(root->content);
    free(root->name);
    free(root);
}

void ls(TreeNode *currentNode, char *arg)
{

    //  printf("%d\n", arg[0] == 0); //!

    TreeNode *pastNode = currentNode;

    if (arg[0] != 0)
    {
        FolderContent *content = (FolderContent *)currentNode->content;
        List *directories_list = content->children;
        ListNode *curr = directories_list->head;

        while (curr != NULL)
        {
            if (strcmp(curr->info->name, arg) == 0)
            {
                if (curr->info->type == FILE_NODE)
                {
                    FileContent *content = curr->info->content;
                    printf("%s: %s\n", arg, content->text);
                    return;
                }
                else
                {
                    currentNode = curr->info; // To print the info in the next node
                    break;
                }
            }
            curr = curr->next;
        }

        if (curr == NULL)
        {
            printf("ls: cannot access '%s': No such file or directory\n", arg);
            return;
        }
    }

    FolderContent *content = (FolderContent *)currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    while (curr != NULL)
    {
        printf("%s\n", curr->info->name);
        curr = curr->next;
    }

    currentNode = pastNode;
}

void pwd(TreeNode *treeNode) // Done
{
    if (strcmp(treeNode->name, "root") == 0)
    {
        printf("root");
        return;
    }

    pwd(treeNode->parent);
    printf("/%s", treeNode->name);
}

TreeNode *getDirectory(TreeNode *currentNode, char *path)
{
    char *token = strtok(path, "/");

    while (token)
    {
        if (strcmp(token, "..") == 0)
        {
            if (currentNode->parent == NULL)
                return NULL;
            else
                currentNode = currentNode->parent;
        }
        else
        {
            ListNode *aux_node = searchForFile(currentNode, token);

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

TreeNode *cd(TreeNode *currentNode, char *path)
{

    char *error_handling_string = strdup(path); // Bcs we'll use strok on the main string

    TreeNode *aux = getDirectory(currentNode, path);

    if (aux == NULL || aux->type == FILE_NODE)
    {
        printf("cd: no such file or directory: %s\n", error_handling_string);
        free(error_handling_string);
        return currentNode;
    }

    free(error_handling_string);
    return aux; // Presupunand ca mereu se ajunge la un director
}

void PrintRecursively(TreeNode *currentNode, int indent, int *nr_of_folders, int *nr_of_files)
{
    for (int i = 0; i < indent; i++)
        printf("    ");

    // printf("\t");

    printf("%s\n", currentNode->name);

    if (currentNode->type == FOLDER_NODE)
    {
        (*nr_of_folders)++;

        FolderContent *content = (FolderContent *)currentNode->content;
        List *directories_list = content->children;
        ListNode *curr = directories_list->head;

        while (curr != NULL)
        {
            PrintRecursively(curr->info, indent + 1, nr_of_folders, nr_of_files);
            curr = curr->next;
        }
    }
    else
        (*nr_of_files)++;
}

void tree(TreeNode *currentNode, char *arg)
{

    char *error_handling_string = strdup(arg);
    TreeNode *aux = getDirectory(currentNode, arg);

    if (aux == NULL || aux->type == FILE_NODE)
    {
        printf("%s [error opening dir] \n\n0 directories, 0 files\n\n", error_handling_string);
        free(error_handling_string);
        return;
    }

    free(error_handling_string);

    FolderContent *content = (FolderContent *)aux->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;
    int nr_of_folders = 0;
    int nr_of_files = 0;

    while (curr != NULL)
    {
        PrintRecursively(curr->info, 0, &nr_of_folders, &nr_of_files);
        curr = curr->next;
    }

    printf("%d directories, %d files\n", nr_of_folders, nr_of_files);
}

void mkdir(TreeNode *currentNode, char *folderName)
{

    if (searchForFile(currentNode, folderName) != NULL)
    {
        printf("mkdir: cannot create directory '%s': File exists\n", folderName);
        free(folderName);
        return;
    }

    TreeNode *new_directory = createDirectory(folderName, currentNode);
    ListNode *new_node = createNode(new_directory);

    FolderContent *content = (FolderContent *)currentNode->content;

    List *directory_list = (List *)content->children;

    addToList(directory_list, new_node);
}

void cleanNode(ListNode *node)
{
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
    ListNode *curr = list->head;
    ListNode *prev;

    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
        if (prev->info->type == FILE_NODE)
            cleanNode(prev);
        else
        {
            FolderContent *content = (FolderContent *)prev->info->content;
            List *directory_list = (List *)content->children;

            if (directory_list->head != NULL)
                cleanListRec(directory_list);

            cleanNode(prev);
        }
    }
}

void rmrec(TreeNode *currentNode, char *resourceName)
{

    ListNode *aux_node = searchForFile(currentNode, resourceName);

    if (aux_node == NULL)
    {
        printf("rmrec: failed to remove '%s': No such file or directory\n", resourceName);
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

void removeNodeFromList(TreeNode *currentNode, char *folderName)
{
    FolderContent *content = (FolderContent *)currentNode->content;
    List *directories_list = content->children;
    ListNode *curr = directories_list->head;

    if (strcmp(curr->info->name, folderName) == 0)
    {
        directories_list->head = curr->next;

        cleanNode(curr);
        return;
    }

    while (curr->next != NULL)
    {
        if (strcmp(curr->next->info->name, folderName) == 0)
        {
            ListNode *aux = curr->next;
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

    // printf("\n\n%s %s\n\n\n", currentNode->name, fileName);

    if (aux == NULL)
    {
        printf("rm: failed to remove '%s': No such file or directory\n", fileName);
        return;
    }
    if (aux->info->type == FOLDER_NODE)
    {
        printf("rm: cannot remove '%s': Is a directory", fileName);
        return;
    }

    removeNodeFromList(currentNode, fileName);
}

void rmdir(TreeNode *currentNode, char *folderName)
{
    ListNode *aux = searchForFile(currentNode, folderName);

    if (aux == NULL)
    {
        printf("rmdir: failed to remove '%s': No such file or directory\n", folderName);
        return;
    }

    if (aux->info->type == FILE_NODE)
    {
        printf("rmdir: failed to remove '%s': Not a directory\n", folderName);
        return;
    }

    FolderContent *content = (FolderContent *)aux->info->content;

    if (content->children->head != NULL)
    {
        printf("rmdir: failed to remove '%s': Directory not empty\n", folderName);
        return;
    }

    removeNodeFromList(currentNode, folderName);
}

void touch(TreeNode *currentNode, char *fileName, char *fileContent)
{
    if (searchForFile(currentNode, fileName) != NULL)
    {
        free(fileName);
        free(fileContent);
        return;
    }

    TreeNode *new_file = createFile(fileName, fileContent, currentNode);
    ListNode *new_node = createNode(new_file);

    FolderContent *content = (FolderContent *)currentNode->content;

    List *directory_list = (List *)content->children;

    addToList(directory_list, new_node);
}

void reverse_string(char *string, int elements)
{

    for (int i = 0; i < elements / 2; i++)
    {
        int aux = string[i];
        string[i] = string[elements - i - 1];
        string[elements - i - 1] = aux;
    }
}

char *get_string_name(char *path)
{
    int pos = strlen(path) - 1;

    char *new_string = malloc(300);
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

void cp(TreeNode *currentNode, char *source, char *destination)
{

    char *aux_destination = strdup(destination);
    char *aux_source = strdup(source);
    TreeNode *source_file = getDirectory(currentNode, aux_source);
    TreeNode *destination_directory = getDirectory(currentNode, aux_destination);

    free(aux_destination);
    free(aux_source);

    if (source_file == destination_directory)
        return;

    if (source_file->type == FOLDER_NODE)
    {
        printf("cp: -r not specified; omitting directory '%s'\n", source);
        return;
    }

    if (destination_directory == NULL)
    {
        char *save_destination = strdup(destination);

        char *create_file_name = get_string_name(destination);

        TreeNode *dir_for_named_file = getDirectory(currentNode, destination); // What's left from the destination

        if (dir_for_named_file == NULL)
        {
            printf("cp: failed to access '%s': Not a directory\n", save_destination);
            free(save_destination);
            free(create_file_name);
            return;
        }

        FolderContent *content = (FolderContent *)dir_for_named_file->content;
        TreeNode *new_file = createFile(create_file_name, strdup(((FileContent *)(source_file->content))->text), destination_directory);
        ListNode *new_node = createNode(new_file);

        addToList(content->children, new_node);

        free(save_destination);
        return;
    }

    if (destination_directory->type == FILE_NODE)
    {
        // Only copy the text
        // Must keep in mind that copied text, if inexistent is a string with 0 on its first position
        free(((FileContent *)(destination_directory->content))->text);
        ((FileContent *)(destination_directory->content))->text = strdup(((FileContent *)(source_file->content))->text);
        return;
    }
    else
    {
        FolderContent *content = (FolderContent *)destination_directory->content;
        TreeNode *new_file = createFile(strdup(source_file->name), strdup(((FileContent *)(source_file->content))->text), destination_directory);
        ListNode *new_node = createNode(new_file);

        addToList(content->children, new_node);
    }
}

void mv(TreeNode *currentNode, char *source, char *destination)
{
    char *aux_destination = strdup(destination);
    char *aux_source = strdup(source);
    TreeNode *source_file = getDirectory(currentNode, aux_source);
    TreeNode *destination_directory = getDirectory(currentNode, aux_destination);

    free(aux_destination);
    free(aux_source);

    if (source_file == destination_directory)
        return;

    ListNode *isok = searchForFile(source_file->parent, source_file->name);

    if (destination_directory == NULL)
    {
        char *save_destination = strdup(destination);

        char *create_file_name = get_string_name(destination);

        TreeNode *dir_for_named_file = getDirectory(currentNode, destination); // What's left from the destination

        if (dir_for_named_file == NULL)
        {
            printf("mv: failed to access '%s': Not a directory\n", save_destination); // Nu mentioneza ce fac in this case
            free(save_destination);
            free(create_file_name);
            return;
        }
        free(save_destination);

        FolderContent *content = (FolderContent *)dir_for_named_file->content;
        // TreeNode *new_file = createFile(create_file_name, strdup(((FileContent *)(source_file->content))->text), destination_directory);
        // ListNode *new_node = createNode(source_file);

        // addToList(content->children,  new_node);

        TreeNode *parent = source_file->parent;
        FolderContent *parent_content = parent->content;
        List *parent_list = parent_content->children;

        if (strcmp(parent_list->head->info->name, source_file->name) == 0)
        {
            ListNode *aux_node = parent_list->head;
            parent_list->head = parent_list->head->next;

            free(aux_node->info->name);
            aux_node->info->name = create_file_name;

            addToList(content->children, aux_node);

            return;
        }

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

    if (destination_directory->type == FILE_NODE)
    {
        char *source_text = ((FileContent *)source_file->content)->text;

        free(((FileContent *)destination_directory->content)->text);

        ((FileContent *)destination_directory->content)->text = strdup(source_text);

        rm(source_file->parent, source_file->name);

        return;
    }

    FolderContent *destination_content = (FolderContent *)destination_directory->content;
    List *destination_list = destination_content->children;

    TreeNode *parent = source_file->parent;

    FolderContent *content_parent = (FolderContent *)parent->content;
    List *directories_list = content_parent->children;
    ListNode *curr = directories_list->head;

    if (strcmp(curr->info->name, source_file->name) == 0)
    {
        directories_list->head = curr->next;
        curr->next = NULL;

        addToList(destination_list, curr);

        return;
    }

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
