#include "BNB.h"

#include <list>
#include <stack>
#include <utility> // std::pair, std::make_pair

using std::list;
using std::stack;
using std::pair;

/*
  Checks if a node is dominated by a solution.
  If it is, return false. (That node will be discarded)
  It it's not, return true.

  Note: the '<=' operator for solution and node is defined in Node.cpp
*/
bool check_dominance(Node &node, list<Solution> &non_dominated_set)
{
    for (auto sol : non_dominated_set)
    {
        if (sol <= node)
            return false;
    }
    return true;
}

pair<int,int> BNB::init(list<Solution> &non_dominated_set)
{
    int num_nodes = 0; int pruned_nodes = 0;
        // We count how many nodes are generated and how many are pruned

    initial_solution(non_dominated_set);

    stack<Node*> nodes;

    Node* first_node = initial_node();
    nodes.push(first_node);
    num_nodes++;

    /* 
      On this method, we check the pruning condition before the insertion of a node and after its removal.
      This happens because a node can be "prunable" in its creation
      On the other hand, we can insert a non-prunable node on the stack, but when it's removed,
      there can be a solution that dominates the lower bound of that node.
    */
    while(!nodes.empty())
    {
        //std::cout << "Extract node from stack. ";
        Node *n = nodes.top(); nodes.pop();
        //std::cout << "Extracted node from stack.\n";
        //n->print(); //std::cout << "\n";

        if(!check_dominance(*n, non_dominated_set))
        {
            // The lower bound of the node in consideration is dominated by a solution
            // So, we prune that node
            pruned_nodes++;
            delete n;
            continue;
            //std::cout << "Pruned node\n";
        }

        // last_item varies between 0 and n_facs-1.
        // So the last item assigned before a node makes a viable solutions is n_facs-2
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
            /*----------------------- create child nodes -----------------------*/
            for (auto j = n->remaining_locations.rbegin(); j != n->remaining_locations.rend(); j++)
            {
                Node *child = make_node(n_facs, *j, n); //child->print();
                num_nodes += 1;

                if (!check_dominance(*child, non_dominated_set))
                {
                    // The lower bound of the node in consideration is dominated by a solution
                    // So, we prune that node
                    pruned_nodes++;
                    delete child;
                    continue;
                }

                nodes.push(child);            
            }
            /*----------------------- done create child nodes -----------------------*/
        }

        delete n;
    }

    return std::make_pair(num_nodes, pruned_nodes);
}

void BNB::make_solution(Node *n, list<Solution> &non_dominated_set)
{
    if(n->last_item != n_facs-2)
    {
        std::cout << "Calling make_solution with an innapropriate node.\n";
        if(n->last_item < n_facs-2) std::cout << "Node has not enough assignments yet.\n";
        else std::cout << "Node has more assignments than should have.\n";
        return;
    }

    /*------------------------------- CONSTRUCT SOLUTION -------------------------------*/
    // The last item is assigned to the last remaining location
    Solution s(n_facs, n_objs);

    s.permutation = n->partial_assignment;
    auto loc = n->remaining_locations.begin();
    s.permutation[n_facs-1] = *loc;

    s.compute_objs(*dist_mat, *flow_mats);
    /*------------------------------- CONSTRUCT SOLUTION -------------------------------*/

    /*------------------------------- CHECK DOMINANCE RELATIONS -------------------------------*/
    for (auto it_sol = non_dominated_set.begin(); it_sol != non_dominated_set.end(); it_sol++ )
    {
        if (s.objs == it_sol->objs)
        {
            if(s.permutation == it_sol->permutation) return;            
            else continue;
        }
        else if(s <= *it_sol)
        {
            // The new solution dominates (they are different) a solution from set
            // The solution from set must be removed
            it_sol = non_dominated_set.erase(it_sol); it_sol--;
                /* The erase method returns the next element, and the for will move one more element,
                   so we need to move back 
                */
        }
        else if(*it_sol <= s)
        {
            // The new solution is dominated by a solution in the set, and therefore must be discarded
            return;
        }
    }
    /*------------------------------- CHECK DOMINANCE RELATIONS -------------------------------*/

    // If the new solution was not discarded during for-loop, it must be inserted in the non-dominated set
    non_dominated_set.push_back(s);
}

Node* BNB::make_node(int n_facs, int location, Node *p_node)
{
    Node *n = new Node(p_node);
    n->last_item += 1;
    n->remaining_locations.erase(location);
    n->partial_assignment[n->last_item] = location;
    n->lower_bound.resize(n_objs);
    n->compute_lowerbound(*dist_mat, *flow_mats);
    return n;
    // Ideas for the future: compute the lower bound of a child node using only the parent node,
    // so it will not be necessary create a new node and use more memory and more time
}

void BNB::initial_solution(list<Solution> &non_dominated_set)
{
    non_dominated_set.emplace_back(n_facs, n_objs);
    auto first_s = non_dominated_set.begin();
    for (int i = 0; i < n_facs; ++i) first_s->permutation[i] = i;
    first_s->compute_objs(*dist_mat, *flow_mats);
    //first_s->print();
}

Node* BNB::initial_node()
{
    Node *n = new Node();

    int locations[n_facs];
    for (int i = 0; i < n_facs; ++i) locations[i] = i;
    n->n_facs = n_facs;
    n->n_objs = n_objs;
    n->last_item = -1;
    n->remaining_locations = set<int>(locations, locations+n_facs);
    n->partial_assignment.resize(n_facs);
    
    n->lower_bound.resize(n_objs);
    for (int i = 0; i < n_objs; ++i) n->lower_bound[i] = 0;
    
    return n;
}