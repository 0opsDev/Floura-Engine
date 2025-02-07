#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
    if (vertices.empty()) {
        //throw std::runtime_error("Vertices, indices, or textures are empty");
		std::cout << "mesh.cpp Vertices are empty" << std::endl;
    }
    if(indices.empty()){
        std::cout << "mesh.cpp indices are empty" << std::endl;
    }
    if(textures.empty()) {
        //problem is within texture or fileclass
        //looking like the file
        //model.cpp knows the location of the texture
        std::cout << "mesh.cpp textures are empty" << std::endl;
    }

    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

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

void Mesh::Draw
(
    Shader& shader,
    Camera& camera,
    glm::mat4 matrix,
    glm::vec3 translation,
    glm::quat rotation,
    glm::vec3 scale
)
{
    shader.Activate();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string num;
        std::string type = textures[i].type;
        if (type == "diffuse")
        {
            num = std::to_string(numDiffuse++);
			//std::cout << "mesh.cpp - Diffuse texture: " << num << std::endl;
        }
        else if (type == "specular") {
            num = std::to_string(numSpecular++);
			//std::cout << "mesh.cpp - Specular texture: " << num << std::endl;
        }
        textures[i].texUnit(shader, (type + num).c_str(), i);
        textures[i].Bind();
    }

    // Camera Matrix
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");

    // Initialize matrix

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);
    
    // Transform matrix to their correct form
    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    // Push matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

    // Draw the mesh to the buffer
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
