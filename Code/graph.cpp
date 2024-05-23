#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <string>
#include <array>
#include <algorithm>

#define READ 0
#define WRITE 1

std::string pipeRead(int pipe)
{
	std::string message;
	char c;
	int readed = 0;
	while ((readed = read(pipe, &c, 1)) > 0)
		message += c;
	if (readed == -1)
	{
		perror("Error reading pipe.");
		exit(EXIT_FAILURE);
	}
	return message;
}

void pipeWrite(int pipe, std::string message)
{
	if( write(pipe, message.c_str(), message.length()) == -1)
	{
		perror("Error writing pipe.");
		exit(EXIT_FAILURE);
	}
}

std::string pipesRead(std::vector<int> const& pipesIn)
{
	std::string message = "{";
	message += std::to_string(getpid());
	for (auto pipeIn : pipesIn)
	{
		message += ";";
		message += pipeRead(pipeIn);
	}
	message += "}";
	return message;
}


void pipesWrite(std::vector<int> const& pipesOut, std::string message)
{
	for (auto pipeOut : pipesOut)
		pipeWrite(pipeOut, message);
}

std::vector<std::array<int, 2>> createPipes(int count)
{
	std::vector<std::array<int, 2>> pipes;
	for (int i = 0; i < count; ++i)
	{
		std::array<int, 2> newPipe;
		if (pipe(newPipe.data()) == -1)
		{
			perror("Error creating pipe.");
			exit(EXIT_FAILURE);
		}
		pipes.push_back(newPipe);
	}
	return pipes;
}

void childProcess(std::vector<int> const& pipesIn, std::vector<int> const& pipesOut)
{
	std::string message=pipesRead(pipesIn);
	pipesWrite(pipesOut,message);
}

void parentProcess(std::vector<int> const& pipesIn)
{
	std::string message=pipesRead(pipesIn);
	std::cout << message << "\n";
}

int main(int argc, char* argv[])
{
	auto pipes = createPipes(7);
       //0 - child0 -> par
	if (fork() == 0)
	{
		// child0
		std::vector<int> unusedPipes={pipes[0][READ],pipes[1][READ],pipes[1][WRITE],
                pipes[2][READ],pipes[2][WRITE],pipes[3][READ],pipes[3][WRITE],pipes[4][READ],
                pipes[4][WRITE],pipes[5][READ],pipes[5][WRITE],pipes[6][READ],pipes[6][WRITE]};
		std::vector<int> pipesIn={};
		std::vector<int> pipesOut={pipes[0][WRITE]};

        for(auto pipe:unusedPipes)
			close(pipe);

		//this part only in process initiating the communication
		std::string message = "{*";
		message += std::to_string(getpid());
		message += "*}";
		pipesWrite(pipesOut,message);
		//in all other children:
		//childProcess();

		for(auto pipe:pipesIn)
			close(pipe);
		for(auto pipe:pipesOut)
			close(pipe);
	}

if (fork() == 0)
	{
		// child1
		std::vector<int> unusedPipes={pipes[0][WRITE],pipes[1][READ],pipes[2][READ],
                pipes[3][READ],pipes[4][READ],pipes[4][WRITE],pipes[5][READ],pipes[5][WRITE],
                pipes[6][READ],pipes[6][WRITE]};
		std::vector<int> pipesIn={pipes[0][READ]};
		std::vector<int> pipesOut={pipes[1][WRITE],pipes[2][WRITE],pipes[3][WRITE]};

        for(auto pipe:unusedPipes)
			close(pipe);

		//this part only in process initiating the communication
		//std::string message = "{*";
		//message += std::to_string(getpid());
		//message += "*}";
		//pipesWrite(pipesOut,message);
		//in all other children:
		childProcess(pipesIn,pipesOut);

		for(auto pipe:pipesIn)
			close(pipe);
		for(auto pipe:pipesOut)
			close(pipe);
	}

if (fork() == 0)
	{
		// child2
		std::vector<int> unusedPipes={pipes[0][READ],pipes[0][WRITE],pipes[1][WRITE],pipes[2][READ],
                pipes[2][WRITE],pipes[3][READ],pipes[3][WRITE],pipes[4][READ],pipes[5][READ],pipes[5][WRITE],
                pipes[6][READ],pipes[6][WRITE]};
		std::vector<int> pipesIn={pipes[1][READ]};
		std::vector<int> pipesOut={pipes[4][WRITE]};

        for(auto pipe:unusedPipes)
			close(pipe);

		//this part only in process initiating the communication
		//std::string message = "{*";
		//message += std::to_string(getpid());
		//message += "*}";
		//pipesWrite(pipesOut,message);
		//in all other children:
		childProcess(pipesIn,pipesOut);

		for(auto pipe:pipesIn)
			close(pipe);
		for(auto pipe:pipesOut)
			close(pipe);
	}

if (fork() == 0)
	{
		// child3
		std::vector<int> unusedPipes={pipes[0][READ],pipes[0][WRITE],pipes[1][READ],pipes[1][WRITE],
                pipes[2][READ],pipes[2][WRITE],pipes[3][WRITE],pipes[4][READ],pipes[4][WRITE],pipes[5][READ],
                pipes[6][READ],pipes[6][WRITE]};
		std::vector<int> pipesIn={pipes[3][READ]};
		std::vector<int> pipesOut={pipes[5][WRITE]};

        for(auto pipe:unusedPipes)
			close(pipe);

		//this part only in process initiating the communication
		//std::string message = "{*";
		//message += std::to_string(getpid());
		//message += "*}";
		//pipesWrite(pipesOut,message);
		//in all other children:
		childProcess(pipesIn,pipesOut);

		for(auto pipe:pipesIn)
			close(pipe);
		for(auto pipe:pipesOut)
			close(pipe);
	}

if (fork() == 0)
	{
		// child4
		std::vector<int> unusedPipes={pipes[0][READ],pipes[0][WRITE],pipes[1][READ],pipes[1][WRITE],
                pipes[2][WRITE],pipes[3][READ],pipes[3][WRITE],pipes[4][WRITE],pipes[5][WRITE],pipes[6][READ],};
		std::vector<int> pipesIn={pipes[2][READ],pipes[4][READ],pipes[5][READ]};
		std::vector<int> pipesOut={pipes[6][WRITE]};

        for(auto pipe:unusedPipes)
			close(pipe);

		//this part only in process initiating the communication
		//std::string message = "{*";
		//message += std::to_string(getpid());
		//message += "*}";
		//pipesWrite(pipesOut,message);
		//in all other children:
		childProcess(pipesIn,pipesOut);

		for(auto pipe:pipesIn)
			close(pipe);
		for(auto pipe:pipesOut)
			close(pipe);
	}


	else
	{
		// parent
		std::vector<int> unusedPipes={pipes[0][READ],pipes[0][WRITE],pipes[1][READ],pipes[1][WRITE],
                pipes[2][READ],pipes[2][WRITE],pipes[3][READ],pipes[3][WRITE],pipes[4][READ],pipes[4][WRITE],
                pipes[5][READ],pipes[5][WRITE],pipes[6][WRITE]};
		std::vector<int> pipesIn={pipes[6][READ]};

		for(auto pipe:unusedPipes)
			close(pipe);

		parentProcess(pipesIn);

		for(auto pipe:pipesIn)
			close(pipe);
	}
	return 0;
}
