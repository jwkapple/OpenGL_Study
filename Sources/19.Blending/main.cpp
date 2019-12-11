
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.h"
#include "modelclass.h"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>



#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
bool blinn = false;
bool blinnKeyPressed = false;

void frameBuffer_size_callback(GLFWwindow* window, int width, int height);

void processinput(GLFWwindow* window);

unsigned int loadTexture(const char *path);

// camera
glm::vec3 cameraPos = glm::vec3(-3.0f, 1.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,0.0f);

glm::vec3 lightPos = glm::vec3(0.0f, 3.0f, 3.0f);

// timing
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// cursor position
GLfloat lastX = 400;
GLfloat lastY = 300;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set OpenGL profile to Core (For modern OpenGL use)

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Tutorial", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed creating window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set context to window.
	glfwMakeContextCurrent(window);


	// Initialize GLAD before using GLFW functions.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to intialize GLAD" << std::endl;

		return -1;
	}

	// depth buffer
	// -------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glDepthMask(GL_FALSE);
	
	// stencil buffer
	// -------------------
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	// shader& model initialize
	// ------------------------------------
	Model ourModel("Data/nanosuit/nanosuit.obj");
	Model dragon("Data/dragon.obj");
	Shader nanoSuitShader("nanoSuitShader.vs", "nanoSuitShader.fs");
	Shader woodFloor("woodFloor.vs", "woodFloor.fs");
	Shader single("shaderSingleColor.vs", "shaderSingleColor.fs");
	Shader doub("shaderSingleColor2.vs", "shaderSingleColor2.fs");
	Shader grassShader("grassShader.vs", "grassShader.fs");

	// wood 
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};



	GLuint VAO;
	GLuint VBO;


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	
	unsigned int stone = loadTexture("Data/stone.png");

	// box

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	// grass initialize

	GLuint grassVAO, grassVBO;
	
	glGenVertexArrays(1, &grassVAO);
	glBindVertexArray(grassVAO);

	glGenBuffers(1, &grassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));


	// box initialize

	GLuint VAO2, VBO2;

	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);

	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	unsigned int grass = loadTexture("Data/grass.png");
	unsigned int container = loadTexture("Data/container2.png");

	// Set viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	
	// matrix coordinates 
	// -----------------------------------

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	
	glm::vec3 arr[] =
	{
		glm::vec3(1.0f,0.0f,1.0f),
		glm::vec3(3.0f, 0.0f, 4.0f)
	};


	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{


		GLfloat currentFrame = glfwGetTime();

		deltaTime = lastFrame - currentFrame;
		lastFrame = currentFrame;

		glm::mat4 view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
		projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 500.0f);

		// Input
		processinput(window);

		glGetString(GL_VENDOR);
		// rendering

		glClearColor(0.0f,0.0f,0.5f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


		/*// nanosuit
		nanoSuitShader.use();

	    nanoSuitShader.setMat4("projection", projection);
		nanoSuitShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.50f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		nanoSuitShader.setMat4("model", model);
		nanoSuitShader.setVec3("lightPos", lightPos);
		nanoSuitShader.setVec3("cameraPos", cameraPos);
		
		n.Draw(nanoSuitShader);
		*/

		// Floor

		glStencilMask(0x00);

		woodFloor.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, stone);



		woodFloor.setMat4("view", view);
		woodFloor.setMat4("projection", projection);
		
		woodFloor.setFloat("material.texture_diffuse1", 0);
		woodFloor.setVec3("lightPos", lightPos);
		woodFloor.setVec3("cameraPos", cameraPos);
		woodFloor.setBool("blinn", blinn);
		woodFloor.setMat4("model", glm::mat4(1.0f));


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		

		// grass

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grass);

		grassShader.use();

		grassShader.setMat4("view", view);
		grassShader.setMat4("projection", projection);
		grassShader.setFloat("texture1", 0);

		glBindVertexArray(grassVAO);

		for (GLuint i = 0; i < vegetation.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, vegetation[i]);
			grassShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		
		
		

		// Container
		
		doub.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container);

		doub.setMat4("view", view);
		doub.setMat4("projection", projection);
		doub.setFloat("diffuseMap", 0);

		single.use();
		single.setMat4("view", view);
		single.setMat4("projection", projection);

		for (GLuint i = 0; i < 2; i++)
		{
			
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			model = glm::mat4(1.0f);
			model = glm::translate(model, arr[i]);

			doub.use();
			doub.setMat4("model", model);

			glBindVertexArray(VAO2);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			model = glm::scale(model, glm::vec3(1.1f));
			glBindVertexArray(VAO2);
			single.use();
			single.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
;
			glEnable(GL_DEPTH_TEST);
			glStencilMask(0xFF);
			
		}
	
		glBindVertexArray(0);
		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();  

		glClear(GL_STENCIL_BUFFER_BIT);
	}
	
	glfwTerminate();
	
	return 0;
}

void frameBuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processinput(GLFWwindow* window)
{

	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = false;
	}
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;

	glGenTextures(1, &textureID);

	int width, height, nrComponents;  
	
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (format == GL_RGBA)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else if (format == GL_RGB)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
