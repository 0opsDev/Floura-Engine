#include "line.h"
#include <Core/Render.h>

Line3D::Line3D(glm::vec3 pos1, glm::vec3 pos2)
{
    vertices = { {pos1}, {pos2} };
    indices = { 0, 1 };

    VAO.Bind();
    VBO VBO(vertices);
    EBO EBO(indices);

    const size_t stride = sizeof(Vertex);
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, position));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();

    Line3D::translate(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
}

void Line3D::updateVBO(glm::vec3 pos1, glm::vec3 pos2)
{
    vertices = { {pos1}, {pos2} };
    indices = { 0, 1 };

    VAO.Bind();
    VBO VBO(vertices);
    EBO EBO(indices);

    const size_t stride = sizeof(Vertex);
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, position));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

Line3D::~Line3D() // Delete
{
	VAO.Delete();
	vertices.clear();
	indices.clear();
}
void Line3D::translate(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    glm::mat4 Trans = glm::mat4(1.0f);
    glm::mat4 Rot = glm::mat4(1.0f);
    glm::mat4 Sca = glm::mat4(1.0f);

    Trans = glm::translate(Trans, position);

    Rot = glm::rotate(Rot, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    Rot = glm::rotate(Rot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    Rot = glm::rotate(Rot, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    Sca = glm::scale(Sca, scale);

    lineMatrix = Trans * Rot * Sca;
}

void Line3D::draw(glm::vec3 colour)
{
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::main->FBO);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    RenderClass::LineShader.Activate();
    VAO.Bind();

    // colour
    RenderClass::LineShader.setFloat3("aColour", colour);
    // Push model matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(RenderClass::LineShader.ID, "lineMatrix"), 1, GL_FALSE, glm::value_ptr(lineMatrix));
    // Camera Matrix
    glUniform3f(glGetUniformLocation(RenderClass::LineShader.ID, "camPos"), Camera::Position.x, Camera::Position.y, Camera::Position.z);
    Camera::Matrix(RenderClass::LineShader, "camMatrix");
    // Draw the mesh
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
