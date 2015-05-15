/*
 * solver_heuristic.h
 *
 *  Created on: 07/mag/2015
 *      Author: giacomo
 */

#ifndef SOLVER_SOLVER_HEURISTIC_H_
#define SOLVER_SOLVER_HEURISTIC_H_

#include <data/array.h>
#include <data/data.h>
#include <data/path.h>
#include <set>

#include <boost/optional.hpp>

using node = path::node;

struct solver_heuristic {
public:
	data& d;
	unsigned int train;

	auto solve() -> path;
	solver_heuristic(data& d, auto train): d{d}, train{train} {}

private:
	auto dijkstra_extra_greedy(graph * gr, trains * trn, auto start, auto src_segments, auto dst_segments) -> bv<node>;
	auto simple_single_scheduler(graph * gr, trains * trn, auto start, auto src_segments, auto dst_segments) -> bv<node>;
};

#endif /* SOLVER_SOLVER_HEURISTIC_H_ */
