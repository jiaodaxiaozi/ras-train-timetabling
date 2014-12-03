#ifndef DATA_H
#define DATA_H

#include <string>
#include <unordered_map>

#include <boost/container/vector.hpp>
#include <boost/property_tree/ptree.hpp>

#include <params.h>

template<typename T>
using bv = boost::container::vector<T>;

using bvec = bv<bool>;
using indicator_matrix = bv<bvec>;
using indicator_matrix_3d = bv<indicator_matrix>;
using indicator_matrix_4d = bv<indicator_matrix_3d>;
using vertices_map = indicator_matrix_3d;
using graph_adjacency_map = indicator_matrix_4d;

using ivec = bv<int>;
using int_matrix = bv<ivec>;
using int_matrix_3d = bv<int_matrix>;
using int_matrix_4d = bv<int_matrix_3d>;
using int_matrix_5d = bv<int_matrix_4d>;
using vertex_count_matrix = int_matrix_3d;

using dvec = bv<double>;
using double_matrix = bv<dvec>;
using double_matrix_3d = bv<double_matrix>;
using double_matrix_4d = bv<double_matrix_3d>;
using double_matrix_5d = bv<double_matrix_4d>;

using delay_price_map = std::unordered_map<char, double>;

/*
    SEGMENTS:       0 = sigma
                    1 ... ns = actual segments
                    ns + 1 = tau

    TRAINS:         0 ... nt

    TIME INTERVALS: 0 = reserved for sigma
                    1 ... ni = actual times (sigma and tau can also use them)
                    ni + 1 = reserved for tau
*/

struct data {
    int                 nt, // Number of trains
                        ns, // Number of segments
                        ni, // Number of time intervals
                        headway, // Time intervals between two consecutive trains
                        wt_tw_left, // Penalised if arriving at terminal with more than this advance
                        wt_tw_right, // Penalised if arriving at terminal with more than this delay
                        sa_tw_right; // Penalised if arriving at SA point with more than this delay

    double              wt_price, // Unit price for advance/delay at terminal
                        sa_price, // Unit price for delay at SA point
                        unpreferred_price; // Unit price for use of unpreferred track

    ivec                tr_wt, // Train's want time
                        sidings, // List of sidings
                        xovers, // List of cross-overs
                        sa_num; // (train => number of SA points)

    bv<char>            tr_class; // Train class ('A', 'B', 'C', 'D', 'E', 'F')

    bvec                tr_sa, // Is it a SA train?
                        tr_heavy; // Is the train heavy?
                        
    delay_price_map     delay_price; // Map (train type => unit price) of the penality to pay when not running at max speed

    indicator_matrix    v_for_someone; // ((segment, time) => bool) is true if there is at least one train s.t. v(train, segment, time) == true
    
    int_matrix          tr_orig_seg, // Train's origin segment(s) (train => segment1, ...)
                        tr_dest_seg, // Train's destination segment(s) (train => segment1, ...)
                        min_time_to_arrive_at, // ((train, segment) => time) minimum time for <train> to reach <segment> from the origin terminal
                        max_time_to_leave_from, // ((train, segment) => time) maximum time for <train> to leave <segment> to the destination terminal
                        min_travel_time, // ((train, segment) => time) minimum travel time on <segment> for <train>
                        max_travel_time, // ((train, segment) => time) maximum travel time on <segment> for <train> in order for the penalty to be < UB
                        main_tracks, // (segment => segment1, ...) list of mains if <segment> is a siding, or an empty list
                        unpreferred_segments, // (train => segment1, ...) list of unpreferred segments for <train>
                        sa_times; // ((train, sa#) => time) time at which <train> should be at the <sa#>-th SA point

    int_matrix_3d       tnetwork, // ((train, segment) => segment1, ...) list of segments connected to <segment> in <train>'s direction [includes itself]
                        inverse_tnetwork, // ((train, segment) => segment1, ...) list of segments s.c. <segment> is connected to them in <train>'s direction [include itself]
                        bar_tnetwork, // ((train, segment) => segment1, ...) list of segments connected to <segment> in <train>'s direction
                        bar_inverse_tnetwork, // ((train, segment) => segment1, ...) list of segments s.c. <segment> is connected to them in <train>'s direction
                        trains_for, // ((segment, time) => train1, ...) list of trains for which v(train, segment, time) == true
                        segments_for_sa; // ((train, sa#) => segment1, ...) segments for <train>'s <sa#>-th SA point
    
    vertices_map        v; // ((train, segment, time) => bool) is true if vertex (<train>, <segment>) is in <train>'s graph

    graph_adjacency_map adj; // ((train, segment1, time, segment2, [time + 1]) => bool) is the adjacency matrix of the graph of <train>
    
    data(const std::string& file_name);
    
private:
    std::string         file_name, // Data file name
                        instance_name; // Instance name

    double              speed_ew, // Maximum speed East -> West
                        speed_we, // Maximum speed West -> East
                        speed_siding, // Maximum speed in sidings
                        speed_switch, // Maximum speed in switches
                        speed_xover, // Maximum speed in cross-overs
                        total_cost_ub; // Upper bound on the total cost of the solution
                        
    ivec                seg_e_ext, // Easternmost extreme points of segments
                        seg_w_ext, // Westernmost extreme points of segments
                        tr_entry_time, // Train's entry time
                        tr_orig_ext, // Train's origin extreme
                        tr_dest_ext, // Train's destination extreme
                        tr_tob, // Train's tons per operating brake
                        mow_ext_e, // Mow east extremes
                        mow_ext_w, // Mow west extremes
                        mow_start_times, // Mow start times
                        mow_end_times, // Mow end times
                        last_time_we_need_sigma, // Create sigma from 0 to this time
                        first_time_we_need_tau; // Create taut from this time to ni+1
    
    dvec                seg_e_min_dist, // Min distance from east terminal to segment
                        seg_w_min_dist, // Min distance from west terminal to segment
                        seg_length, // Length of segment
                        seg_siding_length, // "Real" length of a siding (i.e. wthout the switch)
                        tr_speed_mult, // Train's speed multiplier
                        tr_length, // Train's length
                        tr_max_speed; // Max speed of a train on the tracks
    
    bv<char>            seg_type; // Segment type ('0', '1', '2', 'S', 'T', 'X', 'D') - 'D' used for sigma, tau

    bvec                seg_eastbound, // Is the segment's preferred direction eastbound?
                        seg_westbound, // Is the segment's preferred direction westbound?
                        tr_eastbound, // Is the train eastbound?
                        tr_westbound, // Is the train westbound?
                        tr_hazmat; // Is the train hazmat?
                        
    int_matrix          sa_ext; // (train => extreme1, ...) is the raw list of SA points for <train>

    indicator_matrix    mow, // ((segment, time) => bool) is true if there is a mow at <segment> at time <time>
                        network, // ((segment1, segment2) => bool) is true if <segment1> and <segment2> are connected
                        unpreferred; // ((train, segment) => bool) is true if <segment> is unpreferred for <train>

    vertex_count_matrix n_in, // ((train, segment, time) => n) is the number of arcs going into the vertex
                        n_out; // ((train, segment, time) => n) is the number of arcs going out of the vertex

    auto read_speeds(const boost::property_tree::ptree& pt);
    auto read_relevant_times(const boost::property_tree::ptree& pt);
    auto read_prices(const boost::property_tree::ptree& pt);
    auto read_segments(const boost::property_tree::ptree& pt);
    auto read_trains(const boost::property_tree::ptree& pt);
    auto read_mows(const boost::property_tree::ptree& pt);

    auto calculate_train_orig_dest_segments();
    auto calculate_mows();
    auto calculate_schedules();
    auto calculate_network();
    auto calculate_auxiliary_data();
    auto calculate_vertices();
    auto calculate_adjacency();
    auto calculate_costs();
    
    auto generate_sigma_s_arcs();
    auto generate_s_tau_arcs();
    auto generate_stop_arcs();
    auto generate_movement_arcs();
    
    auto cleanup_adjacency();
    
    auto print_adjacency() const;
};

#endif