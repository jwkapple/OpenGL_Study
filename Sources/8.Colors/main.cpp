
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader_s.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void frameBuffer_size_callback(GLFWwindow* window, int width, int height);

void processinput(GLFWwindow* window);

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,0.0f);

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

	glEnable(GL_DEPTH_TEST);


	// build Texture
	// --------------------------------------

	// texture 1
	// --------------------------------------
	/*GLuint texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannel;
	unsigned char* data = stbi_load("Data/container.jpg", &width, &height, &nrChannel, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load Texture" << std::endl;
	}

	stbi_image_free(data);

	*/



	// build and compile our shader program
	// --------------------------------------

	Shader objectShader("vertexShader.vs", "fragmentShader.fs");
	Shader lampShader("lightVertexShader.vs", "lightFragmentShader.fs");

	// Set viewport
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};



	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	// Vertex Array Object



	

	// Vertex Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	


	// object cube object
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// lamp cube object
	GLuint LampVAO;
	glGenVertexArrays(1, &LampVAO);
	glBindVertexArray(LampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// matrix coordinates 
	// -----------------------------------

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	// projection matrix
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, 500.0f);



	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);
	
	glm::mat4 rotation = glm::mat4(1.0f);
	
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();

		deltaTime = lastFrame - currentFrame;
		lastFrame = currentFrame;

		glm::mat4 view = glm::lookAt(cameraPos, cameraFront + cameraPos, cameraUp);

		// Input
		processinput(window);


		// rendering

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
	*/

		rotation = glm::rotate(rotation,(float)(glm::radians(10.0f)* deltaTime), glm::vec3(1.0f, 2.0f, 3.0f));

		glm::mat4 model = glm::mat4(1.0f);

		
		objectShader.use();
		
		objectShader.setMat4("translate", model);
		objectShader.setMat4("model", model);
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("rotation", rotation);
		objectShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
		objectShader.setMat4("view", view);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		
	
		lampShader.use();

		model = glm::translate(model, glm::vec3(1.0f, -0.6f, -1.5f));

		lampShader.setMat4("translate", model);
		lampShader.setMat4("model", model);
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("rotation", rotation);
		lampShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		lampShader.setMat4("view", view);

		glBindVertexArray(LampVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


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
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}
