#include "XRequestManager.h"

std::unique_ptr<XRequestManager> XRequestManager::Create() {
  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) {
    std::cout << "Failed to open X display " << XDisplayName(nullptr);
    return nullptr;
  }

  return std::unique_ptr<XRequestManager>(new XRequestManager(display));
}

XRequestManager::XRequestManager(Display* display) : 
	disp(display),
  root(DefaultRootWindow(display)) {};

XWindowAttributes XRequestManager::getAttrs() {
	XWindowAttributes attrs;
	XGetWindowAttributes(disp, root, &attrs);
	std::cout << attrs.width << ", " << attrs.height << std::endl;
	return attrs;
}

int XRequestManager::switch_desktop(int target) {
	XEvent event;
	// define mask, populate event, window, XInternAtom
	long mask = SubstructureRedirectMask | SubstructureNotifyMask; // uh

  const char* msg = "_NET_CURRENT_DESKTOP";
  // const char* msg = "_WIN_WORKSPACE";
  event.xclient.type = ClientMessage; //uh
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(disp, msg, False);
  event.xclient.window = DefaultRootWindow(disp);	//uh
  event.xclient.format = 32;
  event.xclient.data.l[0] = target;
  event.xclient.data.l[1] = 0;
  event.xclient.data.l[2] = 0;
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;

	if(XSendEvent(disp, DefaultRootWindow(disp), False, mask, &event))
		return 0;
	else
		return 1;
}

XRequestManager::~XRequestManager() {
	XCloseDisplay(disp);
}