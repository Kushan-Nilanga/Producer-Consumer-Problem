//
//  buffer.h
//  prod-con
//
//  Created by Don Kushan Athalage on 29/8/21.
//

/// PRODUCE CONSUMER PATTERN
//
// Potential Problems
//
// Manage Shared Memory
// - Stop race conditions - Using a MUTEX
//
// Checking if buffer is full
//
// If consumer try to get data from empty buffer
// Bounded buffer cant grow in size
// To stop threads from accessing synchronosly some variables should stop
// race conditions
// MUTEX is the way to go

// Extra data from producers should not be naively ignored
// SEMAPHORES is the way to go

/// CURRENT PRODUCER CONSUMER PROBLEM
// Producers => {time, id, num_cars} -> Every 5 mins for X num traffic lights
// Consumers => process data to output N most conjested traffic light
//
// *flexible number of consumers and producers

// Features
// blocking or non blockin

#ifndef buffer_h
#define buffer_h


#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <map>


// Light data structure to be stored in bounded buffer
struct light
{
    int id;
    int n_cars;
    long long time;
} typedef light;


using namespace std;

// Buffer to store data into
class QueueBuffer
{
private:
    std::queue<light> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    unsigned int buffer_size;

public:
    // Constructor
    QueueBuffer(unsigned int buffer_size) : buffer_size(buffer_size) {}

    // Adding an element
    void push(light element)
    {
        // Waiting on mutex to release the lock
        std::unique_lock<std::mutex>
            mlock(mutex_);
        while (queue_.size() >= buffer_size)
        {
            cond_.wait(mlock);
        }
        queue_.push(element);
        mlock.unlock();
        cond_.notify_one();
    }

    // Remove an element
    light pop()
    {
        // Waiting on mutex to release the lock
        std::unique_lock<std::mutex>
            mlock(mutex_);
        while (queue_.empty())
        {
            cond_.wait(mlock);
        }
        auto val = queue_.front();
        queue_.pop();
        mlock.unlock();
        cond_.notify_one();
        return val;
    }
};


class MapBuffer
{
private:
    std::map<int, light, greater<int>> map_;
    std::mutex mutex_;
    std::condition_variable cond_;
    unsigned int buffer_size;

public:
    // Constructor
    MapBuffer(unsigned int buffer_size) : buffer_size(buffer_size)
    {
        system("echo 'time,id,n_cars' > outfile.csv");
    }

    // Adding an element
    void insert(light element)
    {
        mutex_.lock();
        bool exists = false;
        for (auto it = map_.begin(); it != map_.end(); it++)
        {
            if (it->second.id == element.id)
            {
                exists = true;
                light ref = it->second;
                ref.n_cars += element.n_cars;
                map_.erase(it);
                map_.insert(std::pair<int, light>(ref.n_cars, ref));
                break;
            }
        }

        if (!exists)
        {
            map_.insert(std::pair<int, light>(element.n_cars, element));
        }
        mutex_.unlock();
    }
    
    void save(){
        mutex_.lock();
        
        int i = 1;
        string timestr = to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        for (auto it = map_.begin(); it != map_.end(); it++)
        {
            string cmd = "echo '";
            cmd += timestr;
            cmd += ", ";
            cmd += to_string(it->second.id);
            cmd += ", ";
            cmd += to_string(it->second.n_cars);
            cmd += "' >> outfile.csv";
            system(cmd.c_str());
            if(i>=buffer_size) break;
            i++;
        }
        mutex_.unlock();
    }
    
    void print(){
        mutex_.lock();
        int i = 1;
        for (auto it = map_.begin(); it != map_.end(); it++)
        {
            printf("[id: %d, n_cars: %d]", it->second.id, it->second.n_cars);
            if(i>=buffer_size) break;
            i++;
        }
        printf("\n");
        mutex_.unlock();
    }

    // clear the buffer
    void clear()
    {
        // Waiting on mutex to release the lock
        mutex_.lock();
        map_.clear();
        mutex_.unlock();
    }
};


#endif /* buffer_h */
