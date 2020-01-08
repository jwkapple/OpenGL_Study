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
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

// timing
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// cursor position
GLfloat lastX = 400;
GLfloat lastY = 300;


int main()
{
	// initialize window(glfw & glad)
	

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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// shader& model initialize
	
	Shader floorShader("floorShader.vs", "floorShader.fs");
	Shader offScreenShader("offScreenShader.vs", "offScreenShader.fs");
	Shader boxShader("boxShader.vs", "boxShader.fs");
	Shader glowShader("glowShader.vs", "glowShader.fs");
	Shader shadowShader("shadowShader.vs", "shadowShader.fs");
	Shader floorShadowShader("floorShadowShader.vs", "floorShadowShader.fs");
	Shader lightShader("lightShader.vs", "lightShader.fs");



	glm::vec3 arr[] =
	{
		glm::vec3(2.0f,0.0f,3.0f),
		glm::vec3(0.0f, 1.5f,1.0f)
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

	
#pragma region uniform buffer

	GLuint boxUniformBlock = glGetUniformBlockIndex(boxShader.ID, "Matrices");
	GLuint floorUniformBlock = glGetUniformBlockIndex(floorShader.ID, "Matrices");
	GLuint glowUniformBlock = glGetUniformBlockIndex(glowShader.ID, "Matrices");
	GLuint floorShadowUniformBlock = glGetUniformBlockIndex(floorShadowShader.ID, "Matrices");
	GLuint lightUniformBlock = glGetUniformBlockIndex(lightShader.ID, "Matrices");

	glUniformBlockBinding(boxShader.ID, boxUniformBlock, 0);
	glUniformBlockBinding(floorShader.ID, floorUniformBlock, 0);
	glUniformBlockBinding(glowShader.ID, glowUniformBlock, 0);
	glUniformBlockBinding(floorShadowShader.ID, floorShadowUniformBlock, 0);
	glUniformBlockBinding(lightShader.ID, lightUniformBlock, 0);


	GLuint uboMatrcies;

	glGenBuffers(1, &uboMatrcies);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrcies, 0, 2 * sizeof(glm::mat4));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	GLuint frameBufferVBO, frameBufferVAO;
	glGenVertexArrays(1, &frameBufferVAO);
	glBindVertexArray(frameBufferVAO);

	glGenBuffers(1, &frameBufferVBO);
	glBindBuffer(GL_ARRAY_BUFFER, frameBufferVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

#pragma endregion
	
#pragma region framebuffer

	GLuint frame;
	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);

	GLuint frameTexture;
	glGenTextures(1, &frameTexture);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameTexture, 0);

	GLuint renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	unsigned int intermediateFBO;
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion
	
#pragma region depthMap

	GLuint depthFBO;
	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// light space transform 

	float near_plane = 1.0f, far_plane = 7.5f;

	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(lightPos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;

#pragma endregion

#pragma region floor

	GLuint brick = loadTexture("Data/wood.png");
	
	GLuint floorVAO, floorVBO;

	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
#pragma endregion

#pragma region box


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

#pragma endregion	


	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); // set viewport

	// matrix coordinates 
	
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 floorModel = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 500.0f);


	// loop
	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{
		
		GLdouble currentFrame = glfwGetTime();

		deltaTime = lastFrame - currentFrame;
		lastFrame = currentFrame;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);


		// shadow uniform
		float near_plane = 1.0f, far_plane = 10.5f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(lightProjection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(lightView));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Input
		processinput(window);
		glGetString(GL_VENDOR);

		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		

#pragma region shadow render

		glEnable(GL_DEPTH_TEST);

		// Floor
		
		glStencilMask(0x00);


		floorShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brick);
		
		floorShader.setVec3("lightPos", lightPos);
		floorShader.setVec3("cameraPos", cameraPos);
		floorShader.setBool("blinn", blinn);
		floorShader.setMat4("model", floorModel);
		floorShader.setFloat("material.texture_diffuse1", 0);
		glBindVertexArray(floorVAO);

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

		model = glm::mat4(1.0f);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glClear(GL_STENCIL_BUFFER_BIT);

#pragma endregion
		
	

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

#pragma region usual render

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Floor

		glStencilMask(0x00);

		floorShadowShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brick);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		floorShadowShader.setVec3("lightPos", lightPos);
		floorShadowShader.setVec3("cameraPos", cameraPos);
		floorShadowShader.setBool("blinn", blinn);
		floorShadowShader.setMat4("model", floorModel);
		floorShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		floorShadowShader.setInt("diffuseTexture", 0);
		floorShadowShader.setInt("shadowMap", 1);
		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Container

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

		model = glm::mat4(1.0f);

		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glClear(GL_STENCIL_BUFFER_BIT);

		lightShader.use();
		model = glm::scale(model, glm::vec3(0.5f));
		model = glm::translate(model, lightPos);
		lightShader.setMat4("model", model);
		lightShader.setVec3("color", glm::vec3(0.9f,0.8f,0.8f));

		glBindVertexArray(boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(0.1f));
		lightShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, frame);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#pragma endregion

		

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