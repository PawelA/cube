#include <unistd.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/gl.h>
#include <GL/glx.h>

#define FPS 60

int main(int argc, char *argv[]) {

	Display *dpy = XOpenDisplay(NULL);
	Window root = DefaultRootWindow(dpy);
	XWindowAttributes attrs;
	XGetWindowAttributes(dpy, root, &attrs);
	Visual *vi = attrs.visual;

	XVisualInfo info;
	info.visual = vi;
	info.visualid = vi->visualid;
	info.bits_per_rgb = vi->bits_per_rgb;
	info.red_mask = vi->red_mask;
	info.green_mask = vi->green_mask;
	info.blue_mask = vi->blue_mask;
	info.class = vi->class;
	//info.colormap_size = ???
	info.depth = 24;
	info.screen = 0;

	XSetWindowBackground(dpy, root, 0x1a1a1a);

	GLXContext ctx = glXCreateContext(dpy, &info, NULL, GL_TRUE);
	glXMakeCurrent(dpy, root, ctx);

	glClearColor(0.1, 0.1, 0.1, 0.1);
	glColor3f(0.9, 0.9, 0.9);

	Window root_ret;
	Window child_ret;
	int root_x, root_y;
	int child_x, child_y;
	int mask;

	Screen *screen = XScreenOfDisplay(dpy, 0);
	int width  = XWidthOfScreen(screen);
	int height = XHeightOfScreen(screen);

	while (1) {
		XQueryPointer(dpy, root,
				&root_ret, &child_ret,
				&root_x, &root_y,
				&child_x, &child_y,
				&mask);

		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();
		glFrustum(-.1, .1, -.1, .1, 0.5, 10.0);
		glTranslatef(0.0, 0.0, -5.0);
		glScalef((float)height / width, 1.0, 1.0);
		glRotatef(180.0f * root_x / (width-1) , 0.0, 1.0, 0.0);
		glRotatef(180.0f * root_y / (height-1), 0.0, 0.0, 1.0);

		glBegin(GL_LINES);
		glVertex3f(-.5, -.5,  .5);
		glVertex3f(-.5,  .5,  .5);
		glVertex3f(-.5, -.5,  .5);
		glVertex3f( .5, -.5,  .5);
		glVertex3f( .5,  .5,  .5);
		glVertex3f(-.5,  .5,  .5);
		glVertex3f( .5,  .5,  .5);
		glVertex3f( .5, -.5,  .5);

		glVertex3f(-.5, -.5, -.5);
		glVertex3f(-.5,  .5, -.5);
		glVertex3f(-.5, -.5, -.5);
		glVertex3f( .5, -.5, -.5);
		glVertex3f( .5,  .5, -.5);
		glVertex3f(-.5,  .5, -.5);
		glVertex3f( .5,  .5, -.5);
		glVertex3f( .5, -.5, -.5);

		glVertex3f(-.5, -.5,  .5);
		glVertex3f(-.5, -.5, -.5);
		glVertex3f(-.5,  .5,  .5);
		glVertex3f(-.5,  .5, -.5);
		glVertex3f( .5, -.5,  .5);
		glVertex3f( .5, -.5, -.5);
		glVertex3f( .5,  .5,  .5);
		glVertex3f( .5,  .5, -.5);
		glEnd();

		glXSwapBuffers(dpy, root);

		usleep(1000000 / FPS);
	}

	XCloseDisplay(dpy);
	return 0;
}
