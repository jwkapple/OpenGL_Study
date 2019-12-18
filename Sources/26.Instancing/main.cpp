
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include "main.h"
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

unsigned int loadCubemap(vector<std::string> faces);

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

	// depth buffer && stencil buffer
	// -------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// shader& model initialize
	// ------------------------------------
	Model ourModel("Data/nanosuit/nanosuit.obj");
	Shader floorShader("floorShader.vs", "floorShader.fs");
	Shader boxShader("boxShader.vs", "boxShader.fs");
	Shader glowShader("glowShader.vs", "glowShader.fs");
	Shader skyboxShader("skyboxShader.vs", "skyboxShader.fs");
	Shader instanceShader("instanceShader.vs", "instanceShader.fs");


	vector<std::string> faces =
	{
		"Data/skybox/right.tga",
		"Data/skybox/left.tga",
		"Data/skybox/top.tga",
		"Data/skybox/bottom.tga",
		"Data/skybox/front.tga",
		"Data/skybox/back.tga",
	};

	glm::vec3 arr[] =
	{
		glm::vec3(0.0f,0.0f,-1.0f),
		glm::vec3(2.0f, 0.0f, -4.0f)
	};

	glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	// setting uniform buffer

	GLuint boxUniformBlock = glGetUniformBlockIndex(boxShader.ID, "Matrices");
	GLuint floorUniformBlock = glGetUniformBlockIndex(floorShader.ID, "Matrices");
	GLuint glowUniformBlock = glGetUniformBlockIndex(glowShader.ID, "Matrices");
	GLuint skyboxUniformBlock = glGetUniformBlockIndex(skyboxShader.ID, "Matrices");

	glUniformBlockBinding(boxShader.ID, boxUniformBlock, 0);
	glUniformBlockBinding(floorShader.ID, floorUniformBlock, 0);
	glUniformBlockBinding(glowShader.ID, glowUniformBlock, 0);
	glUniformBlockBinding(skyboxShader.ID, skyboxUniformBlock, 0);

	GLuint uboMatrcies;

	glGenBuffers(1, &uboMatrcies);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrcies, 0, 2 * sizeof(glm::mat4));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// floor

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


	// box initialize

	GLuint boxVAO, boxVBO;

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);

	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	unsigned int windowTexture = loadTexture("Data/window.png");
	unsigned int container = loadTexture("Data/container2.png");
	

	// skybox

	GLuint skybox = loadCubemap(faces);

	GLuint skyboxVAO, skyboxVBO;

	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	// quad 

	GLuint quadVAO, quadVBO, instanceVBO;

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(2);

	glVertexAttribDivisor(2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	

	glBindVertexArray(0);




	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); // set viewport


	// matrix coordinates 
	// -----------------------------------

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 500.0f);

	
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);


	// loop
	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{

		GLfloat currentFrame = glfwGetTime();

		deltaTime = lastFrame - currentFrame;
		lastFrame = currentFrame;

		glm::mat4 view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);
		
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Input
		processinput(window); 

		glGetString(GL_VENDOR);


		// rendering

		glClearColor(1.0f,1.0f,1.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		/*
		// Floor

		glStencilMask(0x00);

		floorShader.use();

		glBindTexture(GL_TEXTURE_2D, stone);
		
		floorShader.setFloat("material.texture_diffuse1", 0);
		floorShader.setVec3("lightPos", lightPos);
		floorShader.setVec3("cameraPos", cameraPos);
		floorShader.setBool("blinn", blinn);
		floorShader.setMat4("model", glm::mat4(1.0f));

		glBindVertexArray(VAO);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Container
			
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container);


		glowShader.use();
		glowShader.setFloat("diffuseMap", 0);

		boxShader.use();

		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		for (GLuint i = 0; i < 2; i++)
		{
			
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			model = glm::mat4(1.0f);
			model = glm::translate(model, arr[i]);

			glowShader.use();
			glowShader.setMat4("model", model);

			glBindVertexArray(boxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			
			model = glm::scale(model, glm::vec3(1.1f));
			glBindVertexArray(boxVAO);
			boxShader.use();
			boxShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
;
			glStencilMask(0xFF);
			
		}
		
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glClear(GL_STENCIL_BUFFER_BIT);



		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
		
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);

		skyboxShader.setInt("skybox", 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);
	
		glDepthFunc(GL_LESS);

		*/

		instanceShader.use();

		for (GLuint i = 0; i < 100; i++)
		{
			stringstream ss;
			string index;
			ss << i;
			index = ss.str();
			instanceShader.setVec2(("offsets[" + index + "]").c_str(), translations[i]);
		}

		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);


		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();  

		
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

unsigned int loadCubemap(vector<std::string> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (GLuint i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{

			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height,
				0, format, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		
	}
	return textureID;
}
