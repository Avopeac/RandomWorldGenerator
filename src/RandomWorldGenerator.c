// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

#define PI 3.14

mat4 projectionMatrix;
int mapWidth, mapHeight;

GLuint windowWidth = 1000;
GLuint windowHeight = 600;

GLuint indexAdds[6][2] = {  {0, -1},
							{1, -1},
							{1, 0},
							{0, 1},
							{-1, 1},
							{-1, 0}};

vec3 calcNormal(vec3 t1, vec3 t2, vec3 t3)
{
	vec3 v1,v2,v;
	v1 = VectorSub(t2, t1);
	v2 = VectorSub(t3, t1);
	v = CrossProduct(v1, v2);
	return v;
}

Model* GenerateTerrain(TextureData *tex)
{
	

	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z, i, tx, tz;
	vec3 t1, t2, t3;
	vec3 tempNormal, totNormal;
	bool oob;
	Model* model;
	GLfloat *vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = (GLfloat*)malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = (GLfloat*)malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = (GLuint*)malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 10.0 * windowWidth/windowHeight;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.
			//normalArray[(x + z * tex->width)*3 + 0] = 0.0;
			//normalArray[(x + z * tex->width)*3 + 1] = 1.0;
			//normalArray[(x + z * tex->width)*3 + 2] = 0.0;
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
		//calc normals
	for(x = 0; x < tex->width; x++)
	{
		for(z = 0; z < tex->height; z++)
		{
			totNormal = SetVector(0,1,0);
			for(i = 0; i < 6; i++)
			{
				oob = false;
				tempNormal = SetVector(0,0,0);
				t1.x = vertexArray[(x + z * tex->width)*3 + 0];
				t1.y = vertexArray[(x + z * tex->width)*3 + 1]; 
				t1.z = vertexArray[(x + z * tex->width)*3 + 2]; 

				
				tx = x + indexAdds[i%6][0];
				tz = z + indexAdds[i%6][1];
				if(tx < 0 || tx >= tex->width || tz < 0 || tz >= tex->height) //if outside map, set to vector.up 
				{
					oob = true;  
				}
				else
				{
					t2.x = vertexArray[(tx + tz * tex->width)*3 + 0];
					t2.y = vertexArray[(tx + tz * tex->width)*3 + 1]; 
					t2.z = vertexArray[(tx + tz * tex->width)*3 + 2];	
				}

				tx = x + indexAdds[(i+1)%6][0];
				tz = z + indexAdds[(i+1)%6][1];
				if(tx < 0 || tx >= tex->width || tz < 0 || tz >= tex->height) //if outside map, set to vector.up 
				{
					oob = true; 
				}
				else
				{
					t3.x = vertexArray[(tx + tz * tex->width)*3 + 0];
					t3.y = vertexArray[(tx + tz * tex->width)*3 + 1]; 
					t3.z = vertexArray[(tx + tz * tex->width)*3 + 2];
				}

				

				if(oob) tempNormal = SetVector(0,-1,0);
				else
				{
					tempNormal = calcNormal(t1, t2, t3);
				}
				totNormal = VectorAdd(totNormal, tempNormal);
			}
			totNormal = Normalize(totNormal);
			//fprintf(stderr, "NormalX: ");
			//fprintf(stderr, "%.3g", totNormal.x );
			//fprintf(stderr, "\t\tNormalY: ");
			//fprintf(stderr, "%.3g", totNormal.y );
			//fprintf(stderr, "\t\tNormalZ: ");
			//fprintf(stderr, "%.3g", totNormal.z );
			//fprintf(stderr, "\n" );
			normalArray[(x + z * tex->width)*3 + 0] = totNormal.x;
			normalArray[(x + z * tex->width)*3 + 1] = totNormal.y;
			normalArray[(x + z * tex->width)*3 + 2] = totNormal.z;
		}
	}
	// End of terrain generation
	
	// Create Model and upload to GPU:
	mapWidth = tex->width;
	mapHeight = tex->height;
	model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}


// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain
GLfloat elapsedTime;
GLfloat deltaTime;
vec3 camPos; 
vec3 camRot;

	

void init(void)
{
	// GL inits
	dumpInfo();
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 100.0);

	// Load and compile shader
	program = loadShaders("shaders/terrain.vert", "shaders/terrain.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("textures/maskros512.tga", &tex1);

	camPos = SetVector(-100,-2,-100);
// Load terrain data
	
	LoadTGATextureData("textures/fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");
}

GLfloat calculateHeight(GLfloat x, GLfloat z)
{
	GLfloat height;
	vec3 rayOrigin, ray, newRay, normal, point, intersection;
	vec3 v2, v3;
	float d, t;
	int tileX = floor(x);
	int tileZ = floor(z);
	
	if( (z - tileZ) < 1 - (x - tileX) ) //1st triangle
	{
		//fprintf(stderr, "\n1st triangle: ");

		point.x = tm->vertexArray[(tileX + tileZ * mapWidth)*3 + 0];
		point.y = tm->vertexArray[(tileX + tileZ * mapWidth)*3 + 1];
		point.z = tm->vertexArray[(tileX + tileZ * mapWidth)*3 + 2];
		
		v2.x = tm->vertexArray[(tileX + 1 + tileZ * mapWidth)*3 + 0];
		v2.y = tm->vertexArray[(tileX + 1 + tileZ * mapWidth)*3 + 1];
		v2.z = tm->vertexArray[(tileX + 1 + tileZ * mapWidth)*3 + 2];
		
		v3.x = tm->vertexArray[(tileX + (tileZ + 1)* mapWidth)*3 + 0];
		v3.y = tm->vertexArray[(tileX + (tileZ + 1)* mapWidth)*3 + 1];
		v3.z = tm->vertexArray[(tileX + (tileZ + 1)* mapWidth)*3 + 2];
	}
	else //2nd triangle
	{
		//fprintf(stderr, "\n2nd triangle: ");
	
		point.x = tm->vertexArray[(tileX + 1 + (tileZ + 1)* mapWidth)*3 + 0];
		point.y = tm->vertexArray[(tileX + 1 + (tileZ + 1)* mapWidth)*3 + 1];
		point.z = tm->vertexArray[(tileX + 1 + (tileZ + 1)* mapWidth)*3 + 2];
	
		v2.x = tm->vertexArray[(tileX + (tileZ + 1)* mapWidth)*3 + 0];
		v2.y = tm->vertexArray[(tileX + (tileZ + 1)* mapWidth)*3 + 1];
		v2.z = tm->vertexArray[(tileX + (tileZ + 1)* mapWidth)*3 + 2];
	
		v3.x = tm->vertexArray[(tileX + 1 + tileZ * mapWidth)*3 + 0];
		v3.y = tm->vertexArray[(tileX + 1 + tileZ * mapWidth)*3 + 1];
		v3.z = tm->vertexArray[(tileX + 1 + tileZ * mapWidth)*3 + 2];
	}
	height = point.y + 3;
	normal = Normalize(calcNormal(point, v2, v3));
	rayOrigin = SetVector(x, height + 2, z);
	ray = SetVector(0,-1,0);

	//Calculate plane
	d = DotProduct(normal, point);

	if(DotProduct(normal, ray) == 0) return 0;

	t = (d - DotProduct(normal, rayOrigin)) / DotProduct(normal, ray);
	newRay = ScalarMult(ray, t);
	intersection = VectorAdd(rayOrigin, newRay);

	//fprintf(stderr, "\nTileX: ");
	//fprintf(stderr, "%.3g", x);
	//fprintf(stderr, "\tTileZ: ");
	//fprintf(stderr, "%.3g", z);
	//fprintf(stderr, "\tHeight: ");
	//fprintf(stderr, "%.3g", height);
	

	/*fprintf(stderr, "NormalX: ");
	fprintf(stderr, "%.3g", normal.x );
	fprintf(stderr, "\t\tNormalY: ");
	fprintf(stderr, "%.3g", normal.y );
	fprintf(stderr, "\t\tNormalZ: ");
	fprintf(stderr, "%.3g", normal.z );
	fprintf(stderr, "\n" );
		*/
	//fprintf(stderr, "\nHeight: ");
	//fprintf(stderr, "%.3g", intersection.y );
	
	return intersection.y;
}

void display(void)
{
	mat4 total, modelView, camMatrix;
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	camMatrix = Mult(Mult(Rx(camRot.x), Ry(camRot.y) ) , T(camPos.x, camPos.y, camPos.z) );
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");
	
	glutSwapBuffers();
}

bool camLeft, camRight, camUp, camDown;
void UpdateCamera()
{
	vec3 vecZ = SetVector(0,0,0);
	float speed = 0.5;
	vec3 forwardDirection = ScalarMult(SetVector(sin(camRot.y), 0.0f, cos(camRot.y)), speed);
	vec3 sidewaysDirection = ScalarMult(SetVector(sin(camRot.y + PI/2), 0.0f, cos(camRot.y + PI/2)), speed);
	if(GetAsyncKeyState('W'))
	{
		camPos = VectorAdd(camPos, forwardDirection);
	}
	if(GetAsyncKeyState('S'))
	{
		camPos = VectorAdd(camPos, VectorSub(vecZ ,forwardDirection));
	}
	if(GetAsyncKeyState('A'))
	{
		camPos = VectorAdd(camPos, sidewaysDirection);
	}
	if(GetAsyncKeyState('D'))
	{
		camPos = VectorAdd(camPos, VectorSub(vecZ, sidewaysDirection));
	}
	if(GetAsyncKeyState(VK_LEFT))
	{
		camRot.y +=  PI * deltaTime; 
	}
	if(GetAsyncKeyState(VK_RIGHT))
	{
		camRot.y -=  PI * deltaTime; 
	}
	if(GetAsyncKeyState(VK_UP))
	{
		camRot.x -=  PI * deltaTime;
	}
	if(GetAsyncKeyState(VK_DOWN))
	{
		camRot.x +=  PI * deltaTime;
	}
	if(camUp)
	{
		camRot.x -= PI*0.5*deltaTime;
	}
	if(camDown)
	{
		camRot.x += PI*0.5*deltaTime;
	}
	if(camRight)
	{
		camRot.y -= PI*0.5*deltaTime;
	}
	if(camLeft)
	{
		camRot.y += PI*0.5*deltaTime;
	}
	if(camPos.x > -1) camPos.x = -1;
	if(camPos.z > -1) camPos.z = -1;
	if(camPos.x < -mapWidth + 1) camPos.x = -mapWidth + 1;
	if(camPos.z < -mapHeight + 1) camPos.z = -mapHeight + 1;
	

	camPos.y = -calculateHeight(-camPos.x, -camPos.z) - 2;
}
void timer(int i)
{
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	deltaTime = t - elapsedTime;
	elapsedTime = t;
	UpdateCamera();
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}


void mouse(int x, int y)
{
	camUp = camDown = camLeft = camRight = false;
	//printf("%d %d\n", x, y);
	if(y <= 20)
	{
		camUp = true;
	}
	else if(y >= windowHeight - 20)
	{
		camDown = true;
	}
	if(x <= 20)
	{
		camLeft = true;
	}
	if(x >= windowWidth - 20)
	{
		camRight = true;
	}
	
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	//glutInitContextVersion(3, 2);
	glutInitWindowSize (windowWidth, windowHeight);
	glutCreateWindow ("Random World Generator");
	glutDisplayFunc(display);
	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit (1);
	}
	init ();
	//initKeymapManager();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
