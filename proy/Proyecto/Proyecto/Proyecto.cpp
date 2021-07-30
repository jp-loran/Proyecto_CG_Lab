/*
Semestre 2021-2
Práctica : Iluminación
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


//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
//para musica 
#include <irrklang/irrKlang.h>
using namespace irrklang;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;
Camera camera2;

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
Model Corazon_M;
Model MurCuerpo_M;
Model ala1;
Model ala2;
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
//murcielago1 COMPLEJA
float offsetXmur = 0.0f;
float offsetYmur = 0.0f;
float offsetYmur2 = 0.0f;
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
/********MUCRCIELAGO 3 compleja***************/
float offsetXmur3 = 0.0f;
float offsetYmur3 = 0.0f;
float offsetZmur3 = 0.0f;
float posXmur3 = 0.0f;
float posYmur3 = 0.0f;
float posZmur3 = 0.0f;
float anguloXmur3 = 0.0f;
float anguloZmur3 = 0.0f;
float anguloYmur3 = 0.0f;
//murcielago2 BASICA
float offsetXmur2 = 0.0f;
float offsetZmur2 = 360.0f;
float posXmur2 = 0.0f;
float posYmur2 = 0.0f;
float posZmur2 = 0.0f;
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
/*Corazon*/
float offsetXcorazon = 360.0f;
float offsetYcorazon = 0.0f;
float offsetAnguloCorazon = 0.0f;
float posXcorazon = 0.0f;
float posYcorazon = 0.0f;
float posZcorazon = 0.0f;
float anguloCorazon = 0.0f;

/*para luces dia noche*/
float tiempo = 0.0;
int contador=0;
//espectaculo
float tiempo2 = 0.0;
int contador2 = 0;
int cambio2=0;
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


//cálculo del promedio de las normales para sombreado de Phong
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
	/*para musica*/
	irrklang::ISoundEngine* engine= irrklang::createIrrKlangDevice();;// tema general
	irrklang::ISoundEngine* engine2 = irrklang::createIrrKlangDevice();;// sonido del perro
	irrklang::ISoundEngine* engine3 = irrklang::createIrrKlangDevice();;// para espectaculo
	engine->play2D("Music/principal.mp3", true);
	engine->setSoundVolume(0.5f);
	/***********/
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	bool cambio = true;
	CreateObjects();
	CrearCubo();
	CreateShaders();

	/*VARIABLES de animacion*/
	float offset=0.0f;
	glm::vec3 posPerro;
	
	
	camera = Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.0f, 0.3f);
	camera2 = Camera(glm::vec3(0.0f, 10.0f, 0.0f ), glm::vec3(1.0f, 0.0f, 1.0f), -60.0f, 0.0f, 2.0f, 0.3f);
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
	/*Escenario_M = Model();
	Escenario_M.LoadModel("Models/escenario.obj");*/

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

	MurCuerpo_M = Model();
	MurCuerpo_M.LoadModel("Models/murcielagoCuerpo.obj");
	ala1 = Model();
	ala1.LoadModel("Models/ala1.obj");
	ala2 = Model();
	ala2.LoadModel("Models/ala2.obj");

	Corazon_M = Model();
	Corazon_M.LoadModel("Models/corazon.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/fondort.png");//derecha 
	skyboxFaces.push_back("Textures/Skybox/fondoleft.png");//izq
	skyboxFaces.push_back("Textures/Skybox/fondodn.png");//abajo
	skyboxFaces.push_back("Textures/Skybox/fondoup.png");//arriba
	skyboxFaces.push_back("Textures/Skybox/fondobk.png");//atras
	skyboxFaces.push_back("Textures/Skybox/fondofrente.png");//frente
	std::vector<std::string> skyboxFaces2;
	skyboxFaces2.push_back("Textures/Skybox/fondodiart.png");//derecha 
	skyboxFaces2.push_back("Textures/Skybox/fondodialeft.png");//izq
	skyboxFaces2.push_back("Textures/Skybox/fondodn.png");//abajo
	skyboxFaces2.push_back("Textures/Skybox/fondodiaup.png");//arriba
	skyboxFaces2.push_back("Textures/Skybox/fondodiabk.png");//atras
	skyboxFaces2.push_back("Textures/Skybox/fondodiafrente.png");//frente
	
	

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		2.0f, 2.0f,//intensidad
		0.0f,0.0f,0.0f,//posPerro.x,posPerro.y,posPerro.z,//posicion
		0.5f, 0.2f, 0.1f);//4(.2)^2-2*0.5*0.1
	pointLightCount++;
	/*luna*/
	pointLights[1] = PointLight(0.2f, 0.0f, 1.0f,
		3.0f, 3.0f,//intensidad
		-110.00, 60.00, -45.00, //
		0.000000000000005f, 0.1f, 0.000000000050f);//4(.2)^2-2*0.5*0.1
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna amor y amistad
	spotLights[0] = SpotLight(0.5f, 0.0f, 0.2f,
		0.0f, 0.0f,
		17.0f, 100.0f, 22.0f,//pos
		0.0f, -0.5f, 0.0f,//dir
		1.0f, 0.0f, 0.0f,//4*25-2
		10.0f);
	spotLightCount++;
	//linterna pascua
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,
		22.0f, 100.0f, -22.0f,//pos
		0.0f, -0.5f, 0.0f,//dir
		1.0f, 0.0f, 0.0f,//4*25-2
		10.0f);
	spotLightCount++;
	//navidad
	spotLights[2] = SpotLight(0.0f, 0.3f, 0.1f,
		0.0f, 0.0f,
		-13.0f, 100.0f, -21.0f,//pos
		0.0f, -0.5f, 0.0f,//dir
		1.0f, 0.0f, 0.0f,//4*25-2
		10.0f);
	spotLightCount++;
	//mexico
	spotLights[3] = SpotLight(0.0f, 1.0f, 0.0f,
		0.0f, 0.0f,
		-13.0f, 100.0f, 17.0f,//pos
		0.0f, -0.5f, 0.0f,//dir
		1.0f, 0.0f, 0.0f,//4*25-2
		10.0f);
	spotLightCount++;

	

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
		
		
		
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		if (mainWindow.cambio3ra()) {
			//camera2.keyControl(mainWindow.getsKeys(), deltaTime);
			camera2.mouseControl(0.0f, -89.0);//aérea
			camera2.setPosition(glm::vec3 (0.0f+mainWindow.getmuevex(), 100.0f, 0.0f+mainWindow.getmuevez()));
			skybox.DrawSkybox(camera2.calculateViewMatrix(), projection);
			printf("\n %.2f", mainWindow.getYChange());
		}
		else {//3ra persona
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), 0.0f);
			skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		}
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		if (mainWindow.cambio3ra()) {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera2.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera2.getCameraPosition().x, camera2.getCameraPosition().y, camera2.getCameraPosition().z);
		}
		else {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}
		

		//luz ligada a la cámara de tipo flash 
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		//información al shader de fuentes de iluminación
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
		
		/***********************************************************************************PERRO************************************************************/
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
				pointLights[0].SetColor(1.0f,0.0f,7.0f);
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
				pointLights[0].SetColor(1.0f, 0.0f, 0.0f);
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
						if (!engine->isCurrentlyPlaying("Music/perro.mp3")) {
							engine2->play2D("Music/perro.mp3");
							engine2->setSoundVolume(0.30f);
						}
					}
					else {
						engine2->setAllSoundsPaused();
						avanzarDerecha = false;
						hacerCirculo = false;
						avanzarIzquierda = false;
						offsetXperro = 0.0f;
						offsetZperro = 0.0f;
					}	
				}
				pointLights[0].SetColor(0.0f, 0.5f, 0.5f);
			}
		
		
		glm::vec3 despPerro = glm::vec3(posXperro, posYperro, posZperro);
		posPerro= glm::vec3(0.0f, -2.0f, 0.0f)+despPerro;
		pointLights[0].SetPos(posPerro+ glm::vec3(1.0f, 9.0f, 0.0f));
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		model = glm::translate(model, posPerro);
		model = glm::rotate(model,(anguloXperro)*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Perro_M.RenderModel();

		/***************************************************************MURCIELAGO***********************************************************************/
		
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
		
			if (offsetYmur2 > 360) {
				offsetYmur2 = 0.0;
			}
			offsetYmur2 += 50.0*deltaTime;
			posYmur = sin(offsetYmur2*toRadians);
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
		/***************************************************************MURCIELAGO 2BASICA**********************************************/
		if (mainWindow.iniciaAnim2()) {
			offsetZmur2 = 0.0f;

		}
		if (offsetZmur2<360) {
			offsetZmur2 += 2.0*deltaTime;
			posXmur2 = 50*sin(offsetZmur2*toRadians);
			posYmur2 = sin(offsetYmur2*toRadians);
		}
		
		glm::vec3 despMurcielago2 = glm::vec3(posXmur2, posYmur2, posZmur2);
		glm::vec3 posMurcielago2 = glm::vec3(30.0f, 100.0f, -30.0f) + despMurcielago2;
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::translate(model, posMurcielago2);
		model = glm::rotate(model, (45)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (-90) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		murcielago_M.RenderModel();
		/**************************************************MURCIELAGO COMPLEJA CON ALAS*********************************************************/
		offsetZmur3 += 2.0*deltaTime;
		posXmur3 = 50 * sin(offsetZmur3*toRadians);
		posYmur3 = sin(offsetYmur2*toRadians);

		posXmur3 = (float)(40 * cos(offsetZmur3*toRadians)); //calculo de x
		posZmur3 = (float)(20 * sin(offsetZmur3*toRadians)); //calculo de y 

		glm::vec3 despMurcielago3 = glm::vec3(posXmur3, 0.0f, posZmur3);
		glm::vec3 posMurcielago3 = glm::vec3(30.0f, 100.0f, -80.0f) + despMurcielago3;
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::translate(model, posMurcielago3);
		modelAux = model;
		model = glm::rotate(model, (45)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (-90) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		MurCuerpo_M.RenderModel();

		model = glm::mat4(1.0);
		model = modelAux;
		model = glm::rotate(model, (45+posYmur3*10)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (-90) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		ala1.RenderModel();

		model = glm::mat4(1.0);
		model = modelAux;
		model = glm::rotate(model, (45 + posYmur3 * 10)*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, (-90) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		ala2.RenderModel();
		/*********************************************************************Jack***************************************************/
		

		//if (mainWindow.iniciaAnim()) {
		/*          Brazeo                */
			if (anguloZjack < 30.0) {
				offsetZangulojack += 4.5*deltaTime;
				anguloZjack = 10 * sin(offsetZangulojack * toRadians);
			}
			/******************ir al arbol del amor y la amistad*/
			if (!irAAmor && !irAPascua && !irANavidad && !irAMexico && !regreso) {
				if (posZjack < 18.0) {
					offsetZjack += 0.005*deltaTime;
					posZjack += offsetZjack ;
				}
				else {
					if (anguloYjack < 90.0) {
						offsetYangulojack += 0.05*deltaTime;
						anguloYjack += offsetYangulojack;
					}
					else {
						if (posXjack < 12.0) {
							offsetXjack += 0.005*deltaTime;
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
		//}
		
		/*printf("\nposx:%.2f", posXjack);
		printf("\nposZ:%.2f", posZjack);
		printf("\nangulooY:%.2f", anguloYjack);
		printf("\nangulooZ	:%.2f", anguloZjack);*/
			

		
		glm::vec3 despJack = glm::vec3(posXjack, posYjack, posZjack);
		glm::vec3 posJack= glm::vec3(0.0f + despJack.x, -2.0f + despJack.y, 0.0f + despJack.z) ;
		model = glm::mat4(1.0);
		model = glm::translate(model,posJack);
		modelAux = model;//jerarquizacion
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, anguloYjack * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
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
		/***************************************************Corazón flotante******************************************************/
		if (mainWindow.iniciaAnim()) {
			offsetXcorazon = 0.0f;
		}
		if (offsetXcorazon<360.0) {
			offsetXcorazon += 150.5*toRadians;
			posXcorazon = (float)(1 * cos(offsetXcorazon*toRadians)); //calculo de x
			posYcorazon = (float)(1 * sin(offsetXcorazon*toRadians)); //calculo de y 

		}
		glm::vec3 despCorazon = glm::vec3(posXcorazon, posYcorazon, posZcorazon);
		glm::vec3 posCorazon= glm::vec3(12.0f , 0.0f , 22.0f )+despCorazon;
		model = glm::mat4(1.0);
		model = glm::translate(model, posCorazon);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, anguloZjack * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Corazon_M.RenderModel();
		tiempo += 0.01 * deltaTime;
		//printf("\nDelta			: %.2f",tiempo);
		if (((int)tiempo)%10==0 ) {//10.0-10.1 .. 11.0
			if (contador == 0) {
				cambio = !cambio;
				//printf("\nCambio");
				contador ++;
				if (cambio) {
					skybox = Skybox(skyboxFaces);
					pointLights[0].SetIntensity(3.0, 3.0);
					pointLights[1].SetIntensity(3.0,3.0);
				}
				else {
					pointLights[0].SetIntensity(0.0, 0.0);
					skybox = Skybox(skyboxFaces2);
					pointLights[1].SetIntensity(0.0, 0.0);
				}
			}
		}
		else {
			contador = 0;
		}
		
		if (mainWindow.iniciaEspectaculo()) {
			if (!engine3->isCurrentlyPlaying("Music/fiesta.mp3")) {
				//irrklang::ISoundEngine* engine3 = irrklang::createIrrKlangDevice();;// para
				engine3->play2D("Music/fiesta.mp3");
				engine3->setSoundVolume(0.5f);
			}
			
			tiempo2 +=0.15* deltaTime;
			spotLights[0].SetIntensity(1.0,2.0);//amor
			spotLights[1].SetIntensity(3.0, 3.0);//pascua
			spotLights[2].SetIntensity(3.0, 3.0);//navidad
			spotLights[3].SetIntensity(3.0, 3.0);//mexicana
			//spotLights[0].SetColor(1.0, 0.0, 1.0);
			if (((int)tiempo2)%2==0) {
			//if(fmod(tiempo2,0.5)==0.0){
				if (contador2 == 0) {
					contador2++;
					cambio2++;
					if (cambio2 == 1) {
						spotLights[3].SetColor(0.0,1.0,0.0);
						spotLights[1].SetColor(0.0, 0.5, 0.5);
					}
					else if (cambio2==2) {
						spotLights[3].SetColor(1.0, 1.0, 1.0);
						spotLights[1].SetColor(0.0, 0.0, 0.4);
					}
					else if (cambio2==3)
					{
						spotLights[3].SetColor(1.0, 0.0, 0.0);
						spotLights[1].SetColor(0.5, 0.0, 0.5);
					}
					else {
						cambio2 = 0;
					}
				}
			}
			else {
				contador2 = 0;
			}
		}
		else {
			engine3->stopAllSounds();
			spotLights[0].SetIntensity(0.0, 0.0);
			spotLights[1].SetIntensity(0.0, 0.0);
			spotLights[2].SetIntensity(0.0, 0.0);
			spotLights[3].SetIntensity(0.0, 0.0);
		}

		

		//camara
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

