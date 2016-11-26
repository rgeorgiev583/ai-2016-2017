#ifndef __ASTAR__
#define __ASTAR__

#include <vector>
#include <functional>
#include <memory>

#include "State.hpp"

namespace SlidingBlocks
{
    using StatePtr = std::shared_ptr<State>;
    using StateList = std::vector<StatePtr>;
    using HeuristicFunction = std::function<int(const State&, const State&)>;

    struct Node;
    using NodePtr = std::shared_ptr<Node>;

    struct Node
    {
        int G, H;
        StatePtr State;
        NodePtr Parent;

        Node(StatePtr state, NodePtr parent = nullptr);

        int GetScore() const;
    };

    StateList FindPath(const State& source, const State& target, HeuristicFunction heuristic = ManhattanDistance);
}

#endif  // __ASTAR__
