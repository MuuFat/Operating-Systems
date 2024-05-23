#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 4;  //number of threads

int const bufferSize = 9;


class Buffer
{
private:
    //buffer containing the data
	std::vector<int> values;
	
	//Mutex guarding the access to the critical section 
	Semaphore mutex;
	
	//Number of empty elements in the buffer
	//If 0 then buffer is full
	Semaphore full;
	
	//Number of elements to read in the buffer
	//If it 0 then the buffer is empty
	Semaphore empty;
	
	//Consumer A can read
	Semaphore semA;
	
	//Consumer B can read
	Semaphore semB;
	
	bool readA, readB;

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:
	Buffer() : mutex(1), readA(false), readB(false),full(bufferSize),empty(0),semA(1),semB(1)
	{
	}

	void put(int value)
	{
		full.p();
		mutex.p();
		// inserting the data into the buffer
		values.push_back(value);
		print("P");
		if (values.size()>3)
			empty.v();
		mutex.v();
	}

	int getA()
	{
		semA.p();
		empty.p();
		mutex.p();
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
			semA.v();
			semB.v();
			full.v();
		}
		else
			empty.v();
		mutex.v();
		return v;
	}

	int getB()
	{
		semB.p();
		empty.p();
		mutex.p();
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
			semA.v();
			semB.v();
			full.v();
		}
		else
			empty.v();
		mutex.v();
		return v;
	}
};

Buffer buffer;

void* threadProd(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		buffer.put(i);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
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

	// waiting for all threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);

	return 0;
}

