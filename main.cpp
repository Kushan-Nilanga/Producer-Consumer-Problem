/*
Producers
    X traffic lights
    12 per light per hour
    {timestamp, light id, cars passed by}
 
Consumers
    N most conjested
    Every hour
 */



#include "utils.h"
#include "buffer.h"
#include "workers.h"
#include <iostream>
#include <chrono>
#include <unistd.h>

#include <map>

QueueBuffer *boundbuffer = new QueueBuffer(100);;
QueueBuffer* Producers::data_buffer = boundbuffer;
QueueBuffer* Consumers::data_buffer = boundbuffer;

MapBuffer* Consumers::sorted_buffer = new MapBuffer(MAP_SIZE);


static void* print_sorted(){
    while(true){
        auto time = std::chrono::system_clock::now().time_since_epoch().count();
        if(time % (UPDATE_INT * 1000000) == 0){
            Consumers::sorted_buffer->print();
            Consumers::sorted_buffer->save();
            Consumers::sorted_buffer->clear();
            sleep(0.1);
        }
    }
}


int main(){
    srand(time(NULL));
    
    // Defining the producer and worker numbers
    uint8_t n_producers = 3;
    uint8_t n_consumers = 4;
    int n_lights = 10;
    
    // Initialising the workers
    Producers *myProducers = new Producers(n_producers, n_lights);
    Consumers *myConsumers = new Consumers(n_consumers);
    
    auto printer_thread = create_threads(1, print_sorted);
    
    // Joining the worker threads to the main thread
    join_threads(1, printer_thread);
    myProducers->start_work();
    myConsumers->start_work();
    
    printf("ending...\n");
    return 0;
}
