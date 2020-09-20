#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <thread> 
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <cmath>

#include <cstring>
#include <linux/input.h>

#define MOUSEFILE "/dev/input/mice"
#define EVENTFILE "/dev/input/event7"

// std::mutex mtx;
// std::condition_variable cv;
bool ready = false;

// std::unique_lock<std::mutex> lck(mtx);
int fds[2];
char buf[3];

void action() {
  std::cout << "Action performed!" << std::endl;
}

void parser() {
  // buf[0] is the input device number but the numbers its printing dont make sense
  int moveDistance_x = buf[1];
  int moveDistance_y = buf[2];

  // find a better way to check for the swipe action, this doesnt work very well
  if(abs(moveDistance_x) > 10)
    action();
}

/* 
 * readMouseFile thread gets the data from the mousefile 
 * and prints it out in real-time
 *
 * This can probably do any parsing necessary, and hand off 
 * only the pertinent data to the parseData thread 
 */
void readMouseFile() {
  // std::unique_lock<std::mutex> lck(mtx);
  while (!ready);
  std::ifstream myFile (MOUSEFILE, std::ios::in | std::ios::binary);

  char data[3];
  size_t data_size = sizeof(data);
  while(myFile.read(data, data_size))
    write(fds[1], data, data_size);
}


/* 
 * readEventFile thread gets the data from the eventfile 
 * 
 *
 * It reads the data from the pipe in the readmousefile 
 * I mightwant to separate the checks into a different function
 */
void readEventFile() {
  struct input_event event;
  while(!ready);

  std::ifstream infile(EVENTFILE);
  char data[sizeof(event)];

  while(infile.is_open()) {
    infile.read(data, sizeof(event));

    memcpy(&event, data, sizeof(event));
    if(event.type == EV_KEY) {
      if(event.code == BTN_TOOL_TRIPLETAP)
        read(fds[0], buf, sizeof(buf));
        action();
    }
  }
  infile.close();
}



int main(int argc, char** argv) {
	if(pipe(fds) == -1)
        fprintf(stderr, "Error creating pipe\n");
  else {
  	std::thread mouse_data_reader (readMouseFile);
  	std::thread event_data_reader (readEventFile);
    ready = true;

  	mouse_data_reader.join();
  	event_data_reader.join();
  }
	return 0;

}