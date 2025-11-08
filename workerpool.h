#pragma once

#include "Worker.h"
#include <iostream>
#include <deque>
#include <cstddef>

class WorkerPool {
private:
    std::deque<Worker> workers_; // Worker is pthread or std::thread
    bool start_flag_;
    size_t thread_cnt_;
public:
    WorkerPool(size_t thread_cnt) : thread_cnt_(thread_cnt == 0 ? 1 : thread_cnt), start_flag_(false){}
    ~WorkerPool() {
        stop_pool();
    }
    bool start_pool(){
        if(start_flag_==true){
            std::cerr << "[ERROR] already start thread pool "
			<< "(WorkerPool::start_pool) " << '\n';
			return false;
        }
        for (int i = 0; i < thread_cnt_; ++i) workers_.emplace_back();
        for(int i=0; i < thread_cnt_;i++){ 
            if(workers_[i].start_thread()==false){
                stop_pool();
                return false;
            }
        }
        start_flag_=true;
        return true;
    }
    void stop_pool(){
        if(start_flag_==true){
            for(int i=0; i < thread_cnt_;++i){ 
                workers_[i].stop_thread();
            }
            workers_.clear();
            start_flag_ = false;
        }
    }
    bool monitor_pool(){
        if(start_flag_==false){
            std::cerr << "[ERROR] don't start thread pool "
			<< "(WorkerPool::start_pool) " << '\n';
            return false;
        }
        int dead_cnt=0;
        int recovery_fail_cnt=0;
        for(int i=0;i<thread_cnt_;++i){
            if(workers_[i].get_thread_term()){
                dead_cnt++;
                workers_[i].stop_thread();
                if(workers_[i].start_thread()==false){
                    recovery_fail_cnt++;
                }
            }
        }
        std::cout << "LIVE THREAD COUNT : "<< thread_cnt_- dead_cnt<<" "<<"DEAD THREAD COUNT : "<< dead_cnt<<" "
        <<"RECOVERY SUCCESS THREAD COUNT : "<<dead_cnt-recovery_fail_cnt<<" "<<"RECOVERY FAIL THREAD COUNT : "<<recovery_fail_cnt<<" \n";
        if(recovery_fail_cnt>0){
            stop_pool();
            return false;
        }
        return true;
    }
}