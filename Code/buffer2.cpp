#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 5;  //number of threads

int const bufferSize = 7;


class Buffer
{
private:
	//Mutex guarding the access to the critical section
	Semaphore mutex;
	
	//Number of elements to read in the buffer
	//If 0 then buffer is empty
	Semaphore empty;
	
	//Number of empty locations in the buffer
	//If 0 then buffer is full
	Semaphore full;
	
	//Semaphore to control the turn of the consumers
	Semaphore turn;
	
	//Buffer where the values are stored
	std::vector<int> values;

	void print(std::string name)
	{
		std::cout << "\n ###" << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "] ###\n";
	}

public:
	Buffer() : mutex(1),empty(0),full(7),turn(1)
	{
	}

	void put(int value)
	{
		full.p();
		
		mutex.p();
		values.push_back(value);
		print("P");
		mutex.v();
		
		empty.v();
	}

	int getA()
	{
		turn.p();
		
		empty.p();
		
		mutex.p();
		auto value = values.front();
		values.erase(values.begin());
		print("A");
		mutex.v();
		
		full.v();
		
		turn.v();
		
		return value;
	}

	int getB()
	{
		turn.v();
		
		empty.p();
		
		mutex.p();
		auto value = values.front();
		values.erase(values.begin());
		print("B");
		mutex.v();
		
		full.v();
		
		turn.p();
		
		return value;
	}
};

Buffer buffer;

void* threadProd(void* arg)
{
	for (int i = 0; i < 20; ++i)
	{
		buffer.put(i);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 30; ++i)
	{
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 30; ++i)
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
	pthread_create(&(tid[2]), NULL, threadProd, NULL);
	pthread_create(&(tid[3]), NULL, threadConsA, NULL);
	pthread_create(&(tid[4]), NULL, threadConsB, NULL);

	// waiting for all threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);

	return 0;
}

