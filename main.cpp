#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <thread> 
#include <fstream>
#include <mutex>
#include <condition_variable>

#define MOUSEFILE "/dev/input/mice"

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

// std::unique_lock<std::mutex> lck(mtx);
int fds[2];
char buf[3];

void readBinaryFile() {
  // std::unique_lock<std::mutex> lck(mtx);
  while (!ready);
  std::ifstream myFile (MOUSEFILE, std::ios::in | std::ios::binary);

  char data[3];
  size_t data_size = sizeof(data);
  while(myFile.read(data, data_size))
    write(fds[1], data, data_size);
}

void parseData() {
  // std::unique_lock<std::mutex> lck(mtx);
  while (!ready);

  while(true) {
      read(fds[0], buf, sizeof(buf));
      printf("%d, %d, %d\n", buf[0], buf[1], buf[2]);
  }
}

void go() {
  std::cout << "dick" << std::endl;
  std::unique_lock<std::mutex> lck(mtx);
  std::cout << "dick" << std::endl;
  ready = true;
  cv.notify_all();
}


int main(int argc, char** argv) {
	if(pipe(fds) == -1)
        fprintf(stderr, "Error creating pipe\n");
  else {
  	std::thread mouse_data_reader (readBinaryFile);
  	std::thread parsing_thread (parseData);
    ready = true;

  	mouse_data_reader.join();
  	parsing_thread.join();
  }
	return 0;

}
