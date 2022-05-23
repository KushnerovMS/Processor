#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "List.h"
#include <kms/GraphDump.h>
#include <kms/Logs.h>

List* ListCtr (int capacity, int itemSize, ListError* err)
{
    
    if (err)
        *err = NULL_LIST_ERROR;
    

    List* list = (List*) calloc (1, sizeof (List));
    if (list == nullptr)
    {
        if (err) err -> allocationError = 1;
        return nullptr;
    }

    list -> data = (char*) calloc (capacity + 1, itemSize);
    list -> next = (int*) calloc (capacity + 1, sizeof (int)); 
    list -> prev = (int*) calloc (capacity + 1, sizeof (int)); 

    if (list -> data == nullptr ||
        list -> next == nullptr ||
        list -> prev == nullptr)
    {
        if (err) err -> allocationError = 1;
        return nullptr;
    }

    for (int i = 1; i < capacity + 1; i ++)
    {
        list -> next[i] = -(i + 1);
        list -> prev[i] = -1;
    }
    list -> next[capacity] = 0;

    list -> free = 1;
    list -> capacity = capacity;
    list -> size = 0;
    list -> itemSize = itemSize;

    list -> beginCanary = CANARY;
    list -> endCanary = CANARY;

    if (err)
        list -> err = *err;
    else
        list -> err = NULL_LIST_ERROR;

    return list;
}

ListError ListDtr (List* list)
{
    if (!ListOk (list))
        return list  -> err;

    memset (list -> data, 0, (list -> capacity + 1) * list -> itemSize);
    free (list -> data);

    memset (list -> next, 0, (list -> capacity + 1) * sizeof (int));
    free (list -> next);

    memset (list -> prev, 0, (list -> capacity + 1) * sizeof (int));
    free (list -> prev);

    ListError save = list -> err;

    list -> beginCanary = 0;
    list -> data = nullptr;
    list -> next = nullptr;
    list -> prev = nullptr;
    list -> free = 0;
    list -> capacity = -1;
    list -> itemSize = -1;
    list -> err = NULL_LIST_ERROR;
    list -> endCanary = 0;
    free (list);

    return save;
}


int ListInsert (List* list, const void* data, int index)
{
    if (!ListOk (list))
        return 0;

    if (list -> size == list -> capacity && _listResize (list) == 0)
        return 0;


    if (index == 0 || list -> next[index] < 0 || list -> prev[index] < 0)
    {
        list -> err.emptyCell = 1;
        return 0;
    }

    int free = list -> free;
    list -> free = - list -> next[free];
    

    memcpy (list -> data + free * list -> itemSize, data, list -> itemSize);

    list -> next[list -> prev[index]] = free;
    list -> prev[free] = list -> prev[index];

    list -> next[free] = index;
    list -> prev[index] = free;

    list -> size ++;

    return free;
}

int ListAdd (List* list, const void* data)
{
    if (!ListOk (list))
        return 0;

    if (list -> size == list -> capacity && _listResize (list) == 0)
        return 0;

    Logs.trace ("List add");

    int free = list -> free;
    list -> free = - list -> next[list -> free];


    Logs.trace ("list ptr = %p", list);
    Logs.trace ("List -> data = %p", list -> data);
    Logs.trace ("List -> free = %d", free);


    memcpy (list -> data + free * list -> itemSize, data, list -> itemSize);

    list -> next[list -> prev[0]] = free;
    list -> prev[free] = list -> prev[0];

    list -> next[free] = 0;
    list -> prev[0] = free;

    list -> size ++;

    Logs.trace ("list added");


    return free;
}

int ListDelete (List* list, int index)
{
    if (!ListOk (list))
        return 0;

    if (_listResize (list) == 0)
        return 0;

    if (index == 0 || list -> next[index] < 0 || list -> prev[index] < 0)
    {
        list -> err.emptyCell = 1;
        return 0;
    }

    int newIndex;
    if (list -> next[index] != 0)
        newIndex = list -> next[index];
    else
        newIndex = list -> prev[index];

    list -> next[list -> prev[index]] = list -> next[index]; 
    list -> prev[list -> next[index]] = list -> prev[index];

    list -> next[index] = - list -> free;
    list -> prev[index] = -1;
    memset (list -> data + index * list -> itemSize, 0, list -> itemSize);

    list -> free = index;

    list -> size --;

    return newIndex;
}


int ListSet (List* list, const void* data, int index)
{
    if (!ListOk (list))
        return 0;


    if (index == 0 || list -> next[index] < 0 || list -> prev[index] < 0)
    {
        list -> err.emptyCell = 1;
        return 0;
    }

    memcpy (list -> data + index * list -> itemSize,  data, list -> itemSize);

    return index;
}

void* ListGet (List* list, int index)
{
    if (!ListOk (list))
        return 0;


    if (index == 0 || list -> next[index] < 0 || list -> prev[index] < 0)
    {
        list -> err.emptyCell = 1;
        return 0;
    }

    return list -> data + index * list -> itemSize;
}


bool ListDump (List* list, void (*datadump) (FILE* file, const void* item))
{
    Graph* graph = GraphOpen ();
    fprintf (graph -> file, "rankdir = LR\n");
    ListDump (list, graph -> file, datadump);
    GraphDraw (graph, "test", "svg");

    return 1;
}

bool ListDump (List* list, FILE* file, void (*dump) (FILE* file, const void* item))
{
    bool ok = ListOk (list);

    Graph graph = {};
    graph.file = file;

//    fprintf (file, "nodesep = 1\n");

    GraphNode node = {};
    node.id = list;
    if (ok)
        node.color = 0x90ff90;
    else
        node.color = 0xff9090;
    node.rounded = 1;
    node.shape = RECORD_SHAPE;

    GraphBeginCluster (&graph, &node);
    node.color = 0xffffff;
    GraphAddNode (&graph, &node, 
        "beginCanary: %d|"
        "<data>data: %p|"
        "<next>next: %p|"
        "<prev>prev: %p|"
        "<free>free: %d|"
        "capacity: %d|"
        "itemSize: %d|"
        "size: %d|"
        "endCanary: %d",
        list -> beginCanary,
        list -> data,
        list -> next,
        list -> prev,
        list -> free,
        list -> capacity,
        list -> itemSize,
        list -> size,
        list -> endCanary);
    GraphEndCluster (&graph);

    GraphEdge edge = {};

    GraphStreamlineNodes (&graph, node.id, list -> next);
    GraphAddEdge (&graph, &edge, node.id, "", list -> next, "");

    int i = 0;
    node.color = 0xb0b0b0;
    do
    {
        node.id = list -> next + i;
        GraphBeginCluster (&graph, &node, "%d", i);

        node.color = 0xffffff;
        GraphAddNode (&graph, &node,
            "<data>data|"
            "<next>next: %d|"
            "<prev>prev: %d",
            list -> next[i],
            list -> prev[i]);

        GraphEndCluster (&graph);

        if (list -> next[i] != 0)
            GraphStreamlineNodes (&graph, node.id, list -> next + list -> next[i]);
        GraphAddEdge (&graph, &edge, node.id, ":next", list -> next + list -> next[i], ":prev");

        i = list -> next[i];

        node.color = 0x90ff90;
    } while (i != 0);

    GraphStreamlineNodes (&graph, list, list -> next + list -> free);
    GraphAddEdge (&graph, &edge, list, ":free", list -> next + list -> free, "");

    i = -list -> free;
    while (i != 0)
    {
        node.color = 0xff9090;
        node.id = list -> next - i;
        GraphBeginCluster (&graph, &node, "%d", i);

        node.color = 0xffffff;
        
        GraphAddNode (&graph, &node,
            "<data>data|"
            "<next>next: %d|"
            "<prev>prev: %d",
            list -> next[-i],
            list -> prev[-i]);
            

        GraphEndCluster (&graph);

        if (list -> next[-i] != 0)
            GraphStreamlineNodes (&graph, node.id, list -> next - list -> next[-i]);
        GraphAddEdge (&graph, &edge, node.id, ":next", list -> next - list -> next[-i], ":prev");

        i = list -> next[-i];
    }

    i = 0;

    if (dump != nullptr)
        do
        {
            dump (file, list -> data + i * list -> itemSize);

            GraphStreamlineNodes (&graph, list -> next + i, list -> data + i * list -> itemSize);
            GraphAddEdge (&graph, &edge, list -> next + i, ":data", list -> data + i * list -> itemSize, "");

            i ++;
        } while (i <= list -> capacity);

    return ok;
}

bool ListOk (List* list)
{
    bool ok = 1;

    assert (list);

    if (list -> beginCanary != CANARY ||
        list -> endCanary   != CANARY)
    {
        list -> err.listDamaged = 1;
        return 0;
    }

    if (list -> data == nullptr)
    {
        list -> err.dataNullPtr = 1;
        ok = 0;
    }
    
    if (list -> next == nullptr)
    {
        list -> err.nextNullPtr = 1;
        ok = 0;
    }

    if (list -> prev == nullptr)
    {
        list -> err.prevNullPtr = 1;
        ok = 0;
    }
            
    return ok;
}

void ListErrPrint (ListError err, FILE* file)
{
    if (file == nullptr)
        file = stdout;

    bool ok = 1;
    
    if (err.allocationError)
    {
        ok = 0;
        fprintf (file, "ALLOCATION ERROR\n");
    }
    if (err.dataNullPtr)
    {
        ok = 0;
        fprintf (file, "DATA NULL POINTER\n");
    }
    if (err.nextNullPtr)
    {
        ok = 0;
        fprintf (file, "NEXT NULL POINTER\n");
    }
    if (err.prevNullPtr)
    {
        ok = 0;
        fprintf (file, "DATA NULL POINTER\n");
    }
    if (err.listDamaged)
    {
        ok = 0;
        fprintf (file, "LIST DAMAGED\n");
    }
    if (err.emptyCell)
    {
        ok = 0;
        fprintf (file, "EMPTY SELL\n");
    }
    if (err.maxSizeReached)
    {
        ok = 0;
        fprintf (file, "MAX SIZE REACHED\n");
    }
    
    
    if (ok)
        fprintf (stdout, "no errors\n");
}

int _listResize (List* list)
{
    Logs.trace ("called _listResize");
    int newCapacity = list -> capacity;

    printf ("%i ", newCapacity);

    if (list -> size >= list -> capacity)
    {
        if (list -> capacity >= MAX_CAPACITY)
        {
            list -> err.maxSizeReached = 1;
            return 0;
        }

        if (newCapacity == 1)
            newCapacity = 2;
        newCapacity *= 2 * (1 - 0.5 * newCapacity / MAX_CAPACITY);

        if (newCapacity == list -> capacity)
            newCapacity = MAX_CAPACITY;
    }
    else
    {
        while (list -> size < newCapacity / 3)
            newCapacity = newCapacity * 2 / 3;
    }

    printf ("%i ", newCapacity);
    if (list -> capacity != newCapacity)
    {
        list -> data = (char*) realloc (list -> data, (newCapacity + 1) * list -> itemSize);
        list -> next = (int*) realloc (list -> next, (newCapacity + 1) * sizeof (int));
        list -> prev = (int*) realloc (list -> prev, (newCapacity + 1) * sizeof (int));

        if (list -> data == nullptr ||
            list -> next == nullptr ||
            list -> prev == nullptr)
        {
            list -> err.allocationError = 1;
            return 0;
        }

        for (int i = list -> capacity + 1; i < newCapacity + 1; i ++)
        {
            list -> next[i] = -(i + 1);
            list -> prev[i] = -1;
            memset (list -> data + i * list -> itemSize, 0, list -> itemSize);
        }
        list -> next[newCapacity] = 0;

        list -> free = list -> capacity + 1;


        list -> capacity = newCapacity;
    }

    printf ("%i\n", newCapacity);

    return newCapacity;
}

int ListFindItem   (List* list, const void* key, int (*cmp) (const void* val, const void* key))
{
    for (int i = list -> next[0]; i != 0; i = list -> next[i])
    {
        if (cmp (list -> data + i * list -> itemSize, key))
            return i;
    }

    return 0;
}

int SlowSlowVerySlow_ThereIsNoSenseToCallMe_ThinkHarder_LogicalIndexToPhysicalIndex (List* list, unsigned int logicalIndex)
{
    if (!ListOk (list))
        return 0;

    int physicalIndex = list -> next[0];

    while (logicalIndex -- > 0 || list -> next[physicalIndex] != 0)
        physicalIndex = list -> next[physicalIndex];

    return physicalIndex;
}
