#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <thread> 
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <error.h>

#include <cstring>
#include <linux/input.h>

#define MOUSEFILE "/dev/input/mice"
#define EVENTFILE "/dev/input/event6"

//The higher this value is, the faster/longer you have to swipe
#define SENSITIVITY 20

bool tripleTap = false;

int fds[2];
char buf[3];

void printer(char data[]) {
  printf("%d, %d, %d\n", data[0], data[1], data[2]);
}

void readMouseFile() {
  std::ifstream mouseFile (MOUSEFILE, std::ios::in | std::ios::binary);

  char mouseData[3];
  size_t mouse_data_size = sizeof(mouseData);
  int moveDistance_x = 0;

  while(true) {
    while(tripleTap) {
      mouseFile.read(mouseData, mouse_data_size);
      moveDistance_x = mouseData[1];
      std::cout << moveDistance_x << std::endl;
    }
  }
  mouseFile.close();
}

void readEventFile() {
  std::ifstream eventFile(EVENTFILE);

  struct input_event event;
  size_t eventSize = sizeof(event);
  char eventData[eventSize];

  while(eventFile.is_open()) {
    eventFile.read(eventData, eventSize);
    memcpy(&event, eventData, eventSize);
    if(event.code == BTN_TOOL_TRIPLETAP) {
      tripleTap = true;
      while(true) {
        eventFile.read(eventData, eventSize);
        memcpy(&event, eventData, eventSize);
        if(event.code == BTN_TOOL_TRIPLETAP) {
          tripleTap = false;
          break;
        }
      }
    }
  }
  eventFile.close();
}


int main(int argc, char** argv) {
  std::thread eventReader (readEventFile);
  std::thread mouseReader (readMouseFile);

  eventReader.join();
  mouseReader.join();
  return 0;
}