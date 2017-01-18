#include <vector>
#include <list>
#include <algorithm>
#include <memory>

#include "state.hpp"

#include "astar.hpp"


SlidingBlocks::Node::Node(SlidingBlocks::StatePtr state, SlidingBlocks::NodePtr parent):
        State(state), Parent(parent), G(0), H(0)
{
}

bool SlidingBlocks::Node::operator<(const SlidingBlocks::Node& other)
{
    return H < other.H;
}

std::vector<SlidingBlocks::StatePtr> SlidingBlocks::FindPath(const SlidingBlocks::State& source, const SlidingBlocks::State& target, SlidingBlocks::HeuristicFunction heuristic)
{
    std::list<NodePtr> openSet;
    std::vector<NodePtr> closedSet;
    NodePtr currentNode = nullptr;
    auto sourceNode = std::make_shared<Node>(std::make_shared<State>(source));
    sourceNode->H = heuristic(source, target);
    openSet.push_back(sourceNode);

    while (!openSet.empty())
    {
        auto currentPos = std::min_element(openSet.begin(), openSet.end());
        currentNode = *currentPos;
        if (target == *currentNode->State)
            break;

        openSet.erase(currentPos);
        closedSet.push_back(currentNode);

        auto move = [&target, heuristic, &openSet, &closedSet, currentNode](Step movement)
        {
            auto neighbor = currentNode->State->Move(movement);
            if (!neighbor)
                return;

            auto checkIfIsEqualToNeighbor = [neighbor](auto item)
            {
                return *neighbor == *item->State;
            };

            if (closedSet.end() != std::find_if(closedSet.begin(), closedSet.end(), checkIfIsEqualToNeighbor))
                return;

            int totalCost = currentNode->G + ManhattanDistance(*currentNode->State, *neighbor);
            auto successorPos = std::find_if(openSet.begin(), openSet.end(), checkIfIsEqualToNeighbor);
            NodePtr successor;
            if (openSet.end() == successorPos)
            {
                successor = std::make_shared<Node>(neighbor, currentNode);
                openSet.push_back(successor);
            }
            else
            {
                successor = *successorPos;
                if (totalCost >= successor->G)
                    return;
            }

            successor->Parent = currentNode;
            successor->G = totalCost;
            successor->H = successor->G + heuristic(*neighbor, target);
        };

        move(Step::Left);
        move(Step::Down);
        move(Step::Right);
        move(Step::Up);
    }

    std::vector<StatePtr> path;
    if (!openSet.empty())
        while (currentNode)
        {
            path.push_back(currentNode->State);
            currentNode = currentNode->Parent;
        }
    return path;
}
