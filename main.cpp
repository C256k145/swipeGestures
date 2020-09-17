#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <thread> 
#include <fstream>
#include <condition_variable>
#include <mutex>

#define MOUSEFILE "/dev/input/mice"

bool can_go = true;  // change this to a condition var eventually
std::condition_variable go;
std::mutex mtx;
int fds[2];
char buf[3];

void readBinaryFile() {
  std::unique_lock<std::mutex> lk(mtx);
  go.wait(lk);
  std::ifstream myFile (MOUSEFILE, std::ios::in | std::ios::binary);

  char data[3];
  size_t data_size = sizeof(data);
  while(myFile.read(data, data_size)) {
    // printf("%d, %d, %d\n", data[0], data[2], data[2]);
    write(fds[1], data, data_size);
  }
}

void parseData() {
  std::unique_lock<std::mutex> lk(mtx);
  go.wait(lk);
  while(can_go == true) {
    read(fds[0], buf, sizeof(buf));
    printf("%d, %d, %d\n", buf[0], buf[1], buf[2]);
    write(1, buf, sizeof(buf));
  }
}


int main(int argc, char** argv) {

  printf("testing");
  	/* open a pipe. fd[0] is opened for reading, and fd[1] for writing.*/
	if(pipe(fds) == -1)
        fprintf(stderr, "Error creating pipe\n");
    else {
    	// thread *threads = spawn_threads(num_threads);
    	std::thread mouse_data_reader (readBinaryFile);
    	std::thread parsing_thread (parseData);
      go.notify_all();

	  	/* This can be read from the other end of the pipe */

	  	// printf("fds[0]=%d, fds[1]=%d\n", fds[0], fds[1]);

	  	mouse_data_reader.join();
	  	parsing_thread.join();
    }

    // write(1) => stdout
    // write(0) => stdin
  	return 0;

}
