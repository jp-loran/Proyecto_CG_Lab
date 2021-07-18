/*
Semestre 2021-2
Pr�ctica : Iluminaci�n
Cambios en el shader, en lugar de enviar la textura en el shader de fragmentos, enviaremos el finalcolor
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"


//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Texture pisoTexture;
//MODELOS
Model Escenario_M;

Model torzoJack_M;
Model brazoIzqJack_M;
Model brazoDerJack_M;
Model piernaJack_M;

Model Perro_M;
Model murcielago_M;
/*ANIMACION*/
//perro
float offsetXperro = 0.0f;
float offsetYperro = 0.0f;
float offsetZperro = 0.0f;
float posXperro = 0.0f;
float posYperro = 0.0f;
float posZperro = 0.0f;
float anguloXperro = 0.0f;
float anguloZperro = 0.0f;
float anguloYperro = 0.0f;
bool avanzarDerecha = false;
bool hacerCirculo = false;
bool avanzarIzquierda = false;
//murcielago
float offsetXmur = 0.0f;
float offsetYmur = 0.0f;
float offsetZmur = 0.0f;
float posXmur = 0.0f;
float posYmur = 0.0f;
float posZmur = 0.0f;
float anguloXmur = 0.0f;
float anguloZmur = 0.0f;
float anguloYmur = 0.0f;
bool girarCirculo1 = false;
bool irACirculo2 = false;
bool girarCirculo2 = false;
bool irACirculo1 = false;
//***JACK
float offsetXjack = 0.0f;
float offsetYjack = 0.0f;
float offsetZjack = 0.0f;

float offsetxangulojack = 0.0f;
float offsetYangulojack = 0.0f;
float offsetZangulojack = 0.0f;
float posXjack = 0.0f;
float posYjack = 0.0f;
float posZjack = 0.0f;
float anguloXjack = 0.0f;
float anguloZjack = 0.0f;
float anguloYjack = 0.0f;


bool irAAmor = false;
bool irAPascua = false;
bool irANavidad = false;
bool irAMexico = false;
bool regreso = false;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}




void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};


	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}



void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	CreateShaders();

	/*VARIABLES de animacion*/
	float offset=0.0f;
	
	
	
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.3f);
													/*Texturas CARGA*/

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	
	pisoTexture = Texture("Textures/piso.jpg");
	pisoTexture.LoadTextureA();
	


												/*CARGA DE MODELOS*/
	/*escenario*/
	Escenario_M = Model();
	Escenario_M.LoadModel("Models/escenario.obj");

	/**/
	torzoJack_M= Model();
	torzoJack_M.LoadModel("Models/torzoJack.obj");

	brazoDerJack_M = Model();
	brazoDerJack_M.LoadModel("Models/brazoDerJack.obj");

	brazoIzqJack_M = Model();
	brazoIzqJack_M.LoadModel("Models/brazoIzqJack.obj");

	piernaJack_M = Model();
	piernaJack_M.LoadModel("Models/piernaJack.obj");


	Perro_M = Model();
	Perro_M.LoadModel("Models/perroalv.obj");

	murcielago_M= Model();
	murcielago_M.LoadModel("Models/bat2.obj");



	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/fondort.png");//derecha 
	skyboxFaces.push_back("Textures/Skybox/fondoleft.png");//izq
	skyboxFaces.push_back("Textures/Skybox/fondodn.png");//abajo
	skyboxFaces.push_back("Textures/Skybox/fondoup.png");//arriba
	skyboxFaces.push_back("Textures/Skybox/fondobk.png");//atras
	skyboxFaces.push_back("Textures/Skybox/fondofrente.png");//frente

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	

	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.7f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;
	//luz de helic�ptero
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
		0.5f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;
	//luz de faro

	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	//aumenta las vueltas
	int contador = 0;
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		//luz ligada a la c�mara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		
		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		//glEnable(GL_LIGHTING);

		glm::mat4 model(1.0);
		glm::mat4 modelAux(1.0);
										/*RENDERIZADO MODELOS*/
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();



		
		/**********************************************************************************ESCENARIO*****************************************/
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::rotate(model, 0 * toRadians, glm::vec3(1.0f, 0.0f, -29.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Escenario_M.RenderModel();
		
		/*PERRO*/
		offsetYperro+=1*deltaTime;
		posYperro = 3*sin(offsetYperro*toRadians);
		
			if (!avanzarDerecha && !hacerCirculo && !avanzarIzquierda) {
				/*hacer que avance a derecha*/
				if (posXperro <30.0) {
					offsetXperro += 0.005*deltaTime;
					posXperro += offsetXperro * deltaTime;
				}
				else {
					avanzarDerecha = true;
					hacerCirculo = false;
					avanzarIzquierda = false;
					offsetXperro = 0.0f;
				}
			}
			if (avanzarDerecha && !hacerCirculo && !avanzarIzquierda) {
				/*hacer que avance a hacer circulo*/
				if (offsetXperro < 360.0) {
					offsetXperro += 1*deltaTime;
					
					posXperro = (float)(31 * cos(offsetXperro*toRadians)); //calculo de x
					posZperro = (float)(31 * sin(offsetXperro*toRadians)); //calculo de y 
					if (anguloXperro > -270.0 && posXperro > -31.0) {
						offsetZperro -= 0.02 * deltaTime;//angulo
						anguloXperro += offsetZperro * deltaTime;
					}
					else if (anguloXperro < -270.0 && anguloXperro>-360.0 && posXperro < 0.0) {
						offsetZperro -= 0.03 * deltaTime;//angulo
						anguloXperro += offsetZperro * deltaTime;
					}
					//else i
						
				}
				else {
					if(anguloXperro<-360.0 && anguloXperro>-540.0) {
							offsetZperro -= 0.03 * deltaTime;//angulo
							anguloXperro += offsetZperro * deltaTime;
					}
					else {
						avanzarDerecha = false;
						hacerCirculo = true;
						avanzarIzquierda = false;
						offsetXperro = 0.0f;
						offsetZperro = 0.0f;//angulo
					}
					
				}
				
			}
			if (!avanzarDerecha && hacerCirculo && !avanzarIzquierda) {
				/*hacer que avance a avanzar izq*/
				if (posXperro > 0.0) {
					offsetXperro -= 0.005*deltaTime;
					posXperro += offsetXperro * deltaTime;
				}
				else {
					offsetZperro += 0.5 * deltaTime;//angulo
					if (anguloXperro < 0.0) {
						anguloXperro += offsetZperro * deltaTime;
					}
					else {
						avanzarDerecha = false;
						hacerCirculo = false;
						avanzarIzquierda = false;
						offsetXperro = 0.0f;
						offsetZperro = 0.0f;
					}	
				}
			}
		
		
	
		
		glm::vec3 despPerro = glm::vec3(posXperro, posYperro, posZperro);
		glm::vec3 posPerro= glm::vec3(0.0f, -2.0f, 0.0f)+despPerro;
		//pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		//	0.0f, 0.0f,//intensidad
		//	0.0f,0.0f,0.0f,//posPerro.x,posPerro.y,posPerro.z,//posicion
		//	0.5f, 0.2f, 0.1f);//4(.2)^2-2*0.5*0.1
		//pointLightCount++;
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		model = glm::translate(model, posPerro);
		model = glm::rotate(model,(anguloXperro)*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Perro_M.RenderModel();

		/***************************************************MURCIELAGO***********************************************************************/
		
			if (!girarCirculo1 && !irACirculo2 && !girarCirculo2 && !irACirculo1) {
				//girar circulo 1
				if (offsetXmur < 360.0) {
					offsetXmur += 2.5 * deltaTime;

					posXmur = (float)(10 * cos(offsetXmur*toRadians)); //calculo de x
					posZmur = (float)(10 * sin(offsetXmur*toRadians)); //calculo de y 
				}
				else
				{
					if (anguloYmur < 0.0) {
						offsetYmur += 0.5*deltaTime;
						anguloYmur += offsetYmur * deltaTime;
						if (anguloZmur > 0.0) {
							offsetZmur -= 0.5*deltaTime;
							anguloZmur += offsetZmur * deltaTime;
						}
					}
					else {
						girarCirculo1 = true;
						irACirculo2 = false;
						girarCirculo2 = false;
						irACirculo1 = false;
						offsetXmur = 0.0f;
						offsetYmur = 0.0f;
						offsetZmur = 0.0f;
					}
					
				}

			}
			if (girarCirculo1 && !irACirculo2 && !girarCirculo2 && !irACirculo1) {
				if (posXmur < 60.0) {
					offsetXmur += 0.05 * deltaTime;
					posXmur += offsetXmur * deltaTime;
				}
				else
				{
					girarCirculo1 = false;
					irACirculo2 = true;
					girarCirculo2 = false;
					irACirculo1 = false;
					offsetXmur = 0.0f;
				}

			}
			if (!girarCirculo1 && irACirculo2 && !girarCirculo2 && !irACirculo1) {
				if (offsetXmur < 360.0) {
					offsetXmur += 2.5 * deltaTime;

					posXmur = (float)(10 * cos(offsetXmur*toRadians))+60; //calculo de x
					posZmur = (float)(10 * sin(offsetXmur*toRadians)); //calculo de y 
				}
				else
				{
					//aqui gira 180
					if (anguloYmur>-180.0 ) {
						offsetYmur -= 0.5*deltaTime;
						anguloYmur += offsetYmur * deltaTime;
						if (anguloZmur < 90) {
							offsetZmur -= 0.5*deltaTime;
							anguloZmur -= offsetZmur * deltaTime;
						}
					}
					else {
						girarCirculo1 = false;
						irACirculo2 = false;
						girarCirculo2 = true;
						irACirculo1 = false;
						offsetXmur = 0.0f;
						offsetZmur = 0.0f;
						offsetYmur = 0.0f;
					}
				}
			}
			if (!girarCirculo1 && !irACirculo2 && girarCirculo2 && !irACirculo1) {
				if (posXmur >0.0) {
					offsetXmur += 0.05 * deltaTime;
					posXmur -= offsetXmur * deltaTime;
				}
				else
				{
							girarCirculo1 = false;
							irACirculo2 = false;
							girarCirculo2 = false;
							irACirculo1 = false;
							offsetXmur = 0.0f;
							offsetZmur = 0.0f;
							offsetYmur = 0.0f;
				}
			}
		
		
		glm::vec3 despMurcielago = glm::vec3(posXmur, posYmur, posZmur);
		glm::vec3 posMurcielago = glm::vec3(-31.0f, 100.0f, 30.0f) + despMurcielago;
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::translate(model, posMurcielago);
		model = glm::rotate(model, (-45+anguloZmur)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (90+anguloYmur) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		murcielago_M.RenderModel();

		/*********************************************************************Jack********************************************/
		/******************ir al arbol del amor y la amistad*/

		if (mainWindow.iniciaAnim()) {
		if (anguloZjack < 30.0) {
			offsetZangulojack += 4.5*deltaTime;
			anguloZjack = 10 * sin(offsetZangulojack * toRadians);
		}
			if (!irAAmor && !irAPascua && !irANavidad && !irAMexico && !regreso) {
				if (posZjack < 18.0) {
					offsetZjack += 0.003*deltaTime;
					posZjack += offsetZjack * deltaTime;
				}
				else {
					if (anguloYjack < 90.0) {
						offsetYangulojack += 0.05*deltaTime;
						anguloYjack += offsetYangulojack;
					}
					else {
						if (posXjack < 12.0) {
							offsetXjack += 0.003*deltaTime;
							posXjack += offsetXjack * deltaTime;
						}
						else {
							if (anguloYjack < 180.0) {
								offsetYangulojack += 0.05*deltaTime;
								anguloYjack += offsetYangulojack;
							}
							else {
								irAAmor = true;
								irAPascua = false;
								irANavidad = false;
								irAMexico = false;
								regreso = false;
								offsetZjack = 0.0f;
								offsetXjack = 0.0f;
								offsetYangulojack = 0.0f;
							}

						}
					}
				}
				printf("\nyendo a amor");
			}
			/**************************************ir al arbol de pascua***/
			if (irAAmor && !irAPascua && !irANavidad && !irAMexico && !regreso) {
				if (posZjack > -18.0) {
					offsetZjack += 0.005*deltaTime;
					posZjack -= offsetZjack * deltaTime;
					if (posXjack < 20.0) {
						offsetXjack += 0.004*deltaTime;
						posXjack += offsetXjack * deltaTime;
					}
				}
				else {
					if (anguloYjack < 270.0) {
						offsetYangulojack += 0.05*deltaTime;
						anguloYjack += offsetYangulojack;
					}
					else
					{
						irAAmor = false;
						irAPascua = true;
						irANavidad = false;
						irAMexico = false;
						regreso = false;
						offsetZjack = 0.0f;
						offsetXjack = 0.0f;
						offsetYangulojack = 0.0f;
					}
				}
				printf("\nyendo a pascua");
			}
			/**************************************************ir al arbol de navidad*/
			if (!irAAmor && irAPascua && !irANavidad && !irAMexico && !regreso) {
				if (posXjack > -9.0) {
					offsetXjack -= 0.005*deltaTime;
					posXjack += offsetXjack * deltaTime;
				}
				else {
					if (anguloYjack < 360.0) {
						offsetYangulojack += 0.05*deltaTime;
						anguloYjack += offsetYangulojack;
					}
					else
					{
						irAAmor = false;
						irAPascua = false;
						irANavidad = true;
						irAMexico = false;
						regreso = false;
						offsetZjack = 0.0f;
						offsetXjack = 0.0f;
						offsetYangulojack = 0.0f;
					}
				}
				printf("\nyendo a navidad");
			}
			/***********************************************************ir al arbol mexicano****/
			if (!irAAmor && !irAPascua && irANavidad && !irAMexico && !regreso) {
				if (posZjack < 10.0) {
					offsetZjack += 0.005*deltaTime;
					posZjack += offsetZjack * deltaTime;
				}
				else
				{
					if (anguloYjack < 450.0) {
						offsetYangulojack += 0.05*deltaTime;
						anguloYjack += offsetYangulojack;
					}
					else
					{
						irAAmor = false;
						irAPascua = false;
						irANavidad = false;
						irAMexico = true;
						regreso = false;
						offsetZjack = 0.0f;
						offsetXjack = 0.0f;
						offsetYangulojack = 0.0f;
					}
				}
				printf("\nyendo a mexico");
			}
			/**********************************************regreso al origen*/
			if (!irAAmor && !irAPascua && !irANavidad && irAMexico && !regreso) {
				if (posXjack < 0.0) {
					offsetXjack += 0.005*deltaTime;
					posXjack += offsetXjack * deltaTime;
				}
				else {
					if (anguloYjack < 540.0 && posZjack>0.0 && posXjack >= 0.0) {
						offsetYangulojack += 0.05*deltaTime;
						anguloYjack += offsetYangulojack;
					}
					else
					{
						if (posZjack > 0.0) {
							offsetZjack -= 0.003*deltaTime;
							posZjack += offsetZjack * deltaTime;
						}
						else {
							if (anguloYjack > 360.0) {
								offsetYangulojack += 0.05*deltaTime;
								anguloYjack -= offsetYangulojack * deltaTime;
							}
							else
							{
								irAAmor = false;
								irAPascua = false;
								irANavidad = false;
								irAMexico = false;
								regreso = false;
								offsetZjack = 0.0f;
								offsetXjack = 0.0f;
								offsetYangulojack = 0.0f;
								anguloYjack = 0.0f;
							}
						}
					}
				}
				printf("\nyendo a centro");
			}
		}
		
		/*printf("\nposx:%.2f", posXjack);
		printf("\nposZ:%.2f", posZjack);
		printf("\nangulooY:%.2f", anguloYjack);
		printf("\nangulooZ	:%.2f", anguloZjack);*/
		
		glm::vec3 despJack = glm::vec3(posXjack, posYjack, posZjack);
		glm::vec3 posJack= glm::vec3(0.0f, -2.0f, 0.0f) + despJack;
		model = glm::mat4(1.0);
		model = glm::translate(model,posJack);
		modelAux = model;//jerarquizacion
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, anguloYjack * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		torzoJack_M.RenderModel();
		/********brazoderecho*********/
		model = glm::mat4(1.0);
		model = modelAux;
		model = glm::rotate(model, anguloYjack * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-1.10f, 9.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f)); 
		model = glm::rotate(model, anguloZjack * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoDerJack_M.RenderModel();
		/*************brazoizquierdo**************/
		model = glm::mat4(1.0);
		model = modelAux;
		model = glm::rotate(model, anguloYjack * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(1.0f, 9.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, -anguloZjack * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoIzqJack_M.RenderModel();
		/*************pierna1*****************/
		model = glm::mat4(1.0);
		model = modelAux;
		model = glm::rotate(model, anguloYjack * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.26f, 6.34f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, -anguloZjack * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		piernaJack_M.RenderModel();
		/****************pierna2*******************/
		model = glm::mat4(1.0);
		model = modelAux;
		model = glm::rotate(model, anguloYjack * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.22f, 6.34f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, anguloZjack * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		piernaJack_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
