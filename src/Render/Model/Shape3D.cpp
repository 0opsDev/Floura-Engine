#include "Shape3D.h"


void Shape3D::updatePosition(glm::vec3 Position)
{
	Shape3D::gPosition = Position;
}

void Shape3D::updateRotation(glm::vec3 Rotation)
{
	Shape3D::gRotation = Rotation;
}

void Shape3D::updateScale(glm::vec3 Scale)
{
	Shape3D::gScale = Scale;
}

void Shape3D::create() // needs to take in shape type
{
	//shapeMesh.create();
}

void Shape3D::loadModel(std::string path)
{

}

void Shape3D::draw(Shader& shader)
{
	//shapeMesh.draw(shader);
}

void Shape3D::Delete()
{
	shapeMesh.Delete();
}