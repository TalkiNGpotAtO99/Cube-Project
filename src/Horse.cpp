//
// Display a color cube
//
// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "cube.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

glm::mat4 projectMat;
glm::mat4 viewMat;

GLuint pvmMatrixID;

float rotAngle = 0.0f;
float LUpperLegAngle = 0.0f;
float RUpperLegAngle = 0.0f;
float LLowerLegAngle = 0.0f;
float RLowerLegAngle = 0.0f;
glm::vec3 LLowerLegTrans = glm::vec3(0.0, -0.14, 0.0);
glm::vec3 RLowerLegTrans = glm::vec3(0.0, -0.14, 0.0);

bool isChange = false; // Leg direction change toggle
bool isTrans = false; // Lower Leg translation change toggle

typedef glm::vec4  color4;
typedef glm::vec4  point4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
	point4(-0.5, -0.5, 0.5, 1.0),
	point4(-0.5, 0.5, 0.5, 1.0),
	point4(0.5, 0.5, 0.5, 1.0),
	point4(0.5, -0.5, 0.5, 1.0),
	point4(-0.5, -0.5, -0.5, 1.0),
	point4(-0.5, 0.5, -0.5, 1.0),
	point4(0.5, 0.5, -0.5, 1.0),
	point4(0.5, -0.5, -0.5, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
	color4(0.0, 0.0, 0.0, 1.0),  // black
	color4(0.0, 1.0, 1.0, 1.0),   // cyan
	color4(1.0, 0.0, 1.0, 1.0),  // magenta
	color4(1.0, 1.0, 0.0, 1.0),  // yellow
	color4(1.0, 0.0, 0.0, 1.0),  // red
	color4(0.0, 1.0, 0.0, 1.0),  // green
	color4(0.0, 0.0, 1.0, 1.0),  // blue
	color4(1.0, 1.0, 1.0, 1.0)  // white
};

//----------------------------------------------------------------------------

// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void
quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a];  Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c];  Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d];  Index++;
}

//----------------------------------------------------------------------------

// generate 12 triangles: 36 vertices and 36 colors
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

// OpenGL initialization
void
init()
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("src/vshader.glsl", "src/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(sizeof(points)));

	pvmMatrixID = glGetUniformLocation(program, "mPVM");

	projectMat = glm::perspective(glm::radians(65.0f), 1.0f, 0.1f, 100.0f);
	viewMat = glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------

void drawHorse(glm::mat4 HorseMat)
{
	glm::mat4 modelMat, pvmMat;
	glm::vec3 EarPos[2];
	glm::vec3 LUpperLegPos[2];
	glm::vec3 RUpperLegPos[2];
	glm::vec3 LLowerLegPos[2];
	glm::vec3 RLowerLegPos[2];

	EarPos[0] = glm::vec3(-0.8, 0.25, -0.1); // right
	EarPos[1] = glm::vec3(-0.8, 0.25, 0.1); // left

	LUpperLegPos[0] = glm::vec3(-0.35, -0.2, 0.22); //front
	LUpperLegPos[1] = glm::vec3(0.2, -0.2, 0.22); //back
	RUpperLegPos[0] = glm::vec3(-0.35, -0.2, -0.22); //front
	RUpperLegPos[1] = glm::vec3(0.2, -0.2, -0.22); //back
	LLowerLegPos[0] = LUpperLegPos[0] + LLowerLegTrans; //front
	LLowerLegPos[1] = LUpperLegPos[1] + LLowerLegTrans; //back
	RLowerLegPos[0] = RUpperLegPos[0] + RLowerLegTrans; //front
	RLowerLegPos[1] = RUpperLegPos[1] + RLowerLegTrans; //back

	// Horse middle body
	modelMat = glm::scale(HorseMat, glm::vec3(0.3, 0.3, 0.3));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Horse head direction body
	modelMat = glm::translate(HorseMat, glm::vec3(-0.33, -0.04, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.3, 0.3, 0.3));
	modelMat = glm::rotate(modelMat, 3.35f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Horse tail direction body
	modelMat = glm::translate(HorseMat, glm::vec3(0.28, -0.04, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.3, 0.3));
	modelMat = glm::rotate(modelMat, -3.35f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Horse neck
	modelMat = glm::translate(HorseMat, glm::vec3(-0.6, -0.07, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.2, 0.3, 0.3));
	modelMat = glm::rotate(modelMat, 3.6f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Horse head
	modelMat = glm::translate(HorseMat, glm::vec3(-0.8, 0.1, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.4, 0.2, 0.3));
	modelMat = glm::rotate(modelMat, 3.6f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Horse ear
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(HorseMat, EarPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.1, 0.1));
		modelMat = glm::rotate(modelMat, 3.6f, glm::vec3(0, 0, 1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// Horse tail
	modelMat = glm::translate(HorseMat, glm::vec3(0.45, -0.4, 0));
	modelMat = glm::scale(modelMat, glm::vec3(0.1, 0.65, 0.1));
	modelMat = glm::rotate(modelMat, -0.1f, glm::vec3(0, 0, 1));
	pvmMat = projectMat * viewMat * modelMat;
	glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

	// Horse left Upper legs
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(HorseMat, LUpperLegPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.15, 0.25, 0.15));
		modelMat = glm::rotate(modelMat, LUpperLegAngle , glm::vec3(0, 0, 1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// Horse Right Upper legs
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(HorseMat, RUpperLegPos[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.15, 0.25, 0.15));
		modelMat = glm::rotate(modelMat, RUpperLegAngle, glm::vec3(0, 0, 1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// Horse left Lower legs
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(HorseMat, LLowerLegPos[i] + LLowerLegTrans);
		modelMat = glm::scale(modelMat, glm::vec3(0.15, 0.25, 0.15));
		modelMat = glm::rotate(modelMat, LLowerLegAngle, glm::vec3(0, 0, 1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

	// Horse right Lower legs
	for (int i = 0; i < 2; i++)
	{
		modelMat = glm::translate(HorseMat, RLowerLegPos[i] + RLowerLegTrans);
		modelMat = glm::scale(modelMat, glm::vec3(0.15, 0.25, 0.15));
		modelMat = glm::rotate(modelMat, RLowerLegAngle, glm::vec3(0, 0, 1));
		pvmMat = projectMat * viewMat * modelMat;
		glUniformMatrix4fv(pvmMatrixID, 1, GL_FALSE, &pvmMat[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, NumVertices);
	}

}

void display(void)
{
	glm::mat4 worldMat, pvmMat;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldMat = glm::rotate(glm::mat4(1.0f), rotAngle , glm::vec3(0.0f, 1.0f, 0.0f));

	drawHorse(worldMat);

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void idle(int value)
{
	rotAngle += glm::radians(1.0f); // Full-body rotation
	//Leg rotation
	if (isChange == false) {		// Is leg direction change? (Start with Right direction)
		LUpperLegAngle += glm::radians(2.0f);
		RUpperLegAngle -= glm::radians(2.0f);

		isTrans = true;				// Is Lower Leg translate?
		LLowerLegTrans += glm::vec3(glm::radians(2.0f) * 0.1, 0.0, 0.0);	//Lower Leg translation
		RLowerLegTrans -= glm::vec3(glm::radians(2.0f) * 0.1, 0.0, 0.0);	//Lower Leg translation
		LLowerLegAngle += glm::radians(3.5f);
		RLowerLegAngle -= glm::radians(3.5f);

		if (LUpperLegAngle > glm::radians(30.0f)) {		// limit for direction change
			LUpperLegAngle = glm::radians(30.0f);
			RUpperLegAngle = -glm::radians(30.0f);
			isChange = true;
			isTrans = false;
		}
	}
	else {		// Change direction to Left
		LUpperLegAngle -= glm::radians(2.0f);
		RUpperLegAngle += glm::radians(2.0f);

		isTrans = true;		// Is Lower Leg translate?
		LLowerLegTrans -= glm::vec3(glm::radians(2.0f)*0.1, 0.0, 0.0);		//Lower Leg translation
		RLowerLegTrans += glm::vec3(glm::radians(2.0f)*0.1, 0.0, 0.0);		//Lower Leg translation
		LLowerLegAngle -= glm::radians(3.5f);
		RLowerLegAngle += glm::radians(3.5f);
		
		if (LUpperLegAngle < -glm::radians(30.0f)) {
			LUpperLegAngle = -glm::radians(30.0f);
			RUpperLegAngle = glm::radians(30.0f);
			isChange = false;		// Change direction to Right
			isTrans = false;		// pause lower legs translation
		}
	}

	glutPostRedisplay();
	glutTimerFunc(100, idle, 0);
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033:  // Escape key

	case 'o': case 'O':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void resize(int w, int h)
{
	float ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);

	projectMat = glm::perspective(glm::radians(65.0f), ratio, 0.1f, 100.0f);

	glutPostRedisplay();
}

//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Cube Horse");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(0, idle, 0);

	glutMainLoop();
	return 0;
}
