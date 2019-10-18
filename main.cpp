#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gl_core_3_3.h"
#include <GL/freeglut.h>
#include "util.hpp"
#include "mesh.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
using namespace std;
using namespace glm;
float rot2 = 0;
// Global state
GLint width, height;
unsigned int viewmode;	// View triangle or obj file
GLuint shader,shader_island;			// Shader program
GLuint uniXform;		// Shader location of xform mtx
GLuint vao;				// Vertex array object
GLuint vbuf;			// Vertex buffer
GLfloat amp1 = 0.08;
GLfloat amp2 = 0.05;
GLfloat amp3 = 0.03;

GLsizei vcount;			// Number of vertices
Mesh* mesh;				// Mesh loaded from .obj file
// Camera state
vec3 camCoords;			// Spherical coordinates (theta, phi, radius) of the camera
bool camRot;			// Whether the camera is currently rotating
vec2 camOrigin;			// Original camera coordinates upon clicking
vec2 mouseOrigin;		// Original mouse coordinates upon clicking
struct vert {
	vec3 pos;	// Vertex position
	vec3 norm;	// Vertex normal
	vec2 uv;
};
int a1 = 0;
vector<vert> verts;
vector<vert> init_verts;
void cleanup();
int num_pts = 32;
// Constants
const int MENU_VIEWMODE = 0;		// Toggle view mode
const int MENU_EXIT = 1;			// Exit application
const int VIEWMODE_TRIANGLE = 0;	// View triangle
const int VIEWMODE_OBJ = 1;			// View obj-loaded mesh

// Initialization functions
void initState();
void initGLUT(int* argc, char** argv);
void initOpenGL();
void initTriangle();

// Callback functions
void display();
void reshape(GLint width, GLint height);
void keyRelease(unsigned char key, int x, int y);
void mouseBtn(int button, int state, int x, int y);
void mouseMove(int x, int y);
void idle();
void menu(int cmd);
void cleanup();
vec3 lengths=vec3(0.2,0.4,0.6);
int main(int argc, char** argv) {
	

	try {
		// Initialize
		initState();
		initGLUT(&argc, argv);
		initOpenGL();
		initTriangle();

	} catch (const exception& e) {
		// Handle any errors
		cerr << "Fatal error: " << e.what() << endl;
		cleanup();
		return -1;
	}

	// Execute main loop
	glutMainLoop();

	return 0;
}

void initState() {
	// Initialize global state
	width = 0;
	height = 0;
	viewmode = VIEWMODE_TRIANGLE;
	shader = 0;
	uniXform = 0;
	vao = 0;
	vbuf = 0;
	vcount = 0;
	mesh = NULL;

	camCoords = vec3(5.0, 50.0, 5.0);
	camRot = false;
	
}

void initGLUT(int* argc, char** argv) {
	// Set window and context settings
	width = 800; height = 600;
	glutInit(argc, argv);
	glutInitWindowSize(width, height);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	// Create the window
	glutCreateWindow("FreeGlut Window");

	// Create a menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Toggle view mode", MENU_VIEWMODE);
	//glutAddMenuEntry("Increase Amplitude for wave1", amp1);
	glutAddMenuEntry("Exit", MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// GLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyRelease);
	glutMouseFunc(mouseBtn);
	glutMotionFunc(mouseMove);
	glutIdleFunc(idle);
	glutCloseFunc(cleanup);

}

void initOpenGL() {
	// Set clear color and depth
	glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
	glClearDepth(1.0f);
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Compile and link shader program
	vector<GLuint> shaders,shaders_island;
	shaders.push_back(compileShader(GL_VERTEX_SHADER, "sh_v.glsl"));
	shaders.push_back(compileShader(GL_FRAGMENT_SHADER, "sh_f.glsl"));
	shaders_island.push_back(compileShader(GL_VERTEX_SHADER, "sh_vi.glsl"));
	shaders_island.push_back(compileShader(GL_FRAGMENT_SHADER, "sh_fi.glsl"));
	shader_island = linkProgram(shaders_island);
	shader = linkProgram(shaders);
	// Release shader sources
	for (auto s = shaders.begin(); s != shaders.end(); ++s)
		glDeleteShader(*s);
	shaders.clear();
	// Locate uniforms
	uniXform = glGetUniformLocation(shader, "xform");
	assert(glGetError() == GL_NO_ERROR);
	const char* filename = "water.jpg";
	glActiveTexture(GL_TEXTURE0);
	int image_height = 0, image_width = 0, num_channels = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, &num_channels, 0);

	// Do some simple checking.
	if (image_data == nullptr) {
		std::cerr << "Image reading failed." << std::endl;

	}
	else if (num_channels != 3 && num_channels != 4) {
		std::cerr << "The loaded image doesn't have RGB color components." << std::endl;
		std::cerr << "The loaded image has " << num_channels << " channels" << std::endl;

	}
	else {
		std::cout << "The image loaded has size " << image_width << "x" << image_height << std::endl;
	}
	GLuint tex_1;

	glGenTextures(1, &tex_1);
	glBindTexture(GL_TEXTURE_2D, tex_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,image_width,image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	glActiveTexture(GL_TEXTURE1);




	 filename = "models/rock.png";
	glActiveTexture(GL_TEXTURE1);
	//int image_height = 0, image_width = 0, num_channels = 0;
	 image_data = stbi_load(filename, &image_width, &image_height, &num_channels, 0);

	// Do some simple checking.
	if (image_data == nullptr) {
		std::cerr << "Image reading failed." << std::endl;

	}
	else if (num_channels != 3 && num_channels != 4) {
		std::cerr << "The loaded image doesn't have RGB color components." << std::endl;
		std::cerr << "The loaded image has " << num_channels << " channels" << std::endl;

	}
	else {
		std::cout << "The image loaded has size " << image_width << "x" << image_height << std::endl;
	}
	GLuint tex_2;

	glGenTextures(1, &tex_2);
	glBindTexture(GL_TEXTURE_2D, tex_2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	glActiveTexture(GL_TEXTURE2);

}

void initTriangle() {
	// Create a colored triangle


	//vector<vert> verts;
	for (int i = -16; i < num_pts / 2; i++) {
		for (int j = -16; j < num_pts / 2; j++) {
			verts.push_back({ vec3(i,0, j ) , vec3(0.0,1.0,0.0) , vec2((i + 16.0) / 32.0, (j + 16.0) / 32.0) });
			verts.push_back({ vec3(i,0, j + 1 ) , vec3(0.0,1.0,0.0), vec2((i + 16.0) / 32.0, (j + 16.0 + 1) / 32.0) });
			verts.push_back({ vec3(i + 1,0, j  ) , vec3(0.0,1.0,0.0) , vec2((i + 16.0 + 1.0) / 32, (j + 16.0) / 32.0) });


			verts.push_back({ vec3(i + 1,0, j + 1) , vec3(0.0,1.0,0.0), vec2((i + 16.0 + 1) / 32.0, (j + 16.0 + 1) / 32.0) });
			verts.push_back({ vec3(i,0, j + 1) , vec3(0.0,1.0,0.0) , vec2((i + 16.0) / 32.0, (j + 16.0 + 1) / 32.0) });
			verts.push_back({ vec3(i + 1,0, j) , vec3(0.0,1.0,0.0) , vec2((i + 16.0 + 1.0) / 32.0, (j + 16.0) / 32.0) });

		}
	}

	vcount = verts.size();

	// Create vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create vertex buffer
	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(vert), verts.data(), GL_STATIC_DRAW);
	// Specify vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vert), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)sizeof(vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vert), (GLvoid*)(2 * sizeof(vec3)));
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	assert(glGetError() == GL_NO_ERROR);


}
void display() {
	try {
		// Clear the back buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader_island);
		mat4 xform;
		
		float aspect = (float)width / (float)height;
		// Create perspective projection matrix
		mat4 proj = perspective(45.0f, aspect, 0.1f, 100.0f);
		// Create view transformation matrix
		mat4 view = translate(mat4(1.0f), vec3(0.0, 0.0, -camCoords.z));
		//mat4 rot2 = rotate(mat4(1.0f), radians(float(0.0)), vec3(1.0, 0.0, 0.0));
		mat4 rot = rotate(mat4(1.0f), radians(camCoords.y), vec3(1.0, 0.0, 0.0));
		rot = rotate(rot, radians(camCoords.x), vec3(0.0, 1.0, 0.0));
		xform = proj * view * rot;
		// Get ready to draw
		
		
		
		glUseProgram(shader);


int		 tex_loc = glGetUniformLocation(shader, "diffuse_tex");
		//glActiveTexture(GL_TEXTURE0);
		int tex_toggle = 0;
		if (tex_loc != -1)
		{
			glUniform1i(tex_loc, tex_toggle); // we bound our texture to texture unit 0
		}
		switch (viewmode) {
		case 1:
			glBindVertexArray(vao);
			// Send transformation matrix to shader
			glUniformMatrix4fv(uniXform, 1, GL_FALSE, value_ptr(xform));
			glUniform1f(3, 0.001 * glutGet(GLUT_ELAPSED_TIME));
			//cout << 1 * glutGet(GLUT_ELAPSED_TIME) << endl;
			// Draw the triangle
			glUniform1f(4, amp1);
			glUniform1f(5, amp2);
			glUniform1f(6, amp3);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, vcount*vcount);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(0);
			break;

		case 0: {
			// Load model on demand
			
			glBindVertexArray(vao);
			glUniformMatrix4fv(uniXform, 1, GL_FALSE, value_ptr(xform));
			glUniform1f(3, 0.001 * glutGet(GLUT_ELAPSED_TIME));
			//cout << 1 * glutGet(GLUT_ELAPSED_TIME) << endl;
			// Draw the triangle

		
			//cout <<rot_loc  << endl;
			glUniform1f(4,amp1);
			glUniform1f(5, amp2);
			glUniform1f(6, amp3);
		//	amps_loc = glGetUniformLocation(shader, "wavelengths");
			//cout <<rot_loc  << endl;
		//	glUniform3f(amps_loc, amps.x, amps.y, amps.z);
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, vcount);
			
			glBindVertexArray(0);
			break; }
		}
	//	assert(glGetError() == GL_NO_ERROR);

		// Revert context state
		glUseProgram(0);

		// Display the back buffer
		glutSwapBuffers();

	} catch (const exception& e) {
		cerr << "Fatal error: " << e.what() << endl;
		glutLeaveMainLoop();
	}
}

void reshape(GLint width, GLint height) {
	::width = width;
	::height = height;
	glViewport(0, 0, width, height);
}

void keyRelease(unsigned char key, int x, int y) {
	switch (key) {
	case 27:	// Escape key
		menu(MENU_EXIT);
		break;

	case 49:
		amp1 = amp1 + 0.1;
		break;
	case 50:
		amp1 = amp1 - 0.1;
		break;

	case 51:
		amp2 = amp2 + 0.1;
		break;
	case 52:
		amp2 = amp2 - 0.1;
		break;

	case 53:
		amp3 = amp3 + 0.1;
		break;
	case 54:
		amp3 = amp3 - 0.1;
		break;
	}
	

	//	cout << key << endl;
	cout << "Current amps : \n" << "Amp1 = " << amp1 << "\n" << "Amp2 = " << amp2<< "\n" << "Amp3 = " << amp3 << "\n" << endl;
}

void mouseBtn(int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		// Activate rotation mode
		camRot = true;
		camOrigin = vec2(camCoords);
		mouseOrigin = vec2(x, y);
	}
	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
		// Deactivate rotation
		camRot = false;
	}
	if (button == 3) {
		camCoords.z = clamp(camCoords.z - 0.1f, 0.1f, 10.0f);
		glutPostRedisplay();
	}
	if (button == 4) {
		camCoords.z = clamp(camCoords.z + 0.1f, 0.1f, 10.0f);
		glutPostRedisplay();
	}
}

void mouseMove(int x, int y) {
	if (camRot) {
		// Convert mouse delta into degrees, add to rotation
		float rotScale = min(width / 450.0f, height / 270.0f);
		vec2 mouseDelta = vec2(x, y) - mouseOrigin;
		vec2 newAngle = camOrigin + mouseDelta / rotScale;
		newAngle.y = clamp(newAngle.y, -90.0f, 90.0f);
		while (newAngle.x > 180.0f) newAngle.x -= 360.0f;
		while (newAngle.y < -180.0f) newAngle.y += 360.0f;
		if (length(newAngle - vec2(camCoords)) > FLT_EPSILON) {
			camCoords.x = newAngle.x;
			camCoords.y = newAngle.y;
			glutPostRedisplay();
		}
	}
}

void idle() {
	glutPostRedisplay();
	


}

void menu(int cmd) {
	switch (cmd) {
	case MENU_VIEWMODE:
		viewmode = (viewmode + 1) % 2;
		glutPostRedisplay();	// Tell GLUT to redraw the screen
		break;

	case MENU_EXIT:
		glutLeaveMainLoop();
		break;
	}
}

void cleanup() {
	// Release all resources
	if (shader) { glDeleteProgram(shader); shader = 0; }
	uniXform = 0;
	if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
	if (vbuf) { glDeleteBuffers(1, &vbuf); vbuf = 0; }
	vcount = 0;
	if (mesh) { delete mesh; mesh = NULL; }
}