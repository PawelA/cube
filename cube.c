#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>

#include "x.h"
#include "loadgl.h"
#include "gl.h"
#include "fps.h"

#define FPS 60
#define PI 3.14159265

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

void set_square_viewport(int width, int height) {
	int side = width < height ? width : height;
	glViewport((width - side) / 2, (height - side) / 2, side, side);
	
}

int main(void) {
	if (!connect_dpy())
		return 1;
	if (!make_root_ctx())
		return 1;
	loadgl();

	int width, height;
	get_screen_size(&width, &height);
	set_square_viewport(width, height);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

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

	fps_limit(FPS);

	while (1) {
		int x_pos, y_pos;
		get_cursor_pos(&x_pos, &y_pos);

		float x = PI * ((float) x_pos / (width - 1) - 0.5f);
		float y = PI * ((float) y_pos / (height - 1) - 0.5f);

		float sinx = sinf(x);
		float cosx = cosf(x);
		float siny = sinf(y);
		float cosy = cosf(y);

		GLfloat mat[4][4] = {
			{  cosx, sinx * siny,  sinx * cosy,  0.5f * sinx * cosy },
			{  0.0f, cosy       , -siny       , -0.5f * siny        },
			{ -sinx, cosx * siny,  cosx * cosy,  0.5f * cosx * cosy },
			{  0.0f, 0.0f       ,  0.0f       ,  1.5f               },
		};
		glUniformMatrix4fv(mvp_unif, 1, GL_FALSE, mat[0]);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_LINES, 0, sizeof(data) / (sizeof(float) * 3));

		swap_buffers();
		fps_frame();
	}
}
