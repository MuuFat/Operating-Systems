#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[])
{
  int p1[2], p2[2];
  pipe(p1);
  pipe(p2);

  if (fork() == 0)
  {
    close(p1[0]);
    close(p2[1]);
    int number;
    do
      {
      std::cout << "Child1 process: enter number:";
      std::cin >> number;
      if (number > 0)
        {
        std::cout << "Child1 process: send: " << number << "\n";
        write(p1[1], & number, sizeof(number));
        }
      } while (number > 0);
    close(p1[1]);
    close(p2[0]);
    exit(0);
  }

  if (fork() == 0)
  {
    close(p1[1]);
    close(p2[0]);
    int number;
    do
      {
      read(p1[0], & number, sizeof(number));
      if (number > 67)
        {
        std::cout << "Child2 process: send: " << number << "\n";
        write(p2[1], & number, sizeof(number));
        }
      } while (number > 0);
    close(p1[0]);
    close(p2[1]);
    exit(0);
  }
  close(p1[0]);
  close(p2[1]);
  int number;
  do
    {
    read(p2[0], & number, sizeof(number));
    if (number > 0)
      {
      std::cout << "Parent process: received: " << number << "\n";
      }
    } while (number > 0);
  close(p1[1]);
}
