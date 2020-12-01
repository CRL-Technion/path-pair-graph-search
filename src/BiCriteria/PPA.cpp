#include <memory>
#include <vector>

#include "PPA.h"


PPA::PPA(const AdjacencyMatrix &adj_matrix, Pair<double> eps, const LoggerPtr logger) :
    adj_matrix(adj_matrix), eps(eps), logger(logger) {}


void PPA::insert(PathPairPtr &pp, PPQueue &queue) {
    std::vector<PathPairPtr> &relevant_pps = queue.get_open_pps(pp->id);
    for (auto existing_pp = relevant_pps.begin(); existing_pp != relevant_pps.end(); ++existing_pp) {
        if ((*existing_pp)->is_active == false) {
            continue;
        }
        if (pp->update_nodes_by_merge_if_bounded(*existing_pp, this->eps) == true) {
            // pp and existing_pp were merged successfuly into pp
            if ((pp->top_left != (*existing_pp)->top_left) ||
                (pp->bottom_right != (*existing_pp)->bottom_right)) {
                // If merged_pp == existing_pp we avoid inserting it to keep the queue as small as possible.
                // existing_pp is deactivated and not removed to avoid searching through the heap
                // (it will be removed on pop and ignored)
                (*existing_pp)->is_active = false;
                queue.insert(pp);
            }
            return;
        }
    }
    queue.insert(pp);
}


void PPA::merge_to_solutions(const PathPairPtr &pp, PPSolutionSet &solutions) {
    for (auto existing_solution = solutions.begin(); existing_solution != solutions.end(); ++existing_solution) {
        if ((*existing_solution)->update_nodes_by_merge_if_bounded(pp, this->eps) == true) {
            return;
        }
    }
    solutions.push_back(pp);
}


void PPA::operator()(size_t source, size_t target, Heuristic &heuristic, SolutionSet &solutions) {
    this->start_logging(source, target);

    PPSolutionSet pp_solutions;
    PathPairPtr   pp;
    PathPairPtr   next_pp;

    // Saving all the unused PathPairPtrs in a vector improves performace for some reason
    std::vector<PathPairPtr> closed;

    // Vector to hold mininum cost of 2nd criteria per node
    std::vector<size_t> min_g2(this->adj_matrix.size()+1, MAX_COST);

    // Init open heap
    PPQueue open(this->adj_matrix.size()+1);

    NodePtr source_node = std::make_shared<Node>(source, Pair<size_t>({0,0}), heuristic(source));
    pp = std::make_shared<PathPair>(source_node, source_node);
    open.insert(pp);

    while (open.empty() == false) {
        // Pop min from queue and process
        pp = open.pop();

        // Optimization: PathPairs are being deactivated instead of being removed so we skip them.
        if (pp->is_active == false) {
            continue;
        }

        // Dominance check
        if ((((1+this->eps[1])*pp->bottom_right->f[1]) >= min_g2[target]) ||
            (pp->bottom_right->g[1] >= min_g2[pp->id])) {
            closed.push_back(pp);
            continue;
        }
        min_g2[pp->id] = pp->bottom_right->g[1];

        if (pp->id == target) {
            this->merge_to_solutions(pp, pp_solutions);
            continue;
        }

        // Check to which neighbors we should extend the paths
        const std::vector<Edge> &outgoing_edges = adj_matrix[pp->id];
        for(auto p_edge = outgoing_edges.begin(); p_edge != outgoing_edges.end(); p_edge++) {
            // Prepare extension of path pair
            size_t next_id = p_edge->target;
            Pair<size_t> top_left_next_g = {pp->top_left->g[0]+p_edge->cost[0],
                                            pp->top_left->g[1]+p_edge->cost[1]};
            Pair<size_t> bottom_right_next_g = {pp->bottom_right->g[0]+p_edge->cost[0],
                                                pp->bottom_right->g[1]+p_edge->cost[1]};
            Pair<size_t> next_h = heuristic(next_id);

            // Dominance check
            if ((((1+this->eps[1])*(bottom_right_next_g[1]+next_h[1])) >= min_g2[target]) ||
                (bottom_right_next_g[1] >= min_g2[next_id])) {
                continue;
            }

            // If not dominated extend path pair and push to queue
            // Creation is defered after dominance check as it is
            // relatively computational heavy and should be avoided if possible
            next_pp = std::make_shared<PathPair>(
                            std::make_shared<Node>(next_id, top_left_next_g, next_h, pp->top_left),
                            std::make_shared<Node>(next_id, bottom_right_next_g, next_h, pp->top_left));

            this->insert(next_pp, open);

            closed.push_back(pp);
        }
    }

    // Pair solutions is used only for logging, as we need both the solutions for testing reasons
    SolutionSet pair_solutions;
    for (auto solution = pp_solutions.begin(); solution != pp_solutions.end(); ++solution) {
        solutions.push_back((*solution)->top_left);
        pair_solutions.push_back((*solution)->top_left);
        pair_solutions.push_back((*solution)->bottom_right);
    }

    this->end_logging(pair_solutions);
}


void PPA::start_logging(size_t source, size_t target) {
    // All logging is done in JSON format
    std::stringstream start_info_json;
    start_info_json
        << "{\n"
        <<      "\t\"name\": \"PPA\",\n"
        <<      "\t\"eps\": " << this->eps << "\n"
        << "}";

    if (this->logger != nullptr) {
        LOG_START_SEARCH(*this->logger, source, target, start_info_json.str());
    }
}


void PPA::end_logging(SolutionSet &solutions) {
    // All logging is done in JSON format
    std::stringstream finish_info_json;
    finish_info_json
        << "{\n"
        <<      "\t\"solutions\": [";

    size_t solutions_count = 0;
    for (auto solution = solutions.begin(); solution != solutions.end(); ++solution) {
        if (solution != solutions.begin()) {
            finish_info_json << ",";
        }
        finish_info_json << "\n\t\t" << **solution;
        solutions_count++;
    }

    finish_info_json
        <<      "\n\t],\n"
        <<      "\t\"amount_of_solutions\": " << solutions_count << "\n"
        << "}" <<std::endl;

    if (this->logger != nullptr) {
        LOG_FINISH_SEARCH(*(this->logger), finish_info_json.str());
    }
}