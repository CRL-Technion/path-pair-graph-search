#include <iostream>
#include <set>
#include <string>
#include "Definitions.h"

AdjacencyMatrix::AdjacencyMatrix(size_t graph_size, std::vector<Edge> &edges, bool inverse)
    : matrix((graph_size+1), std::vector<Edge>()), graph_size(graph_size) {

    for (auto iter = edges.begin(); iter != edges.end(); ++iter) {
        if (inverse) {
            this->add(iter->inverse());
        } else {
            this->add((*iter));
        }
    }
}


void AdjacencyMatrix::add(Edge edge) {
    (this->matrix[edge.source]).push_back(edge);
}


size_t AdjacencyMatrix::size() const {return this->graph_size;}


const std::vector<Edge>& AdjacencyMatrix::operator[](size_t vertex_id) const {
    return this->matrix.at(vertex_id);
}


std::ostream& operator<<(std::ostream &stream, const AdjacencyMatrix &adj_matrix) {
    size_t  i = 0;

    stream << "{\n";
    for (auto vertex_iter = adj_matrix.matrix.begin(); vertex_iter != adj_matrix.matrix.end(); ++vertex_iter) {
        stream << "\t\"" << i++ << "\": [";

        std::vector<Edge> edges = *vertex_iter;
        for (auto edge_iter = edges.begin(); edge_iter != edges.end(); ++edge_iter) {
            stream << "\"" << edge_iter->source << "->" << edge_iter->target << "\", ";
        }

        stream << "],\n";
    }
    stream << "}";
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const Edge &edge) {
    // Printed in JSON format
    stream
        << "{"
        <<  "\"edge_source\": " << edge.source << ", "
        <<  "\"edge_target\": " << edge.target << ", "
        <<  "\"edge_cost\": " << edge.cost
        << "}";

    return stream;
}


bool Node::more_than_specific_heurisitic_cost::operator()(const NodePtr &a, const NodePtr &b) const {
    return (a->h[cost_idx] > b->h[cost_idx]);
}


bool Node::more_than_full_cost::operator()(const NodePtr &a, const NodePtr &b) const {
    if (a->f[0] != b->f[0]) {
        return (a->f[0] > b->f[0]);
    } else {
        return (a->f[1] > b->f[1]);
    }
}


std::ostream& operator<<(std::ostream &stream, const Node &node) {
    // Printed in JSON format
    std::string parent_id = node.parent == nullptr ? "-1" : std::to_string(node.parent->id);
    stream
        << "{"
        <<      "\"id\": " << node.id << ", "
        <<      "\"parent\": " << parent_id << ", "
        <<      "\"cost_until_now\": " << node.g << ", "
        <<      "\"heuristic_cost\": " << node.h << ", "
        <<      "\"full_cost\": " << node.f
        << "}";
    return stream;
}

bool PathPair::update_nodes_by_merge_if_bounded(const PathPairPtr &other, const Pair<double> eps) {
    // Returns true on sucessful merge and false if it failure
    if (this->id != other->id) {
        return false;
    }

    NodePtr new_top_left;
    NodePtr new_bottom_right;

    // Pick node with min cost1 (min cost2 if equal)
    if ((this->top_left->f[0] < other->top_left->f[0]) ||
        ((this->top_left->f[0] == other->top_left->f[0]) && (this->top_left->f[1] < other->top_left->f[1]))) {
        new_top_left = this->top_left;
    } else {
        new_top_left = other->top_left;
    }

    // Pick node with min cost2 (min cost1 if equal)
    if ((this->bottom_right->f[1] < other->bottom_right->f[1]) ||
        ((this->bottom_right->f[1] == other->bottom_right->f[1]) && (this->bottom_right->f[0] < other->bottom_right->f[0]))) {
        new_bottom_right = this->bottom_right;
    } else {
        new_bottom_right = other->bottom_right;
    }

    // Check if path pair is bounded after merge - if not the merge is illegal
    if ((((1+eps[0])*new_top_left->g[0]) < new_bottom_right->g[0]) ||
        (((1+eps[1])*new_bottom_right->g[1]) < new_top_left->g[1])) {
        return false;
    }

    this->top_left = new_top_left;
    this->bottom_right = new_bottom_right;
    return true;
}


bool PathPair::more_than_full_cost::operator()(const PathPairPtr &a, const PathPairPtr &b) const {
    if (a->top_left->f[0] != b->top_left->f[0]) {
        return (a->top_left->f[0] > b->top_left->f[0]);
    } else {
        return (a->bottom_right->f[1] > b->bottom_right->f[1]);
    }
}

std::ostream& operator<<(std::ostream &stream, const PathPair &pp) {
    // Printed in JSON format
    stream << "{" << pp.top_left << ", " << pp.bottom_right << "}";
    return stream;
}
