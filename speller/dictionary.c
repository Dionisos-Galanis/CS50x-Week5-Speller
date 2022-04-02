// Implements a dictionary's functionality
#include <cs50.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

// Number of words, loaded
unsigned int LOADED;

// Debug variable - counts the number of malloced nodes and decreased when freed
int NMALLOCED = 0;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
const unsigned int N = 100000;

// Hash table
node *table[N];

// Define a function for searching in a singly-linked list
bool checklist(const char *word, node *list);

bool checklist(const char *word, node *list)
{
    if (strcmp(list->word, word) == 0)
    {
        return true;
    }
    else if (list->next == NULL)
    {
        return false;
    }
    else
    {
        return checklist(word, list->next);
    }
}

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // Convert word to lowercase
    int wlen = strlen(word) + 1;
    char lword[wlen];
    for (int i = 0; i < wlen; i++)
    {
        lword[i] = (char) tolower(word[i]);
    }

    // Calculate a hash
    unsigned int n = hash(lword);

    if (table[n] != NULL)
    {
        // Check the linked list in the n-th bucket
        return checklist(lword, table[n]);
    }
    else
    {
        return false;
    }
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    unsigned const char *us;
    unsigned int h;
    unsigned int mult = 97;

    h = 0;

    for (us = (unsigned const char *) word; *us; us++)
    {
        h = h * mult + *us;
    }

    return h % N;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    LOADED = 0;
    // Open the dictionary file for reading
    FILE *dictfile = fopen(dictionary, "r");
    if (dictfile == NULL)
    {
        return false;
    }

    char curWord[LENGTH + 1];
    char curChar;
    int i = 0;
    unsigned int n;
    node *pointNode;

    // Cycle till the EOF or error
    while (fread(&curChar, sizeof(char), 1, dictfile))
    {
        if (curChar != '\n')
        {
            curWord[i] = curChar;
            i++;
        }
        else // End of word
        {
            curWord[i] = '\0';
            i = 0;
            n = hash(curWord);
            LOADED++;
            // Create a node
            pointNode = malloc(sizeof(node));
            if (pointNode == NULL)
            {
                LOADED = 0;
                unload();
                fclose(dictfile);
                return false;
            }
            NMALLOCED++;
            // Set the word and the next to point to the current first node
            strcpy(pointNode->word, curWord);
            pointNode->next = table[n];
            // Set pointer in the table
            table[n] = pointNode;
        }
    }
    if (ferror(dictfile) != 0) // Error reading file
    {
        LOADED = 0;
        unload();
        fclose(dictfile);
        return false;
    }

    fclose(dictfile);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return LOADED;
}

// Declare and implement the clearlist function
void clearlist(node *ptr);

void clearlist(node *ptr)
{
    if (ptr->next == NULL)
    {
        free(ptr);
        NMALLOCED = NMALLOCED - 1;
    }
    else
    {
        clearlist(ptr->next);
    }
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    if (LOADED > 0)
    {
        // Cycling through the table
        for (int i = 0; i < N; i++)
        {
            if (table[i] != NULL)
            {
                clearlist(table[i]);
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}
