/*
 * sequential_solver_heuristic.cpp
 *
 *  Created on: 07/mag/2015
 *      Author: simone
 */



#include <solver/sequential_solver_heuristic.h>
#include <solver/solver_heuristic.h>

#include <iostream>
#include <fstream>



auto sequential_solver_heuristic::solve_sequentially() -> boost::optional<bv<path>> {
	auto trains_to_schedule = uint_vector();
	bv<path> paths;

	for(auto i = 0u; i < d.nt; i++) {
		paths.push_back(path(d, i));
	}

	for (unsigned int train = 0; train < d.nt; train++) {
		auto local_d = d;

		//Saves the v matrix for the current train
		auto name = "v" + std::to_string(train) + ".txt";

		std::ofstream f (name);
		for(unsigned int i=0; i<d.ns+2; i++){
			for(unsigned int j=0; j<d.ni+1; j++){
				f<<d.gr.v.at(train).at(i).at(j);
			}
			f<<"\n";
		}


		trains_to_schedule.push_back(train);
		paths.at(train).make_empty();

		std::cout << "SEQUENTIAL_SOLVER >> Trains to schedule: ";
		std::copy(trains_to_schedule.begin(), trains_to_schedule.end(), std::ostream_iterator<unsigned int>(std::cout, " "));
		std::cout << std::endl;

		local_d.gr.only_trains(trains_to_schedule, d.nt, d.ns, d.ni);
		constrain_graph_by_paths(local_d.gr, paths);

		auto s = solver_heuristic(local_d,train); //crea e invoca il solver
		auto p_sol = s.solve();

		paths.at(train) = p_sol;
		std::cout << "SEQUENTIAL_SOLVER >> Paths: "<< std::endl;
		for(const auto& p : paths) {
			if(!p.is_dummy()) {
				std::cout << "\tTrain " << p.train << ", cost: " << p.cost << std::endl;
			}
		}
	}
	return paths;
}

auto sequential_solver_heuristic::constrain_graph_by_paths(graph& gr, const bv<path>& paths) -> void {
    assert(paths.size() == d.nt);

    for(auto i = 0u; i < d.nt; i++) {
        if(paths.at(i).is_empty()) {
            continue;
        } else if(paths.at(i).is_dummy()) {
            gr.adj.at(i).at(0).at(0).at(d.ns + 1) = true;
            gr.n_out.at(i).at(0).at(0)++;
            gr.n_in.at(i).at(d.ns + 1).at(1)++;
            if(!gr.v.at(i).at(0).at(0)) {
                gr.v.at(i).at(0).at(0) = true;
                gr.n_nodes.at(i)++;
            }
            if(!gr.v.at(i).at(d.ns + 1).at(1)) {
                gr.v.at(i).at(d.ns + 1).at(1) = true;
                gr.n_nodes.at(i)++;
            }
            gr.v_for_someone.at(0).at(0) = true;
            gr.v_for_someone.at(d.ns + 1).at(1) = true;
        } else {
            fix_path_for(gr, paths.at(i));

            for(auto j = 0u; j < d.nt; j++) {
                if(j != i) {
                    remove_incompatible(gr, j, paths.at(i));
                }
            }
        }
    }

    gr.cleanup(d.nt, d.ns, d.ni);
}

auto sequential_solver_heuristic::fix_path_for(graph& gr, const path& p) -> void {
    for(auto s1 = 0u; s1 <= d.ns + 1; s1++) {
        for(auto t = 0u; t <= d.ni; t++) {
            for(auto s2 = 0u; s2 <= d.ns + 1; s2++) {
                if(gr.adj.at(p.train).at(s1).at(t).at(s2) && !p.x.at(s1).at(t).at(s2)) {
                    gr.adj.at(p.train).at(s1).at(t).at(s2) = false;
                    gr.n_out.at(p.train).at(s1).at(t)--;
                    gr.n_in.at(p.train).at(s2).at(t + 1)--;
                    gr.n_arcs.at(p.train)--;
                }
            }
        }
    }
}

auto sequential_solver_heuristic::remove_incompatible(graph& gr, unsigned int j, const path& p) -> void {
    for(const auto& n : p.p) {
        auto start_time = std::max(1u, n.t - d.headway);
        auto end_time = std::min(d.ni, n.t + d.headway);

        for(auto t = start_time; t <= end_time; t++) {
            for(auto s : gr.delta.at(j).at(n.seg)) {
                if(gr.adj.at(j).at(n.seg).at(t).at(s)) {
                    gr.adj.at(j).at(n.seg).at(t).at(s) = false;
                    gr.n_out.at(j).at(n.seg).at(t)--;
                    gr.n_in.at(j).at(s).at(t + 1)--;
                    gr.n_arcs.at(j)--;
                }
            }
            for(auto s : gr.inverse_delta.at(j).at(n.seg)) {
                if(gr.adj.at(j).at(s).at(t - 1).at(n.seg)) {
                    gr.adj.at(j).at(s).at(t - 1).at(n.seg) = false;
                    gr.n_out.at(j).at(s).at(t - 1)--;
                    gr.n_in.at(j).at(n.seg).at(t)--;
                    gr.n_arcs.at(j)--;
                }
            }
        }
    }
}
