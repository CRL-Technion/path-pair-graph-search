#ifndef UTILS_DEFINITIONS_H
#define UTILS_DEFINITIONS_H

#include <map>
#include <vector>
#include <array>
#include <list>
#include <iostream>
#include <limits>
#include <functional>
#include <memory>


#ifndef DEBUG
    #define DEBUG 1
#endif


const size_t MAX_COST = std::numeric_limits<size_t>::max();


template<typename T>
using Pair      = std::array<T, 2>;

template<typename T>
std::ostream& operator<<(std::ostream &stream, const Pair<T> pair) {
    stream << "[" << pair[0] << ", " << pair[1] << "]";
    return stream;
}


using Heuristic = std::function<Pair<size_t>(size_t)>;


// Structs and classes
struct Edge {
    size_t          source;
    size_t          target;
    Pair<size_t>    cost;

    Edge(size_t source, size_t target, Pair<size_t> cost) : source(source), target(target), cost(cost) {}
    Edge inverse() {
        return Edge(this->target, this->source, this->cost);
    }
};
std::ostream& operator<<(std::ostream &stream, const Edge &edge);


// Graph representation as adjacency matrix
class AdjacencyMatrix {
private:
    std::vector<std::vector<Edge>> matrix;
    size_t                         graph_size;

public:
    AdjacencyMatrix() = default;
    AdjacencyMatrix(size_t graph_size, std::vector<Edge> &edges, bool inverse=false);
    void add(Edge edge);
    size_t size(void) const;
    const std::vector<Edge>& operator[](size_t vertex_id) const;

    friend std::ostream& operator<<(std::ostream &stream, const AdjacencyMatrix &adj_matrix);
};


struct Node;
struct PathPair;
using NodePtr       = std::shared_ptr<Node>;
using PathPairPtr   = std::shared_ptr<PathPair>;
using SolutionSet   = std::vector<NodePtr>;
using PPSolutionSet = std::vector<PathPairPtr>;


struct Node {
    size_t          id;
    Pair<size_t>    g;
    Pair<size_t>    h;
    Pair<size_t>    f;
    NodePtr         parent;

    Node(size_t id, Pair<size_t> g, Pair<size_t> h, NodePtr parent=nullptr)
        : id(id), g(g), h(h), f({g[0]+h[0],g[1]+h[1]}), parent(parent) {};

    struct more_than_specific_heurisitic_cost {
        size_t cost_idx;

        more_than_specific_heurisitic_cost(size_t cost_idx) : cost_idx(cost_idx) {};
        bool operator()(const NodePtr &a, const NodePtr &b) const;
    };

    struct more_than_full_cost {
        bool operator()(const NodePtr &a, const NodePtr &b) const;
    };

    friend std::ostream& operator<<(std::ostream &stream, const Node &node);
};


struct PathPair {
    size_t      id;
    NodePtr     top_left;
    NodePtr     bottom_right;
    NodePtr     parent;
    bool        is_active=true;

    PathPair(const NodePtr &top_left, const NodePtr &bottom_right)
        : id(top_left->id), top_left(top_left), bottom_right(bottom_right), parent(top_left->parent) {};

    bool update_nodes_by_merge_if_bounded(const PathPairPtr &other, const Pair<double> eps);

    struct more_than_full_cost {
        bool operator()(const PathPairPtr &a, const PathPairPtr &b) const;
    };

    friend std::ostream& operator<<(std::ostream &stream, const PathPair &pp);
};

#endif //UTILS_DEFINITIONS_H
