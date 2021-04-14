#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <GL/glx.h>
	
static Display *dpy;
static Window root;
static int screen;

int connect_dpy(void) {
	dpy = XOpenDisplay(NULL);
	if (!dpy) return 0;
	screen = XDefaultScreen(dpy);
	root = XRootWindow(dpy, screen);
	return 1;
}

int make_root_ctx(void) {
	XWindowAttributes attrs;
	Visual *vi;
	XVisualInfo info;
	GLXContext ctx;

	XGetWindowAttributes(dpy, root, &attrs);
	vi = attrs.visual;

	info.visual = vi;
	info.visualid = vi->visualid;
	info.screen = screen;
	info.depth = 24;
	info.class = vi->class;
	info.red_mask = vi->red_mask;
	info.green_mask = vi->green_mask;
	info.blue_mask = vi->blue_mask;
	info.colormap_size = vi->map_entries; // is this right?
	info.bits_per_rgb = vi->bits_per_rgb;

	ctx = glXCreateContext(dpy, &info, NULL, GL_TRUE);
	glXMakeCurrent(dpy, root, ctx);
	
	return 1;
}

void get_screen_size(int *width, int *height) {
	Screen *screen = XScreenOfDisplay(dpy, 0);
	*width  = XWidthOfScreen(screen);
	*height = XHeightOfScreen(screen);
}

void get_cursor_pos(int *x, int *y) {
	Window root_ret;
	Window child_ret;
	int child_x, child_y;
	int mask;
	XQueryPointer(dpy, root,
		&root_ret, &child_ret,
		x, y, &child_x, &child_y, &mask);
}

void swap_buffers(void) {
	glXSwapBuffers(dpy, root);
}
