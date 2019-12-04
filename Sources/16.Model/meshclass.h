#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "shader_s.h"

#include <glad/glad.h> 
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

using namespace std;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

struct Texture
{
	GLuint id;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	void Draw(Shader shader);
private:

	GLuint VAO, VBO, EBO;
	void SetupMesh();


};