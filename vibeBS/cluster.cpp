#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cluster.h"

struct Cluster *clusterPool = NULL;

/// Cluster the non-zero nodes in an array.
/// \param[in]  array : Array to be clustered.
/// \param[in]  dis   : Min distance to judge whether a node belong to a cluster 
/// \param[out] cluster_list_head : list of all clusters.
/// \retval 0 : Success;
/// \retval 1 : Failed.
int get_clusters(struct Array *array, int dis, list_iterator *cluster_list_head)
{
    int retVal;
    int numNodes;
    int x, y, z, i, j;
    struct Node *node, *nnode, *snode, **nodeArray, *nodePool;
    struct Cluster *cluster, *ncluster;
    list_iterator *list, *slist;
    list_iterator all_nodes_list_head;

    retVal = 0;
    list_init(&all_nodes_list_head);
    list_init(cluster_list_head);

    nodeArray = (struct Node **)
        malloc(sizeof(struct Node *) * array->nx * array->ny);
    if (!nodeArray) {
        fprintf(stderr, "Out of memory for nodeArray\n");
        retVal = 1;
        goto ret;
    }

    // Get all the non-zero nodes and allocate a cluster for each non-zero node.
    numNodes = 0;
    for (y = 0; y < array->ny; y++) {
        for (x = 0; x < array->nx; x++) {
            nodeArray[array->nx * y + x] = NULL;
            for (z = 0; z < array->size; z++) {
                if (array->data[(array->nx * y + x) * array->size + z]) break;
            }
            if (z < array->size) {
                // non-zero node
                numNodes++;
            }
        }
    }

    nodePool = (struct Node *) malloc(sizeof(struct Node) * numNodes);
    if (!nodePool) {
        fprintf(stderr, "Out of memory for node\n");
        retVal = 1;
        goto ret;
    }
    assert(clusterPool == NULL);
    clusterPool = (struct Cluster *) malloc(sizeof(struct Cluster) * numNodes);
    if (!clusterPool) {
        fprintf(stderr, "Out of memory for cluster\n");
        retVal = 1;
        goto ret;
    }

    i = 0;
    for (y = 0; y < array->ny; y++) {
        for (x = 0; x < array->nx; x++) {
            nodeArray[array->nx * y + x] = NULL;
            for (z = 0; z < array->size; z++) {
                if (array->data[(array->nx * y + x) * array->size + z]) break;
            }
            if (z < array->size) {
                // non-zero node
                node = nodePool + i;
                node->x = x;
                node->y = y;
                list_add(&all_nodes_list_head, &node->all_nodes_list);
                nodeArray[array->nx * y + x] = node;

                cluster = clusterPool + i;
                list_add(cluster_list_head, &cluster->cluster_list);
                list_init(&cluster->node_list_head);
                list_add(&cluster->node_list_head, &node->node_list);
                node->cluster = cluster;
                cluster->minX = cluster->maxX = x;
                cluster->minY = cluster->maxY = y;
                cluster->numNode = 1;

                i++;
            }
        }
    }
    assert(i == numNodes);

    // Clustering
    list_for_each(list, &all_nodes_list_head) {
        node = list_entry(list, struct Node, all_nodes_list);
        // Scan all the nodes [x - dis, y - dis], [x + dis, y + dis];
        // Only need [x, y], [x - dis, y - dis]
        // The order is important for the performance
        for (y = node->y; y >= node->y - dis; y--) {
            for (x = node->x; x >= node->x - dis; x--) {
                if ((x < 0) || (y < 0) ||
                        (x >= array->nx) || (y >= array->ny)) continue;
                nnode = nodeArray[array->nx * y + x];
                if (!nnode) continue;
                assert((nnode->x == x) && (nnode->y == y));
                if ((x == node->x) && (y == node->y)) continue;
                if (node->cluster == nnode->cluster) continue;
                if ((abs(x - node->x) + abs(y - node->y)) > dis) continue;
                if (node->cluster->numNode > nnode->cluster->numNode) {
                    cluster = node->cluster;
                    ncluster = nnode->cluster;
                } else {
                    cluster = nnode->cluster;
                    ncluster = node->cluster;
                }
                // Merge the two clusters
                // Change the cluster of the nodes at first
                list_for_each(slist, &ncluster->node_list_head) {
                    snode = list_entry(slist, struct Node, node_list);
                    snode->cluster = cluster;
                    if (snode->x < cluster->minX) cluster->minX = snode->x;
                    if (snode->x > cluster->maxX) cluster->maxX = snode->x;
                    if (snode->y < cluster->minY) cluster->minY = snode->y;
                    if (snode->y > cluster->maxY) cluster->maxY = snode->y;                    
                }
                cluster->numNode += ncluster->numNode;
                // Merge the node list
                list_merge(&cluster->node_list_head, &ncluster->node_list_head);
                list_del(&ncluster->cluster_list);
            }
        }
    }
ret:
    if (nodeArray) free(nodeArray);
    if (nodePool) free(nodePool);
    list_for_each(list, cluster_list_head) {
        cluster = list_entry(list, struct Cluster, cluster_list);
        list_init(&cluster->node_list_head);
    }
    return retVal;
}

/// Free the clusters
/// \param[in] cluster_list_head : list of all clusters.
void free_clusters(list_iterator *cluster_list_head)
{
    free(clusterPool);
    clusterPool = NULL;
    return;
}
///Print the array
/// \param[in] array : Array.
void print_array(struct Array *array)
{
    int x, y, z;
    for (y = 0; y < array->ny; y++) {
        for (x = 0; x < array->nx; x++) {
            printf(" ");
            for (z = 0; z < array->size; z++) {
                printf("%d",
                        array->data[(array->nx * y + x) * array->size + z]);
            }
        }
        printf("\n");
    }
    return;
}

/// Print the array according the clusters
/// \param[in] array : Array.
/// \param[in] cluster_list_head : list of all clusters.
void print_clusters(struct Array *array, list_iterator *cluster_list_head)
{
    int x, y, num;
    struct Array tmpArray;
    struct Cluster *cluster;
    list_iterator *list;

    tmpArray.nx = array->nx;
    tmpArray.ny = array->ny;
    tmpArray.size = sizeof(char);
    tmpArray.data = (char *) malloc(tmpArray.size * tmpArray.nx * tmpArray.ny);
    if (!tmpArray.data) {
        fprintf(stderr, "Out of memory!\n");
        goto ret;
    }
    memset(tmpArray.data, 0, tmpArray.size * tmpArray.nx * tmpArray.ny);

    num = 0;
    list_for_each(list, cluster_list_head) {
        num++;
        cluster = list_entry(list, struct Cluster, cluster_list);
        for (y = cluster->minY; y <= cluster->maxY; y++) {
            for (x = cluster->minX; x <= cluster->maxX; x++) {
                tmpArray.data[tmpArray.nx * y + x] = num;
            }
        }
    }
    print_array(&tmpArray);
ret:
    return;
}
