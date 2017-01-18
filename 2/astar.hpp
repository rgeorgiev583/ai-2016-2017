#ifndef __ASTAR__
#define __ASTAR__

#include <vector>
#include <functional>
#include <memory>

#include "state.hpp"


namespace SlidingBlocks
{
    using HeuristicFunction = std::function<int(const State&, const State&)>;
    using StatePtr = std::shared_ptr<State>;
    struct Node;
    using NodePtr = std::shared_ptr<Node>;

    struct Node
    {
        StatePtr State;
        NodePtr Parent;
        int G, H;

        Node(StatePtr state, NodePtr parent = nullptr);

        bool operator<(const Node& other);
    };


    std::vector<StatePtr> FindPath(const State& source, const State& target, HeuristicFunction heuristic = ManhattanDistance);
}


#endif  // __ASTAR__
