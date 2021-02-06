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
	long mask = SubstructureRedirectMask | SubstructureNotifyMask;
  const char* msg = "_NET_CURRENT_DESKTOP";
  event.xclient.type = ClientMessage; 
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(disp, msg, False);
  event.xclient.window = DefaultRootWindow(disp);
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

/*
Options for prop_name are _NET_NUMBER_OF_DESKTOPS and _NET_CURRENT_DESKTOP. 
Probably more too, but thats what im gonna use.
*/
int XRequestManager::get_property(const char* prop_name) {
	Window win = DefaultRootWindow(disp);
	// const char* prop_name = "_NET_NUMBER_OF_DESKTOPS";
	Atom xa_prop_name = XInternAtom(disp, prop_name, False);
	Atom xa_ret_type;
	int ret_format;
	unsigned long ret_nitems;
	unsigned long ret_bytes_after;
	unsigned char *ret_prop;

	XGetWindowProperty(disp, win, xa_prop_name, 0, 1024, False, XA_CARDINAL, 
		&xa_ret_type, &ret_format, &ret_nitems, &ret_bytes_after, &ret_prop);

  if (xa_ret_type != XA_CARDINAL) {
  	printf("Invalid type of %s property \n", prop_name);
  	XFree(ret_prop);
  	return 1;
  }

	unsigned long tmp_size = (ret_format / (32 / sizeof(long))) * ret_nitems;
  char* res = (char*)malloc(tmp_size + 1);
  memcpy(res, ret_prop, tmp_size);
  res[tmp_size] = '\0';

	return (unsigned int)*res;
}

XRequestManager::~XRequestManager() {
	XCloseDisplay(disp);
}