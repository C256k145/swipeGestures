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

//The higher this value is, the faster/longer you have to swipe
#define SENSITIVITY 30

// std::mutex mtx;
// std::condition_variable cv;
bool ready = false;

// std::unique_lock<std::mutex> lck(mtx);
int fds[2];
char buf[3];

int moveDistance_x;

void printer(char data[]) {
  printf("%d, %d, %d\n", data[0], data[1], data[2]);
}

void action(std::string direction) {
  char command[] = "xdotool\0";
  char arg1[] = "key\0";
  char argLeft[] = "ctrl+super+Right";
  char argRight[] = "ctrl+super+Left";

  if(direction == "Right") {
    char* args[] = {command, arg1, argLeft, nullptr};
    if(fork() == 0) {
      execvp(command, args);
      exit(1);
    }
  }
  else if(direction == "Left") {
    char* args[] = {command, arg1, argRight, nullptr};
    if(fork() == 0) {
      execvp(command, args);
      exit(1);
    }
  }
}

void readMouseFile() {
  std::ifstream mouseFile (MOUSEFILE, std::ios::in | std::ios::binary);

  char data[3];
  std::string right = "Right";
  std::string left = "Left";
  size_t data_size = sizeof(data);

  for(int i = 0; i < 10; i++) {
    mouseFile.read(data, data_size);
    moveDistance_x += data[1];
  }
  if(moveDistance_x > SENSITIVITY) {
    action(left);
    moveDistance_x = 0;
  }
  else if(moveDistance_x < (SENSITIVITY*-1)) {
    action(right);
    moveDistance_x = 0;
  }
}

void readEventFile() {
  struct input_event event;
  std::ifstream eventFile(EVENTFILE);

  while(eventFile.is_open()) {
    char eventData[sizeof(event)];
    eventFile.read(eventData, sizeof(event));

    memcpy(&event, eventData, sizeof(event));
    if(event.type == EV_KEY) {
      if(event.code == BTN_TOOL_TRIPLETAP) {
        readMouseFile();
        struct input_event event;
      }
    }
  }
  eventFile.close();
}



int main(int argc, char** argv) {
  readEventFile();
	return 0;
}