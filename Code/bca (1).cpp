#include <iostream>
#include <string>
#include "monitor.h"

int const threadsCounts = 3;  //number of threads

int const numberOfLettersA = 20;
int const numberOfLettersB = 20;
int const numberOfLettersC = 20;

std::string s;

Semaphore semA(1);
Semaphore semB(0);
//TODO add and initiate required semaphore/semaphores

void writeA()
{
	semA.p();
	std::cout << "BCC" << std::flush;
	s += "A";
	semB.v();

}

void writeB()
{
	semB.p();
	std::cout << "AAA" << std::flush;
	s += "B";
	semA.v();
}

void writeC()
{
	//TODO fill the function body
}

void* threadA(void* arg)
{
	for (int i = 0; i < numberOfLettersA; ++i)
	{
		writeA();
	}
	return NULL;
}

void* threadB(void* arg)
{
	for (int i = 0; i < numberOfLettersB; ++i)
	{
		writeB();
	}
	return NULL;
}

void* threadC(void* arg)
{
	//TODO add the thread body of the producer C (use the function writeC)
	return NULL;
}

int main()
{
	pthread_t tid[threadsCounts];

	// create the threads
	pthread_create(&(tid[0]), NULL, threadA, NULL);
	pthread_create(&(tid[1]), NULL, threadB, NULL);
    pthread_create(&(tid[2]), NULL, threadC, NULL);

	//wait for threads completion
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);

	std::cout << "\n";
	std::cout << "s=" << s << "\n";
	return 0;
}

