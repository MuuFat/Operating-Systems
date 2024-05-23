#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include "monitor.h"

int const threadsCounts = 4; // Updated number of threads

int const bufferSize = 9;

class Buffer
{
private:
    std::vector<int> values;

    Semaphore mutex;
    Semaphore full;
    Semaphore semA;
    Semaphore semB;
    bool readA;
    bool readB;
    bool waitA;
    bool waitB;

    void print(std::string name)
    {
        std::cout << "\n### " << name << " " << values.size() << "[";
        for (auto v : values)
            std::cout << v << ",";
        std::cout << "]###\n";
    }

public:
    Buffer() : mutex(1), readA(false), readB(false), full(bufferSize), semA(0), semB(0), waitA(false), waitB(false)
    {
    }

    bool canA()
    {
        int sum = 0;
        for (auto v : values)
            sum += v;
        return values.size() > 3 && sum < 20 && !readA;
    }

    bool canB()
    {
        return values.size() > 3 && !readB;
    }

    void put(int value)
    {
        full.p();
        mutex.p();
        //new element inserted at the end of the buffer
        values.push_back(value);
        print("P");
        //TODO resuming of other clients
        if (values.size() == 4)
        {
            if (canA() && waitA)
                semA.v();
            else if (canB() && waitB)
                semB.v();
            else
                mutex.v();
        }
        else
            mutex.v();
    }

    int getA()
    {
        mutex.p();
        //TODO check if the consumer can consume
        if (!canA())
        {
            waitA = true;
            print("waitA");
            mutex.v();
            semA.p();
            print("resume A");
            waitA = false;
        }
        // reading the data from the buffer (without deleting it)
        int v = values.front();
        readA = true;
        print("A read");
        if (readB)
        {
            // removing item from the buffer
            values.erase(values.begin());
            print("A remove");
            readA = readB = false;
            full.v();
            //TODO resuming of other clients
            if (canB() && waitB)
                semB.v();
            else
                mutex.v();
        }
        else
        {
            //TODO resuming of other clients
            if (canB() && waitB)
                semB.v();
            else
                mutex.v();
        }
        return v;
    }

    int getB()
    {
        mutex.p();
        //TODO check if the consumer can consume
        if (!canB())
        {
            waitB = true;
            print("waitB");
            mutex.v();
            semB.p();
            print("resume B");
            waitB = false;
        }
        // reading the data from the buffer (without deleting it)
        int v = values.front();
        readB = true;
        print("B read");
        if (readA)
        {
            // removing item from the buffer
            values.erase(values.begin());
            print("B remove");
            readA = readB = false;
            full.v();
            //TODO resuming of other clients
            if (canA() && waitA)
                semA.v();
            else
                mutex.v();
        }
        else
        {
            //TODO resuming of other clients
            if (canA() && waitA)
                semA.v();
            else
                mutex.v();
        }
        return v;
    }
};

Buffer buffer;

void *threadProd(void *arg)
{
    for (int i = 0; i < 10; ++i)
    {
        //sleep(1);
        buffer.put(i);
    }
    return NULL;
}

void *threadConsA(void *arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getA();
    }
    return NULL;
}

void *threadConsB(void *arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getB();
    }
    return NULL;
}

int main()
{
    pthread_t tid[threadsCounts];

    // creation of threads
    pthread_create(&(tid[0]), NULL, threadProd, NULL);
    pthread_create(&(tid[1]), NULL, threadProd, NULL);
    pthread_create(&(tid[2]), NULL, threadConsA, NULL);
    pthread_create(&(tid[3]), NULL, threadConsB, NULL);

    //waiting for all threads to finish
    for (int i = 0; i < threadsCounts; i++)
        pthread_join(tid[i], (void **)NULL);

    return 0;
}
