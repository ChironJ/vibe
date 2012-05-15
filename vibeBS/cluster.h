#ifndef _CLUSTER_H_
#define _CLUSTER_H_

#include "list.h"

/// Cluster structure
struct Cluster {
    int minX, minY, maxX, maxY;     ///< MIN/MAX location value of the nodes
    int numNode;                    ///< Nodes' number in the list
    list_iterator node_list_head;   ///< List head of all nodes of the cluster
    list_iterator cluster_list;     ///< Link to the list of all the clusters
};

/// Node structure
struct Node {
    int x, y;                       ///< Node's location in the array
    list_iterator node_list;        ///< Link to the node list of the cluster
    list_iterator all_nodes_list;   ///< Link to the list of all nodes
    struct Cluster *cluster;        ///< Cluster it belongs to
};

/// Array structure
struct Array {
    int nx, ny;                     /// Size of the array
    size_t size;                    /// Item size
    char *data;                     /// Data of the array
};

extern int get_clusters(struct Array *array, int dis, list_iterator *cluster_list_head);
extern void free_clusters(list_iterator *cluster_list_head);
extern void print_array(struct Array *array);
extern void print_clusters(struct Array *array, list_iterator *cluster_list_head);
#endif
