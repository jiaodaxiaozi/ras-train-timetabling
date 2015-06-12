/*
 * solver_heuristic.cpp
 *
 *  Created on: 07/mag/2015
 *      Author: giacomo
 */

#include <solver/solver_heuristic.h>
#include <data/path.h>
#include <assert.h>


//TODO: SUBSTITUTE SQUARE BRACKETS WITH .at(i)

auto solver_heuristic::solve()-> path{

	return path(d, train, simple_single_scheduler(), cost);

}

/*
 * Simple scheduler that schdules train on an empty network.
 * It returns the path as a vector of nodes.
 */

auto solver_heuristic::simple_single_scheduler() -> bv<node>{

	/* The starting segments */
	auto start_segs = d.trn.orig_segs.at(train);

	/* The time at we are NOW. Of course the first "now" time is the entry time of train */
	auto now = d.trn.entry_time.at(train);

	/* The sequence of nodes. It's the path that returns at the end of the function */
	bv<node> seq;

	/* The segments were we are at time now */
	unsigned int here = 0;

	/* Initially, we are here and now (you don't say?)*/
	seq.push_back(node(here,now));

	/*
	 * best is the pair (segment, score) that represents the best next segments analyzed and its score (the lower the better)
	 * We start with all (0,5), where any score > 2 is invalid.
	 */
	std::pair<unsigned int, unsigned int> best = std::make_pair(0,5);;

	/*
	 * For the origin segments we have scores:
	 * 0 - Main track preferred
	 * 1 - Main track unpreferred
	 * 2 - Siding
	 */
	for(const unsigned int& s : start_segs){

		switch(d.seg.type.at(s)) {
		case '0':
			best = std::make_pair(s,0);
			break;

		case '1':
		case '2':
			if(!d.net.unpreferred.at(train).at(s))
				best = std::make_pair(s,0);
			else if (best.second > 1)
				best = std::make_pair(s,1);
			break;

		case 'S':
			if(best.second > 2)
				best = std::make_pair(s,2);
			break;
		}
		/*
		 * Old if-then-else model. Keep it here for safe
		 *
		if(d.seg.type.at(s)>='0' && d.seg.type.at(s)<='2'){
			if(!d.net.unpreferred.at(train).at(s)){
				best = std::make_pair(s,0);
				break;
			}
			else{
				if(best.second>1)
					best = std::make_pair(s,1);
			}
		}
		else{
			if(d.seg.type.at(s)=='S'){
				if(best.second>2)
					best = std::make_pair(s,2);
			}
		}
		*/
	}
	/* Verify if everything went ok and best did not remained (0,5) */
	assert(best.second<3);

	/* Boolean value that comes true if we arrive at the end of the travel or if we ran out of time */
	bool finished = false;

	/* Literaly, wait for the segments to become free, then travel through it */
	wait_and_travel(here, best.first, now, seq, finished);

	/*
	 * Choosing next segments starting from here and now, then wait for it to be free and travel through it.
	 * Checking if we arrived at the last segments. If yes, finished becomes true and we exit the loop.
	 * At the end, here becomes next.
	 */
	unsigned int next;
	while(!finished){
		next = choose_next(here, now);
		wait_and_travel(here, next, now, seq, finished);
		for(const unsigned int& s : d.trn.dest_segs.at(train)) {
			finished = (next==s) || finished;
		}
		here = next;
	}
	seq.push_back(node(d.ns+1,now++));
	/* Returns the sequence of nodes */
	return seq;
}

auto solver_heuristic::wait_and_travel(unsigned int here, unsigned int next, unsigned int& now, bv<node> &seq, bool &finished) -> void{
	unsigned int t = now+1;
	while(unsigned int wt = wait_time(next, t-1)){
		t+=wt;
	}
	now++;
	while(now < t && !finished){
		//cost increases by the arc cost from the last visited segment at now to the current segment at now+1
		cost+=d.gr.costs.at(train).at(seq.at(seq.size()).seg).at(now).at(here);
		cost+=d.pri.delay.at(d.trn.type.at(train));
		seq.push_back(node(here,now++));
		finished = (now > d.ni) || finished;
	}
	unsigned int end_of_travel = now+d.net.min_travel_time.at(train).at(next);
	while(now < end_of_travel && !finished) {
		cost+=d.gr.costs.at(train).at(seq.at(seq.size()).seg).at(now).at(next);
		seq.push_back(node(next,now++));
		finished = (now > d.ni) || finished;
	}
}

auto solver_heuristic::wait_time(unsigned int seg, unsigned int now) -> unsigned int{
	unsigned int
			t = now,
			time_to_wait=now;
	bool busy = false;
	while(t<=now+d.net.min_travel_time.at(train).at(seg) || busy){
		busy = false;
		if(!d.gr.v.at(train).at(seg).at(t)){
			time_to_wait = t+1;
			busy = true;
		}
		t++;
	}
	return time_to_wait-now;
}

auto inline solver_heuristic::choose_next(unsigned int here, unsigned int now) -> unsigned int{
	std::pair<unsigned int, unsigned int> best = std::make_pair(0,5);;
		for(const auto& s : d.gr.bar_delta.at(train).at(here)){
			if(d.seg.type.at(s)>='0' && d.seg.type.at(s)<='2'){
				if(!d.net.unpreferred.at(train).at(s)){
					best = std::make_pair(s,0);
					break;
				}
				else if(best.second>3) {
					best = std::make_pair(s,3);
				}
			}
			else if(d.seg.type.at(s)=='S' && best.second > 1) {
				best = std::make_pair(s,1);
			}
			else if(d.seg.type.at(s)=='X' && best.second > 2){
				best = std::make_pair(s,2);
			}
		}
		assert(best.second<4);

		return best.first;
}

