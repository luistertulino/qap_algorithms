#ifndef NODE_H_
#define NODE_H_

#include <vector>
#include <set>

#include "../lib/libhungarian/hungarian.h"
#include "../lib/definitions.h"
#include "../lib/Solution.h"

using std::vector;
using std::set;

typedef vector<int> LowerBound;

class Node
{
  public:
    LowerBound lower_bound;
    int last_item; // The last item that was assigned; varies between 0 and n_facs-1
    int n_facs, n_objs;
    vector<int> partial_assignment;
    set<int> remaining_locations;

  public:
    Node(){}
    Node(int last, set<int> &remaining, vector<int> &partial, int n_f, int n_o)
    {
        last_item = last;
        remaining_locations = remaining;
        partial_assignment = partial;
        n_facs = n_f;
        n_objs = n_o;
    }
    Node(LowerBound &lb, int last, set<int> &remaining, vector<int> &partial, int n_f, int n_o)
    {
        lower_bound = lb;
        last_item = last;
        remaining_locations = remaining;
        partial_assignment = partial;
        n_facs = n_f;
        n_objs = n_o;
    }
    Node(Node *n)
    {
        last_item = n->last_item;
        remaining_locations = n->remaining_locations;
        partial_assignment = n->partial_assignment;
        n_facs = n->n_facs;
        n_objs = n->n_objs;
    }

    void compute_lowerbound(DistMatrix &dist_mat, FlowMatrices &flow_mats);
    void print();
};

bool operator<=(Solution &s, Node &n);

#endif