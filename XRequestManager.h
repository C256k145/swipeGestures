#ifndef XREQUESTMANAGER_H
#define XREQUESTMANAGER_H

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <iostream>
#include <memory>

class XRequestManager {
private:
	int currentWorkspace;
	int numWorkspaces;
	Display *disp;
	const Window root;

public:

	XRequestManager(Display *display);
	~XRequestManager();

	static std::unique_ptr<XRequestManager> Create();
	XWindowAttributes getAttrs();

	int switch_desktop(int target);

};

#endif