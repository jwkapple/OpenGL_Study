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



#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

bool blinn = false;
bool bRotate = false;
bool blinnKeyPressed = false;

unsigned int loadCubemap(vector<std::string> faces);

// camera
glm::vec3 cameraPos = glm::vec3(0.0f,   0.0f, -2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos(0.8f, -1.7f, 6.0f);
glm::vec3 lightVec[2] = { lightPos, glm::vec3(0.0f) };

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set OpenGL profile to Core (For modern OpenGL use)

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Tutorial", NULL, NULL);
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
	
	Shader depthShader("depthShader.vs","depthShader.gs", "depthShader.fs");
	Shader offScreenShader("offScreenShader.vs", "offScreenShader.fs");
	Shader shadowShader("shadowShader.vs", "shadowShader.fs");
	Shader sceneShader("sceneShader.vs", "sceneShader.fs");
	Shader lightShader("lightShader.vs", "lightShader.fs");
	Shader lineShader("lineShader.vs", "lineShader.gs", "lineShader.fs");
	Shader hdrShader("hdrShader.vs", "hdrShader.fs");
	Shader boxShader("boxShader.vs", "boxShader.fs");

	glm::vec3 arr[] =
	{
		glm::vec3(2.0f,0.0f,3.0f),
		glm::vec3(1.3f, 1.5f,1.6f)
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

	GLuint floorShadowUniformBlock = glGetUniformBlockIndex(sceneShader.ID, "Matrices");
	GLuint lightUniformBlock = glGetUniformBlockIndex(lightShader.ID, "Matrices");
	GLuint lineUniformBlock = glGetUniformBlockIndex(lineShader.ID, "Matrices");
	GLuint hdrUniformBlock = glGetUniformBlockIndex(hdrShader.ID, "Matrices");
	GLuint boxUniformBlock = glGetUniformBlockIndex(boxShader.ID, "Matrices");

	glUniformBlockBinding(sceneShader.ID, floorShadowUniformBlock, 0);
	glUniformBlockBinding(lightShader.ID, lightUniformBlock, 0);
	glUniformBlockBinding(lineShader.ID, lineUniformBlock, 0);
	glUniformBlockBinding(hdrShader.ID, hdrUniformBlock, 0);
	glUniformBlockBinding(boxShader.ID, boxUniformBlock, 0);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), &QuadVertices, GL_STATIC_DRAW);

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
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameTexture, 0);

	GLuint renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion
	
#pragma region HDR framebuffer

	GLuint HdrFBO;

	glGenFramebuffers(1, &HdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);


	// initialize HDR Frame's texture 
	GLuint HdrTexture;
	glGenTextures(1, &HdrTexture);
	glBindTexture(GL_TEXTURE_2D, HdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HdrTexture, 0);

	// initialize HDR Frame's renderbuffer
	GLuint HdrRenderbuffer;
	glGenRenderbuffers(1, &HdrRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, HdrRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, HdrRenderbuffer);

	// check if done right
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: HDR Framebuffer is not complete!" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // detach HDR Framebuffer


#pragma endregion

#pragma region depthMap

	GLuint depthFBO;
	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	GLuint depthCubeMap;
	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

	for (GLuint i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// light space transform 

	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	GLfloat near = 1.0f, far = 100.0f;

	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

	std::vector<glm::mat4> shadowTransforms;

	shadowTransforms.push_back(shadowProj *
		glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj *
		glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

#pragma endregion

#pragma region floor

	GLuint brick = loadTexture("Data/wood.png");
	
	GLuint floorVAO, floorVBO;

	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(PlaneVertices), &PlaneVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
#pragma endregion

#pragma region box

	GLuint container = loadTexture("Data/container2.png");

	GLuint boxVAO, boxVBO;

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);

	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(HdrVertices), &HdrVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	
	GLuint glowBoxVAO, glowBoxVBO;

	glGenVertexArrays(1, &glowBoxVAO);
	glBindVertexArray(glowBoxVAO);

	glGenBuffers(1, &glowBoxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, glowBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), &CubeVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);



#pragma endregion	

#pragma region temp line

	GLuint lineVAO, lineVBO;

	glGenVertexArrays(1, &lineVAO);
	glBindVertexArray(lineVAO);

	glGenBuffers(1, &lineVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVec), &lightVec, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glBindVertexArray(0);

#pragma endregion

#pragma region Matrix Coordinates

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 500.0f);


	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f)); // back light
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));
	// colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

#pragma endregion
	 
	// loop

	glfwSetFramebufferSizeCallback(window, frameBuffer_size_callback);

	while (!glfwWindowShouldClose(window))
	{
		
		GLdouble currentFrame = glfwGetTime();

		deltaTime = (GLfloat)(lastFrame - currentFrame);
		lastFrame = (GLfloat)currentFrame;

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrcies);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Input
		processinput(window);

		glGetString(GL_VENDOR);

		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

		glClear(GL_DEPTH_BUFFER_BIT);

#pragma region shadow render

		glEnable(GL_DEPTH_TEST);

		// Floor
		

		depthShader.use();

		for (GLuint i = 0; i < 6; i++)
		{
			depthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
		
		depthShader.setVec3("lightPos", lightPos);
		depthShader.setMat4("model", model);
		depthShader.setFloat("far_plane", far);

		/*
		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		*/

		// Container

			model = glm::mat4(1.0f);

			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0)); 
			model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
			depthShader.setMat4("model", model);
			glBindVertexArray(boxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);


		model = glm::mat4(1.0f);

#pragma endregion
		
#pragma region HDR render

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, HdrFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		sceneShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brick);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

		sceneShader.setVec3("cameraPos", cameraPos);
		sceneShader.setMat4("model", model);
		sceneShader.setInt("diffuseTexture", 0);
		sceneShader.setInt("depthMap", 1);
		sceneShader.setFloat("far_plane", far);

		cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << endl;

		for (GLuint i = 0; i < lightPositions.size(); i++)
		{
			sceneShader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			sceneShader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}

		/*
		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		*/

		//Container

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, brick);

		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));

		sceneShader.setMat4("model", model);
		glBindVertexArray(boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		boxShader.use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, container);

		model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));

		boxShader.setVec3("lightPos", lightPos);
		boxShader.setVec3("cameraPos", cameraPos);
		boxShader.setMat4("model", model);
		boxShader.setVec3("glowColor", glm::vec3(1.0f, 0.0f, 0.0f));
		boxShader.setFloat("delta", (GLdouble)glm::cos((GLdouble)glfwGetTime()) * 1.75f);
		boxShader.setFloat("diffuse", 0);

		glBindVertexArray(glowBoxVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightShader.use();

		lightShader.setVec3("color", glm::vec3(1.0f));

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 49.5f));

		lightShader.setMat4("model", model);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.3f));

		lightShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

#pragma endregion

#pragma region usual render

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.5f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, HdrTexture);

		hdrShader.use();

		hdrShader.setInt("diffuse", 0);
		hdrShader.setFloat("exposure", 0.5f);
		
		glBindVertexArray(frameBufferVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

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

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && bRotate)
	{
		bRotate = !bRotate;
		cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !bRotate)
	{
		bRotate = !bRotate;
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
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
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrComponents == 1)
			internalFormat = dataFormat = GL_RED;
		else if (nrComponents == 3)
		{
			internalFormat = GL_SRGB;
			dataFormat = GL_RGB;
		}
		else if (nrComponents == 4)
		{
			internalFormat = GL_RGBA;
			dataFormat = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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