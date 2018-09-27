#ifndef BNB_B_
#define BNB_B_

#include "../lib/definitions.h"
#include "../lib/Solution.h"
#include "Node.h"

#include <list>
#include <vector>
#include <utility> // std::pair, std::make_pair

using std::list;
using std::vector;
using std::pair;

class BNB
{
  public:
    int n_facs, n_objs;
    DistMatrix *dist_mat;
    FlowMatrices *flow_mats;

  public:
    BNB();
    
    BNB(int n_f, int n_o, DistMatrix *dist, FlowMatrices *flows)
    {
        n_facs = n_f;
        n_objs = n_o;
        dist_mat = dist;
        flow_mats = flows;
    }
    
    pair<int,int> init(list<Solution> &non_dominated_set);
    void initial_solution(list<Solution> &non_dominated_set);
    Node* initial_node();
    Node* make_node(int n_facs, int item, Node *p_node);
    void make_solution(Node *n, list<Solution> &non_dominated_set);
};

#endif