//
//  workers.h
//  prod-con
//
//  Created by Don Kushan Athalage on 30/8/21.
//

#ifndef workers_h
#define workers_h
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <mutex>
#include <list>
#include <map>
#include <chrono>
#include <string.h>

#include "utils.h"
#include "buffer.h"
#include "constdef.h"


// This class encloses the producer threads
// Once the class is initialised producer threads
// will run the kernal function and put the
class Producers
{
private:
	uint8_t num_workers;
    int num_lights;
	std::thread *workers;

	static void *kernal(int start, int end)
	{
        printf("start %d, end %d\n", start, end);
		while (true)
		{
			// Putting data to the bounded buffer every second
			sleep(PRODUCER_WAIT);
            for(int i = start; i < end; i++){
                // initialising data with random values
                light myLight{
                    .id = i,
                    .n_cars = rand() % 10,
                    .time = std::chrono::steady_clock::now().time_since_epoch().count()};
                string cmd = "echo '";
                cmd += to_string(myLight.time);
                cmd += ", ";
                cmd += to_string(myLight.id);
                cmd += ", ";
                cmd += to_string(myLight.n_cars);
                cmd += "' >> infile.csv";
                system(cmd.c_str());
                data_buffer->push(myLight);
            }
		}
	}

public:
	static QueueBuffer *data_buffer;
	Producers(uint8_t num_workers, int num_lights) : num_workers(num_workers), num_lights(num_lights)
	{
        system("echo 'time,id,n_cars' > infile.csv");
		workers = create_threads(num_workers, kernal, num_lights);
	};

	void start_work()
	{
		join_threads(this->num_workers, this->workers);
	};
};






// Consumers class encloses the the consumer threads and
// the respective kernal function, this kernal function will
// run on worker threads inside the consumers class and
// process the data inside the bound buffer

class Consumers
{
private:
	uint8_t num_workers;
	std::thread *workers;

	// kernal function of the consumer threads
	static void *kernal()
	{
		while (true)
		{
			light myLight = data_buffer->pop();
            sorted_buffer->insert(myLight);
		}
	}

public:
	// Dont forget to initialise these
	static QueueBuffer *data_buffer;
	static MapBuffer *sorted_buffer;

	Consumers(uint8_t num_workers) : num_workers(num_workers)
	{
		workers = create_threads(num_workers, kernal);
	};

	void start_work()
	{ join_threads(this->num_workers, this->workers);};
};

#endif /* workers_h */
