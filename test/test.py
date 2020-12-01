import json
import argparse
import math
import os
import itertools
import pandas as pd


RESOURCE_ROOT = os.path.dirname(os.path.abspath(__file__))+"/Resources/"
LOGS_UNDER_TEST = {
    "queries_BAY_log.json": "queries_BAY_optimal_log.json",
    "queries_COL_log.json": "queries_COL_optimal_log.json",
    "queries_NE_log.json": "queries_NE_optimal_log.json",
    "queries_NY_log.json": "queries_NY_optimal_log.json",
}


def run_test(under_test_root, result_filename=None):
    failures = {}
    for log in LOGS_UNDER_TEST:
        print(f"Started testing {log}")
        log_file = under_test_root+log
        opt_log_file = RESOURCE_ROOT+LOGS_UNDER_TEST[log]

        boastar_fails = test_log_file(log_file, opt_log_file, "BOAStar")
        ppa_fails = test_log_file(log_file, opt_log_file, "PPA")

        if ((len(boastar_fails) != 0) or (len(ppa_fails) != 0)):
            failures[log] = {"BOAStar": boastar_fails, "PPA": ppa_fails}
            print(f"{log}: TEST FAILED")
        else:
            print(f"{log}: TEST PASSED")

    if result_filename != None:
        with open(result_filename, "w") as result_file:
            result_file.write(json.dumps(failures, indent=4))


def test_log_file(log_file, opt_log_file, algo_name):
    log_dict = log_file_to_test_dict(log_file, algo_name)
    opt_log_dict = log_file_to_test_dict(opt_log_file, "CBOAStar")

    failures = {}
    for src_dest in opt_log_dict:
        for eps in log_dict[src_dest]:
            non_dom = \
                test_domination(opt_log_dict[src_dest][(0, 0)],
                                log_dict[src_dest][eps],
                                eps)
            if len(non_dom) != 0:
                if str(src_dest) not in failures:
                    failures[str(src_dest)] = {}

                failures[str(src_dest)][str(eps)] = non_dom.values.tolist()

    return failures


def log_file_to_test_dict(log_filename, algo_name=None):
    with open(log_filename, "r") as log_file:
        logs = json.load(log_file)

    test_dict = {}
    for log in logs:
        if ((algo_name != None) and
            ("search_info" in log) and
            ("name" in log["search_info"]) and
            (log["search_info"]["name"] != algo_name)):
            continue

        src_dest_key = (log["source"], log["target"])
        if src_dest_key not in test_dict:
            test_dict[src_dest_key] = {}

        if (("search_info" in log) and
             ("eps" in log["search_info"])):
            eps_key = (log["search_info"]["eps"][0], log["search_info"]["eps"][1])
        else:
            eps_key = (0, 0)

        if eps_key in test_dict[src_dest_key]:
            print(f"Double log for algo:{algo_name}, src_dest:{src_dest_key}, eps:{eps_key}")
            continue

        solutions = log["finish_info"]["solutions"]
        test_dict[src_dest_key][eps_key] = \
            pd.DataFrame([[solution["cost_until_now"][0],
                           solution["cost_until_now"][1]]
                           for solution in solutions],
                         columns=["cost1", "cost2"])

    return test_dict


def test_domination(opt_solutions, apx_solutions, eps):
    uncovered_opt_solutions = opt_solutions
    for apx_solution in apx_solutions.iterrows():
        uncovered_opt_solutions = \
            uncovered_opt_solutions\
                [(apx_solution[1]["cost1"] > (uncovered_opt_solutions["cost1"]*(1+eps[0]))) | \
                 (apx_solution[1]["cost2"] > (uncovered_opt_solutions["cost2"]*(1+eps[1])))]

    return uncovered_opt_solutions



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Perform tests using generated logs")
    parser.add_argument("logs_under_test_root")
    parser.add_argument("test_results_filename", default=None)

    args = parser.parse_args()
    run_test(args.logs_under_test_root, args.test_results_filename)

