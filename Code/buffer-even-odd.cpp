#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 5;  //number of threads

int const bufferSize = 7;  //changed to 7 

class Buffer
{
private:
	std::vector<int> values;

	Semaphore mutex;
	Semaphore full;
	Semaphore semA;
	Semaphore semB;
	Semaphore semC;
	
	bool waitA;  //true -> consumer A is waiting for the critical section to be  
				 //passed by semA semaphore
	bool waitB;  //true -> consumer B is waiting for the critical section to be 
	             //passed by semB semaphore
	bool waitC;  //true -> consumer C is waiting for the critical section to be 
	             //passed by semC semaphore

	void print(std::string name)
	{
		std::cout << "\n### " << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "]###\n";
	}

public:

	Buffer() : mutex(1), full(bufferSize), semA(0), semB(0), semC(0), waitA(false), waitB(false), waitC(false)
	{
	}


	bool canA()
	{
		//consumer A can consume only odd numbers
		return !values.empty() && values.front() % 2 == 1;
	}
	
	bool canB()
	{
		//consumer B can consume only even numbers
		return !values.empty() && values.front() % 2 == 0;
	}
	
	bool canC()
	{
		//consumer C can consume any number
		return !values.empty();
	}

	void putX(int value)
	{
		full.p();
		mutex.p();
		//new element inserted at the end of the buffer
		values.push_back(value);
		print("X");
		//resuming of other clients
		if (waitA && canA()) //if consumer A is waiting and can consume
		{
			waitA = false; //stop waiting
			semA.v(); //resume consumer A
		}
		else if (waitC && canC()) //if consumer C is waiting and can consume
		{
			waitC = false; //stop waiting
			semC.v(); //resume consumer C
		}
		mutex.v();
	}

	void putY(int value)
	{
		full.p();
		mutex.p();
		//new element inserted at the end of the buffer
		values.push_back(value);
		print("Y");
		//resuming of other clients
		if (waitB && canB()) //if consumer B is waiting and can consume
		{
			waitB = false; //stop waiting
			semB.v(); //resume consumer B
		}
		else if (waitC && canC()) //if consumer C is waiting and can consume
		{
			waitC = false; //stop waiting
			semC.v(); //resume consumer C
		}
		mutex.v();
	}
	
	int getA()
	{
		mutex.p();
		//check if the consumer can consume
		while (!canA()) //while consumer A cannot consume
		{
			waitA = true; //start waiting
			mutex.v(); //release the mutex
			semA.p(); //wait for the semaphore
			mutex.p(); //acquire the mutex
		}
		// reading the data from the buffer (without deleting it)
		int v = values.front();
		// removing item from the buffer
		values.erase(values.begin());
		print("A read & remove");
		full.v();
		//resuming of other clients
		if (waitB && canB()) //if consumer B is waiting and can consume
		{
			waitB = false; //stop waiting
			semB.v(); //resume consumer B
		}
		else if (waitC && canC()) //if consumer C is waiting and can consume
		{
			waitC = false; //stop waiting
			semC.v(); //resume consumer C
		}
	    mutex.v();
		return v;
	}
	
	int getB()
	{
		mutex.p();
		//check if the consumer can consume
		while (!canB()) //while consumer B cannot consume
		{
			waitB = true; //start waiting
			mutex.v(); //release the mutex
			semB.p(); //wait for the semaphore
			mutex.p(); //acquire the mutex
		}
		// reading the data from the buffer (without deleting it)
		int v = values.front();
		values.erase(values.begin());
		print("B read & remove");
		full.v();
		//resuming of other clients
		if (waitA && canA()) //if consumer A is waiting and can consume
		{
			waitA = false; //stop waiting
			semA.v(); //resume consumer A
		}
		else if (waitC && canC()) //if consumer C is waiting and can consume
		{
			waitC = false; //stop waiting
			semC.v(); //resume consumer C
		}
		mutex.v();
		return v;
	}
	
	int getC()
	{
		mutex.p();
		//check if the consumer can consume
		while (!canC()) //while consumer C cannot consume
		{
			waitC = true; //start waiting
			mutex.v(); //release the mutex
			semC.p(); //wait for the semaphore
			mutex.p(); //acquire the mutex
		}
		// reading the data from the buffer (without deleting it)
		int v = values.front();
		values.erase(values.begin());
		print("C read & remove");
		full.v();
		//resuming of other clients
		if (waitA && canA()) //if consumer A is waiting and can consume
		{
			waitA = false; //stop waiting
			semA.v(); //resume consumer A
		}
		else if (waitB && canB()) //if consumer B is waiting and can consume
		{
			waitB = false; //stop waiting
			semB.v(); //resume consumer B
		}
		mutex.v();
		return v;
	}
};

Buffer buffer;

void* threadProdX(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		sleep(1);
		buffer.putX(i*2+1);
	}
	return NULL;
}

void* threadProdY(void* arg)
{
	for (int i = 0; i < 10; ++i)
	{
		sleep(1);
		buffer.putY(i*2);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		sleep(2);
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		sleep(2);
		auto value = buffer.getB();
	}
	return NULL;
}

void* threadConsC(void* arg)
{
	for (int i = 0; i < 17; ++i)
	{
		sleep(3);
		auto value = buffer.getC();
	}
	return NULL;
}

int main()
{
	pthread_t tid[threadsCounts];

	// creation of threads
	pthread_create(&(tid[0]), NULL, threadProdX, NULL);
	pthread_create(&(tid[1]), NULL, threadProdY, NULL);
	pthread_create(&(tid[2]), NULL, threadConsA, NULL);
	pthread_create(&(tid[3]), NULL, threadConsB, NULL);
	pthread_create(&(tid[4]), NULL, threadConsC, NULL);

	//waiting for all threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);

	return 0;
}
