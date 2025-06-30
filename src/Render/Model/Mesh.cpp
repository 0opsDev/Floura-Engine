#include "mesh.h"
#include "utils/init.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
    if (vertices.empty()) {
        //throw std::runtime_error("Vertices, indices, or textures are empty");
        if (init::LogALL || init::LogModel) std::cout << "mesh.cpp Vertices are empty" << std::endl;
    }
    if (indices.empty()) {
        if (init::LogALL || init::LogModel) std::cout << "mesh.cpp indices are empty" << std::endl;
    }
    if (textures.empty()) {
        //problem is within texture or fileclass
        //looking like the file
        //model.cpp knows the location of the texture
        if (init::LogALL || init::LogModel) std::cout << "mesh.cpp textures are empty" << std::endl;
    }

    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;
    //std::cout << "texture size for mesh: " << textures.size() << std::endl;
    //for (size_t i = 0; i < textures.size(); i++)
    //{
    //    std::cout << "Type: " << textures[i].type << std::endl;
    //    std::cout << "unit: " << textures[i].unit << std::endl;
    //    std::cout << "ID: " << textures[i].ID << std::endl;
    //}



    VAO.Bind();

    VBO VBO(vertices);

    EBO EBO(indices);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
    VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw(Shader& shader, glm::mat4 modelMatrix)
{
    shader.Activate();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
	unsigned int numNormal = 0;
    //std::cout << " mesh render start" << std::endl;
    //if (textures.size() > 3) {
    //    std::cout << textures.size() << " tex" << std::endl;
    //}
    //indmesh
    // std::cout << (indmesh) << " indmesh" << std::endl;
   
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string num;
        std::string type = textures[i].type;
        if (type == "diffuse") {
            num = std::to_string(numDiffuse++);
        }
        else if (type == "specular") {
            num = std::to_string(numSpecular++);
        }
		else if (type == "normal") {
			num = std::to_string(numNormal++);
		}
        //std::cout << (type + num).c_str() << std::endl;
        textures[i].texUnit(shader, (type + "0").c_str(), i);
        textures[i].Bind();
        //std::cout << "Binding texture " << i << ": Type " << textures[i].type << ", Uniform: " << type << ", Texture Unit: " << i << ", GL ID: " << textures[i].ID << std::endl;
       
    }

    // Camera Matrix
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), Camera::Position.x, Camera::Position.y, Camera::Position.z);
    Camera::Matrix(shader, "camMatrix");

    // Push model matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw the mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}