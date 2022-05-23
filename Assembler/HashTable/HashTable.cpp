#include "assert.h"

#include "HashTable.h"
#include <kms/Logs.h>
#include <kms/GraphDump.h>

HashTable* HashTableCtr (size_t itemSize, unsigned long long int (*hash) (const void*), int (*cmp) (const void*, const void*))
{
    Logs.debug ("Constructing HashTable");

    if (cmp == nullptr)
    {
        Logs.error ("cmp null pointer");
        return nullptr;
    }

    if (hash == nullptr)
    {
        Logs.error ("hash null pointer");
        return nullptr;
    }


    HashTable* table = (HashTable*) calloc (1, sizeof (HashTable));
    if (table == nullptr)
    {
        Logs.error ("Callocing HashTable failed");
        return nullptr;
    }

    table -> cmp = cmp;
    table -> hash = hash;

    for (size_t i = 0; i < TABLE_LEN; i ++)\
    {
        table -> lists[i] = ListCtr (MAX_LIST_LEN, itemSize);
        if (table -> lists[i] == nullptr)
        {
            Logs.error ("Callocing array of lists failed");
        }
    }

    Logs.debug ("Hash table sucessfully constructed");
    return table;
}

void HashTableDtr (HashTable* table)
{
    Logs.debug ("Destructing HashTable");
    for (size_t i = 0; i < TABLE_LEN; i ++)
        ListDtr (table -> lists[i]);

    free (table);
    
    Logs.debug ("Hash Table successfuly destructed");
}

void* HashTableFind (HashTable* table, const void* key, bool create)
{
    Logs.debug ("Searching in HashTable");
    unsigned long long int keyHash = table -> hash (key);

    List* currentList = table -> lists[keyHash % TABLE_LEN];
    printf ("%lu\n", currentList -> free);
    int listIndex = ListFindItem (currentList, key, table -> cmp);

    if (listIndex == 0)
    {
        if (create)
        {
            char data[table -> lists[0] -> itemSize] = {};
            listIndex = ListAdd (currentList, data);
        }
        else
            return nullptr;
    }
    
    return currentList -> data + listIndex * currentList -> itemSize;
}


bool HashTableDump (HashTable* table, void (*datadump) (FILE* file, const void* item))
{
    Logs.debug ("Graphic dump begins");

    Logs.trace ("Opening file");
    Graph* graph = GraphOpen ();
    fprintf (graph -> file, "rankdir = LR\n");
    Logs.trace ("Dumping table");
    HashTableDump (table, graph -> file, datadump);
    Logs.trace ("Drawing damp");
    GraphDraw (graph, "dump.svg", "svg");

    Logs.debug ("Graphic dump ends");

    return 1;
}

bool HashTableDump (HashTable* table, FILE* file, void (*datadump) (FILE* file, const void* item))
{
    bool ok = HashTableOk (table);

    Graph graph = {};
    graph.file = file;

    //fprintf (file, "layout = fdp\n");

    GraphNode node = {};
    node.id = table;
    if (ok)
        node.color = 0x90ff90;
    else
        node.color = 0xff9090;
    node.rounded = 1;
    node.shape = RECORD_SHAPE;

    GraphBeginCluster (&graph, &node);

    node.color = 0xffffff;
    GraphAddNode (&graph, &node,
        "<lists>lists: %p|"
        "cmp: %p|"
        "hash-function: %p",
        table -> lists,
        table -> cmp,
        table -> hash);

    GraphEndCluster (&graph);

    GraphEdge edge = {};

    GraphAddEdge (&graph, &edge, table, ":lists:e", table -> lists[0], "");

    for (size_t i = 0; i < TABLE_LEN; i ++)
    {
        GraphStreamlineNodes (&graph, table, table -> lists[i]);
        
        ListDump (table -> lists[i], file, datadump);
    } 

    return ok;
}

bool HashTableOk   (HashTable* table)
{
    assert (table);

    bool ok = true;

    if (table -> cmp == nullptr)
    {
        ok = false;
        Logs.warn ("Camparator null pointer");
    }

    if (table -> hash == nullptr)
    {
        ok = false;
        Logs.warn ("Hash-function null pointer");
    }

    if (table -> lists == nullptr)
    {
        ok = false;
        Logs.warn ("Hash table lists null pointer");
    }

    for (size_t i = 0; i < TABLE_LEN; i ++)
    {
        if (table -> lists [i] == nullptr)
        {
            ok = false;
            Logs.warn ("Null pointer of list with index %u", i);
        }
        else if (!ListOk (table -> lists[0]))
        {
            ok = false;
            Logs.warn ("List with index %u isn't OK", i);
        }
    }

    if (ok)
        Logs.trace ("Hash table is OK");

    return ok;
}
