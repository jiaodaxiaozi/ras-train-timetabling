std::cout << "\tColumns e" << std::endl;

for(int i = 0; i < nt; i++) {
    if(d->trains[i].schedule_adherence) {
        std::vector<int> segs;
        std::vector<int> times;
        
        for(auto kv : d->trains[i].schedule) {
            segs.push_back(kv.first);
            times.push_back(kv.second);
        }
        
        for(int s = 0; s < d->trains[i].schedule.size(); s++) {
            // Variable e^i_s
        
            double c_sa_i {d->schedule_delay_price};
        
            // ********** Obj Function **********
            double obj_coeff {0};
            obj_coeff = c_sa_i;
        
            IloNumColumn col = obj(obj_coeff);
        
            // ********** eq_exit_sigma **********
            for(int ii = 0; ii < nt; ii++) {
                int coeff {0};
                col += eq_exit_sigma[ii](coeff);
            }
        
            // ********** eq_enter_tau **********
            for(int ii = 0; ii < nt; ii++) {
                int coeff {0};
                col += eq_enter_tau[ii](coeff);
            }
        
            // ********** eq_one_train **********
            int num_row {0};
            for(int ss = 0; ss < ns; ss++) {
                for(int tt = 1; tt <= ti; tt++) {
                    int coeff {0};
                    col += eq_one_train[num_row++](coeff);
                }
            }
        
            // ********** eq_flow **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n {graphs[ii]->g[*vi]};
                    if(n.s != nullptr) {
                        int coeff {0};
                        col += eq_flow[num_row++](coeff);
                    }
                }
            }
            
            // ********** eq_ensure_sa_visit **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                if(d->trains[ii].schedule_adherence) {
                    for(int chk = 0; chk < d->trains[ii].schedule.size(); chk++) {
                        int coeff {0};
                        col += eq_ensure_sa_visit[num_row++](coeff);
                    }
                }
            }
            
            // ********** eq_wt_1 **********
            for(int ii = 0; ii < nt; ii++) {
                int coeff {0};
                col += eq_wt_1[ii](coeff);
            }
        
            // ********** eq_wt_2 **********
            for(int ii = 0; ii < nt; ii++) {
                int coeff {0};
                col += eq_wt_2[ii](coeff);
            }
            
            // ********** eq_sa_delay **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                if(d->trains[ii].schedule_adherence) {
                    for(auto kv : d->trains[ii].schedule) {
                        int coeff {0};
                        
                        if(i == ii) {
                            coeff = -1;
                        }
                        
                        col += eq_sa_delay[num_row++](coeff);
                    }
                }
            }

            // ********** eq_min_time **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n = graphs[ii]->g[*vi];
                    if(n.s != nullptr) {
                        int coeff {0};
                        col += eq_min_time[num_row++](coeff);
                    }
                }
            }
            
            // ********** eq_exact_time **********
            // num_row = 0;
            // for(int ii = 0; ii < nt; ii++) {
            //     vi_t vi, vi_end;
            //     for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
            //         const Node& n = graphs[ii]->g[*vi];
            //         if(n.s != nullptr && (n.s->type == 'T' || n.s->type == 'X')) {
            //             int coeff {0};
            //             col += eq_exact_time[num_row++](coeff);
            //         }
            //     }
            // }
        
            // ********** eq_headway1 **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n = graphs[ii]->g[*vi];
                    if(n.s != nullptr) {
                        int coeff {0};
                        col += eq_headway1[num_row++](coeff);
                    }
                }
            }
    
            // ********** eq_headway2 **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n = graphs[ii]->g[*vi];
                    if(n.s != nullptr) {
                        int coeff {0};
                        col += eq_headway2[num_row++](coeff);
                    }
                }
            }
    
            // ********** eq_headway3 **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n = graphs[ii]->g[*vi];
                    if(n.s != nullptr) {
                        int coeff {0};
                        col += eq_headway3[num_row++](coeff);
                    }
                }
            }
    
            // ********** eq_headway4 **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n = graphs[ii]->g[*vi];
                    if(n.s != nullptr) {
                        int coeff {0};
                        col += eq_headway4[num_row++](coeff);
                    }
                }
            }
            
            // ********** eq_can_take_siding **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                vi_t vi, vi_end;
                for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                    const Node& n = graphs[ii]->g[*vi];
                    if(n.s != nullptr && n.s->type == 'S') {
                        int coeff {0};
                        col += eq_can_take_siding[num_row++](coeff);
                    }
                }
            }
    
            // ********** eq_heavy_siding **********
            num_row = 0;
            for(int ii = 0; ii < nt; ii++) {
                if(d->trains[ii].heavy) {
                    vi_t vi, vi_end;
                    for(std::tie(vi, vi_end) = vertices(graphs[ii]->g); vi != vi_end; ++vi) {
                        const Node& n = graphs[ii]->g[*vi];
                        if(n.s != nullptr && n.s->type == 'S') {
                            int coeff {0};
                            col += eq_heavy_siding[num_row++](coeff);
                        }
                    }
                }
            }
            
            col += eq_positive_obj[0](obj_coeff);
        
            // We strengthen the bound on e by calculating the maximum possible delay
            int max_delay {std::max(times[s], ti - times[s])};
            
            IloNumVar v(col, 0.0, max_delay, IloNumVar::Int, (
                "X_e_train_" + std::to_string(i) + "_segment_" + std::to_string(segs[s])
            ).c_str());
            var_e.add(v);
        }
    }
}