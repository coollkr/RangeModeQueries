/*
 * It is the file for constructing the data structure and run the query.
 * The abbreviation for this data structure is pst,
 * and it is the data structure described in the following paperS.
 *
 * Reference:
 * [1] Prosenjit Bose, Evangelos Kranakis, Pat Morin, and Yihui Tang. Approximate range mode and range median queries. In Annual Symposium on Theoretical Aspects of Computer Science, pages 377–388. Springer, 2005.
 * [2] D. Jansens. Persistent Binary Search Trees. https://cglab.ca/~dana/pbst/#libadds
 */
#define MEASURE_SPACE
#include <iostream>
#include <math.h>
#include "adds.h"
#include "ApproxMode_PST.h"
using namespace std;

typedef struct _PTreeRootVersion   PTreeRootVersion;
typedef struct _PTreeNodeVersion   PTreeNodeVersion;
typedef struct _PTreeNodeData      PTreeNodeData;
typedef struct _PTreeNode          PTreeNode;

struct _PTreeRootVersion
{
    PTreeNode *root;
    guint      version;
};

struct _PTree
{
    PTreeRootVersion *r; /* versioned root nodes of the tree.  r[0] is
                          the highest (latest) version.  then r[1]..r[nr-1] are
                          older versions in ascending order.  Use first_v(),
                          next_v() and prev_v() to traverse the list. */
    guint             nr;
    GCompareDataFunc  key_compare;
    GDestroyNotify    key_destroy_func;
    GDestroyNotify    value_destroy_func;
    gpointer          key_compare_data;
    guint             nnodes;
    gint              ref_count;
    guint             version;
};

struct _PTreeNodeVersion
{
    guint      version;
    PTreeNode *left;        /* left subtree */
    PTreeNode *right;       /* right subtree */
    PTreeNode *parent;      /* parent node */
};

struct _PTreeNodeData
{
    gint   key;         /* key for this node */
    gint   value;       /* value stored at this node */
    gint       ref_count;
    guint8     stolen;      /* true if the node is stolen instead of removed */
};

struct _PTreeNode
{
    PTreeNodeData    *data; /* the node's permanent data */
    PTreeNodeVersion *v;    /* versions of pointers for the node.  v[0] is the
                             highest (latest) version.  then v[1]..v[nv-1] are
                             older versions in ascending order.  Use first_v(),
                             next_v() and prev_v() to traverse the list. */
    guint             nv;   /* number of versions stored in this node */
};
static gint cmp (gconstpointer k1, gconstpointer k2)
{
    glong r = GPOINTER_TO_INT(k1) - GPOINTER_TO_INT(k2);
    return r < 0 ? -1 : (r == 0 ? 0 : 1);
}


ApproxMode_PST :: ApproxMode_PST(int* array, int len, double alpha) { // constructor
    this->array = array;
    this->len = len;
    this->alpha = alpha;
}
ApproxMode_PST :: ~ApproxMode_PST() {
    p_tree_destroy(tree);
}
int ApproxMode_PST :: getIndexValue(int index) {// return the value of array[index]
    return array[index];
}
int ApproxMode_PST :: findQueryResult(int start_index, int end_index) { // binary search on the PST tree, parameter: left index of the query and right index of the query
    int left = start_index - 1, right = end_index - 1;
    int finalMode = -1;
    gpointer modeNode = p_tree_lookup_related_v(tree, left, GINT_TO_POINTER(right), P_TREE_SEARCH_PREDECESSOR);
    if (modeNode == NULL) {
        finalMode = array[left];
    } else {
        finalMode = array[GPOINTER_TO_INT(modeNode)];
    }
    return finalMode;
}
void ApproxMode_PST :: construct() { // construct the data structure

#ifdef MEASURE_SPACE
    long long space = 0;
#endif MEASURE_SPACE

    int delta = 0;
    for (int i = 0; i < len; i++) {
        delta = max(delta, array[i]); // distinct numbers
    }



    int* dict = new int[delta + 1];
    for (int i = 0; i < delta + 1; i++) {
        dict[i] = 0;
    }



    int rowNumber = 2 * (ceil)(log(len) / log(1 / alpha));
    pair<int, int> **table = new pair<int, int>*[rowNumber]; // template table
    for (int r = 0; r < rowNumber; r++) {
        table[r] = new pair<int, int>[len];
    }


    pair<int, int> *bound = new pair<int, int>[rowNumber];

    for (int r = 0; r < rowNumber; r++) {
        for (int i = 0; i < len; i++) {
            table[r][i] = make_pair(-1, -1);
        }
    }


    for (int r = 0; r < rowNumber; r++) {
        bound[r] = make_pair(-1, -1);
    }


    // update the temporary table

    int fLow = 1, fHigh = 1, prevLow = 1, prevHigh = 1;
    for (int r = 0; r < rowNumber; r++) {  // update the table row by row
        int currentApproximate = 0, currentApproximateFreq = 0;
        int left = 0, right = 0;
        if (r != 0){
            fLow = prevHigh + 1;
            fHigh = (prevLow / alpha) + 1;
            bound[r] = make_pair(fLow, fHigh);
        } else {
            bound[r] = make_pair(1, 1);
        }
        if (fLow > len) {
            break;
        }

        for (int i = 0; i < delta + 1; i++) {
            dict[i] = 0;
        }


        for (right = 0; right < len; right++) { // move the right pointer
            dict[array[right]]++;
            if (currentApproximateFreq < fLow && dict[array[right]] == fHigh) { // reach the fHigh
                currentApproximateFreq = dict[array[right]];
                currentApproximate = array[right];
                while (currentApproximateFreq >= fLow && left <= right ) { // move the left pointer

                    table[r][left] = make_pair(right, r);

                    if (currentApproximate == array[left]) {
                        currentApproximateFreq--;
                    }
                    if (currentApproximateFreq == 0) {
                        currentApproximate = 0;
                    }

                    dict[array[left]]--;
                    left++;
                }
            }

        }

        prevLow = fLow;
        prevHigh = fHigh;

    }
    long nr = 0;
    long nvmNv = 0;



    pair<int, int> *prePair = new pair<int, int>[rowNumber];
    int count = 0;
    pair<int, int> tmp = make_pair(-1, -1);

    tree = p_tree_new(cmp);

    // convert the table into pst tree
    for (int i = 0; i < len; i++) {
        if (i > 0) {
            p_tree_next_version(tree);
        }

        for (int r = 1; r < rowNumber; r++) { // ignore the first row

            if (bound[r].first > len) {
                break;
            }
            pair<int, int> cur = table[r][i];
            pair<int, int> pre = prePair[r];
            if (i > 0) {
                if (cur.first == pre.first && cur.second == pre.second) { // cur value equal with the previous value in the same row
                    continue;
                } else { // does not equal
                    if (cur.first == -1 && cur.second == -1 && pre.first != -1 && pre.second != -1) {
                        if (r >= 1) {
                            // cur value is invalid
                            if (tmp.first != -1 && tmp.second != -1) {
                                // remove the previous first and add the tmp
#ifdef MEASURE_SPACE
                                int p =  GPOINTER_TO_INT(p_tree_lookup_related_nv(tree, p_tree_current_version(tree), GINT_TO_POINTER(pre.first), P_TREE_SEARCH_EXACT));
                                space += (sizeof(PTreeNodeVersion) * p);
                                nvmNv += p;
                                count++;
#endif MEASURE_SPACE
                                p_tree_remove(tree, GINT_TO_POINTER(pre.first));
                                p_tree_insert(tree, GINT_TO_POINTER(tmp.first), GINT_TO_POINTER(tmp.second));
                                tmp.first = -1;
                                tmp.second = -1;
                                prePair[r] = cur;
                            } else {
                                // remove the previous
#ifdef MEASURE_SPACE
                                int p =  GPOINTER_TO_INT(p_tree_lookup_related_nv(tree, p_tree_current_version(tree), GINT_TO_POINTER(pre.first), P_TREE_SEARCH_EXACT));
                                space += (sizeof(PTreeNodeVersion) * p);
                                nvmNv += p;
#endif MEASURE_SPACE
                                p_tree_remove(tree, GINT_TO_POINTER(pre.first));
                                prePair[r] = cur;
                            }
                        }
                        break;
                    } else {    // cur not -1   pre not -1
                        // remove the previous first and add the cur
#ifdef MEASURE_SPACE
                        int p =  GPOINTER_TO_INT(p_tree_lookup_related_nv(tree, p_tree_current_version(tree), GINT_TO_POINTER(pre.first), P_TREE_SEARCH_EXACT));
                        space += (sizeof(PTreeNodeVersion) * p);
                        nvmNv += p;
#endif MEASURE_SPACE

                        p_tree_remove(tree, GINT_TO_POINTER(pre.first));
                        if (tmp.first != -1 && tmp.second != -1) {
#ifdef MEASURE_SPACE
                            count++;
#endif MEASURE_SPACE
                            p_tree_insert(tree, GINT_TO_POINTER(tmp.first), GINT_TO_POINTER(tmp.second));
                            tmp.first = -1;
                            tmp.second = -1;
                        }

                        gpointer equal = p_tree_lookup_related_v(tree, p_tree_current_version(tree) - 1, GINT_TO_POINTER(cur.first), P_TREE_SEARCH_EXACT);
                        if (equal != NULL) { // thats mean tmp cannot add into PST right now, we will try to add it in the next
                            tmp.first = cur.first;
                            tmp.second = cur.second;
                        } else {
#ifdef MEASURE_SPACE
                            count++;
#endif MEASURE_SPACE
                            p_tree_insert(tree, GINT_TO_POINTER(cur.first), GINT_TO_POINTER(cur.second));
                        }

                    }
                }
            } else { // i == 0  first column
                if (cur.first == -1 && cur.second == -1) {
                    prePair[r] = cur;
                    continue;
                }
                p_tree_insert(tree, GINT_TO_POINTER(cur.first), GINT_TO_POINTER(cur.second));
#ifdef MEASURE_SPACE
                count++;
#endif MEASURE_SPACE
            }
            prePair[r] = cur;
        }

    }


    nr = tree->nr; // get the value of nr




    cout << "count: " << count << endl;
    cout << "sizeof(PTreeNode): " << sizeof(PTreeNode) << endl;
    cout << "sizeof(_PTreeNodeData): " << sizeof(PTreeNodeData) << endl;
    cout << "sizeof(PTreeNodeVersion): " << sizeof(PTreeNodeVersion) << endl;
    cout << "sizeof(PTreeRootVersion): " << sizeof(PTreeRootVersion) << endl;
    cout << "sizeof(guint): " << sizeof(guint) << endl;
    cout << "numNv: " << nvmNv << endl;
    cout << "nr: " << nr << endl;


#ifdef MEASURE_SPACE
    space += (sizeof(PTreeNode) * count); // count = the number of PTreeNode
    space += (sizeof(PTreeNodeData) * count);
    space += (sizeof(PTree));
    space += (sizeof(PTreeRootVersion) * nr);
    cout << "space in bytes: " << space << endl;
#endif MEASURE_SPACE







    for (int r = 0; r < rowNumber; r++) {
        delete[] table[r];
    }
    delete[] table;
    delete[] dict;
    delete[] prePair;
    delete[] bound;

}