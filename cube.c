#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/glx.h>

#define FPS 60

const GLfloat data[] = {
	-0.5f, -0.5f, 0.5f,
	-0.5f,  0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	 0.5f, -0.5f, 0.5f,
	 0.5f,  0.5f, 0.5f,
	-0.5f,  0.5f, 0.5f,
	 0.5f,  0.5f, 0.5f,
	 0.5f, -0.5f, 0.5f,

	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
};

const char *vert_code = 
	"#version 330 core\n"
	"layout(location = 0) in vec3 in_coords;"
	"uniform mat4 mvp;"
	"void main() {"
		"gl_Position = mvp * vec4(in_coords, 1.0f);"
	"}";

const char *frag_code =
	"#version 330 core\n"
	"out vec3 color;"
	"void main() {"
		"color = vec3(0.9f, 0.9f, 0.9f);"
	"}";

int main(int argc, char *argv[]) {
	// get the root window
	Display *dpy = XOpenDisplay(NULL);
	if (!dpy) exit(1);
	Window root = DefaultRootWindow(dpy);

	// get the root window's visual
	XWindowAttributes attrs;
	XGetWindowAttributes(dpy, root, &attrs);
	Visual *vi = attrs.visual;

	// create visual info (manually lol)
	XVisualInfo info;
	info.visual = vi;
	info.visualid = vi->visualid;
	info.bits_per_rgb = vi->bits_per_rgb;
	info.red_mask = vi->red_mask;
	info.green_mask = vi->green_mask;
	info.blue_mask = vi->blue_mask;
	info.class = vi->class;
	info.depth = 24;
	info.screen = 0;

	// create the opengl context and activate it
	GLXContext ctx = glXCreateContext(dpy, &info, NULL, GL_TRUE);
	glXMakeCurrent(dpy, root, ctx);

	// init glew
	if (glewInit() != GLEW_OK) exit(1);

	/* now we can actually use core opengl yay */

	// set up buffers
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// create rendering program
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vert, 1, &vert_code, NULL);
	glShaderSource(frag, 1, &frag_code, NULL);

	glCompileShader(vert);
	glCompileShader(frag);

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);

	glLinkProgram(prog);

	glDetachShader(prog, vert);
	glDetachShader(prog, frag);

	glDeleteShader(vert);
	glDeleteShader(frag);

	glUseProgram(prog);

	GLuint mvp_unif = glGetUniformLocation(prog, "mvp");
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


	Screen *screen = XScreenOfDisplay(dpy, 0);
	int width  = XWidthOfScreen(screen);
	int height = XHeightOfScreen(screen);
	int side = width < height ? width : height;

	glViewport((width - side) / 2, (height - side) / 2, side, side);

	while (1) {
		Window root_ret;
		Window child_ret;
		int root_x, root_y;
		int child_x, child_y;
		int mask;
		XQueryPointer(dpy, root,
				&root_ret, &child_ret,
				&root_x, &root_y,
				&child_x, &child_y,
				&mask);

		float x = M_PI * ((float)root_x / (width - 1) - 0.5f);
		float y = M_PI * ((float)root_y / (height - 1) - 0.5f);

		float sinx = sin(x);
		float cosx = cos(x);
		float siny = sin(y);
		float cosy = cos(y);

		GLfloat mat[4][4] = {
			{  cosx, sinx * siny,  sinx * cosy,  0.5f * sinx * cosy },
			{  0.0f, cosy       , -siny       , -0.5f * siny        },
			{ -sinx, cosx * siny,  cosx * cosy,  0.5f * cosx * cosy },
			{  0.0f, 0.0f       ,  0.0f       ,  1.5f               },
		};

		glUniformMatrix4fv(mvp_unif, 1, GL_FALSE, mat[0]);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_LINES, 0, 24);

		glXSwapBuffers(dpy, root);
		usleep(1000000 / FPS);
	}
}
