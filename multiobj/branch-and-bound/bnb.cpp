#include "BNB.h"

#include <list>
#include <stack>
#include <utility> // std::pair, std::make_pair

using std::list;
using std::stack;
using std::pair;


bool check_dominance(vector<int> &objs, list<Solution> &non_dominated_set){
    for (auto sol : non_dominated_set)
    {
        if (sol.objs <= objs)
            return false;
    }
    return true;
}

pair<int,int> BNB::init(list<Solution> &non_dominated_set){
    int num_nodes = 0; int pruned_nodes = 0;
        // We count how many nodes are generated and how many are pruned

    initial_solution(non_dominated_set);

    stack<Node*> nodes;

    Node* first_node = initial_node();
    nodes.push(first_node);
    num_nodes++;

    /* 
      On this method, we check the pruning condition before the insertion of a node and after the removal.
      This happens because a node can be "prunable" in its creation
      On the other hand, we can insert a non-prunable node on the stack, but when it's removed,
      there can be a solution that dominates the lower bound of that node.
    */
    /*while(!nodes.empty())
    {
        Node *n = nodes.top(); nodes.pop();

        if(!check_dominance(n->lower_bound, non_dominated_set))
        {
            // The lower bound of the node in consideration is dominated by a solution
            // So, we prune that node
            pruned_nodes++;
            delete n;
            continue;
        }

        // last_item varies between 0 and n_facs-1.
        // When the condition is satisfied, there is only one item to add, so it's not necessary create nodes
        if(n->last_item == n_facs-2)
        {
            num_nodes++; // A solution is a leaf node
            make_solution(n, non_dominated_set); 
            // A solution will be constructed and also possibly inserted in the non-dominated solutions set
        }
        else
        {
            // In this case, a solution cannot be constructed yet

            for (auto j = n->remaining_locations.rbegin(); j != n->remaining_locations.rend(); j++)
            {
                Node *child = make_node(n_facs, *j, n);
                num_nodes += 1;

                if (!check_dominance(child->lower_bound, non_dominated_set))
                {
                    // The lower bound of the node in consideration is dominated by a solution
                    // So, we prune that node
                    pruned_nodes++;
                    delete child;
                    continue;
                }

                nodes.push(child);            
            }
        }

        delete n;
    }*/

    return std::make_pair(num_nodes, pruned_nodes);
}

void BNB::make_solution(Node *n, list<Solution> &non_dominated_set)
{
    if(n->last_item != n_facs-2)
    {
        std::cout << "Calling make_solution with an innapropriate node.\n";
        if(n->last_item < n_facs-2) std::cout << "Node has not enough assignments yet.\n";
        else std::cout << "Node has more assignments than should have.\n";
    }

    Solution s;
    s.permutation = n->partial_assignment;
    auto item = n->remaining_locations.begin();
    s.permutation[n_facs-1] = *item;
    s.compute_objs(*dist_mat, *flow_mats);

    for (auto it_sol = non_dominated_set.begin(); it_sol != non_dominated_set.end(); it_sol++ )
    {
        if (s.objs == it_sol->objs)
        {
            // The solutions have same objective, so we can keep both of them, a priori
            continue;
        }
        else if (s.objs <= it_sol->objs)
        {
            // The new solution dominates (they are different) a solution from set
            // The solution from set must be removed
            it_sol = non_dominated_set.erase(it_sol); it_sol--; 
                /* The method will return the next element, and the for will move one more element,
                   so we need to move back 
                */
        }
        else if (it_sol->objs <= s.objs)
        {
            // The new solution is dominated by a solution in the set, and therefore must be discarded
            return;
        }
    }

    // If the new solution was not discarded during for, it must be inserted in the non-dominated set
    non_dominated_set.push_back(s);
}

Node* BNB::make_node(int n_facs, int location, Node *p_node)
{
    Node *n = new Node(p_node);

    n->last_item += 1;
    n->remaining_locations.erase(location);
    n->partial_assignment[n->last_item] = location;

    n->compute_lowerbound(*dist_mat, *flow_mats);

    return n;
}

void BNB::initial_solution(list<Solution> &non_dominated_set)
{
    non_dominated_set.emplace_back(n_facs, n_objs);

    auto first_s = non_dominated_set.begin();

    for (int i = 0; i < n_facs; ++i)
    {
        first_s->permutation[i] = i;
    }

    first_s->compute_objs(*dist_mat, *flow_mats);
    first_s->print();
}

Node* BNB::initial_node()
{
    Node *n = new Node();
    //std::cout << "initial node\n";
    int locations[n_facs]; //std::cout << "array declaration\n";
    //std::cout << "n_facs: " <<n_facs;
    for (int i = 0; i < n_facs; ++i)
    {
        locations[i] = i; //std::cout << " set loc " << i;
    }

    n->last_item = -1; //std::cout << "Node last_item\n";
    n->remaining_locations = set<int>(locations, locations+n_facs); //std::cout << "node remaining locations\n";
    n->partial_assignment.resize(n_facs);
    
    n->lower_bound.resize(n_objs);
    for (int i = 0; i < n_objs; ++i)
    {
        n->lower_bound[i] = 0;
    } //std::cout << "node lower bound\n";

    //n->print();

    return n;
}