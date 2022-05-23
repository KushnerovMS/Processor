#ifndef GRAPH_DUMP_H
#define GRAPH_DUMP_H

#include <stdio.h>

const char NODE_STYLE_STRUCTURE[] = "record";

const float LINE_COLOR_COEF = 0.8;

//const unsigned long GRAPH_BACKGROUND_COLOR = 0x909090ff;

enum NodeShape
{
    NONE_SHAPE      = 0,
    RECORD_SHAPE    = 1,
    RECTANGLE_SHAPE = 2
};

const char NODE_SHAPES[][100] = {"none",
                                "record",
                                "rectangle"};

typedef struct Graph_
{
    FILE* file;

    char* name;
} Graph;

typedef struct GraphNode_
{
    const void* id;

    NodeShape shape;
    bool rounded;
    unsigned long color;
} GraphNode;

typedef GraphNode_ GraphCluster;

typedef struct GraphEdge_
{
    unsigned long color;
    int minLen;
    int penWidth;
    
} GraphEdge;

Graph*  GraphOpen ();
void    GraphDraw (Graph* graph, char* name, char* format);

void    GraphAddNode (Graph* graph, GraphNode* node, const char* labelfrmt = nullptr, ...);
void    GraphAddEdge (Graph* graph, GraphEdge* edge,
                        const void* beginId, const char* beginPort,
                        const void* endId, const char* endPort,
                        const bool streamLine = 0,
                        const char* labelfrmt = nullptr, ...);
void    GraphBeginCluster (Graph* graph, GraphCluster* cluster, const char* labelfrmt = nullptr, ...);
void    GraphEndCluster (Graph* graph);

void    GraphStreamlineNodes (Graph* graph, const void* id1, const void* id2);

unsigned long _getLineColor (unsigned long color);
unsigned long _getFontColor (unsigned long color);

#endif
