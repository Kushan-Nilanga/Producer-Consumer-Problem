//
//  utils.h
//  prod-con
//
//  Created by Don Kushan Athalage on 30/8/21.
//

#ifndef utils_h
#define utils_h

#include <cstdlib>
#include <thread>

std::thread* create_threads(uint8_t num_threads, void* func(int, int), int len){
    int batch_size = len / num_threads;
    std::thread *threads = new std::thread[num_threads];
    for(uint8_t i = 0; i < num_threads; i++){
        threads[i] = std::thread(func, i * batch_size, (i + 1) * batch_size);
    }
    return threads;
};


std::thread* create_threads(uint8_t num_threads, void* func(void)){
    std::thread *threads = new std::thread[num_threads];
    for(uint8_t i = 0; i < num_threads; i++){
        threads[i] = std::thread(func);
    }
    
    return threads;
}

void join_threads(uint8_t num_threads, std::thread* threads){
    for(uint8_t i = 0; i < num_threads; i++){
        threads[i].join();
    }
}


#endif /* utils_h */
