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
// temp, i wanna add BVH collisions
glm::vec3 closestPointOnTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 ap = p - a;

    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);

    if (d1 <= 0.0f && d2 <= 0.0f) return a;

    glm::vec3 bp = p - b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3) return b;

    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
        float v = d1 / (d1 - d3);
        return a + v * ab;
    }

    glm::vec3 cp = p - c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c;

    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return a + w * ac;
    }

    float va = d3 * d6 - d5 * d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return b + w * (c - b);
    }

    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;
    return a + ab * v + ac * w;
}

bool Mesh::collide(glm::vec3 victimXYZ, glm::vec3 victimScale, std::string collidertype) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, rotation.x, glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, rotation.y, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, rotation.z, glm::vec3(0, 0, 1));
    transform = glm::scale(transform, scale);

    float epsilon = 0.01f;
    float maxCollisionDistance = 2.0f;

    for (size_t i = 0; i < indices.size(); i += 3) {
        const Vertex& v0 = vertices[indices[i]];
        const Vertex& v1 = vertices[indices[i + 1]];
        const Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 worldV0 = glm::vec3(transform * glm::vec4(v0.position, 1.0f));
        glm::vec3 worldV1 = glm::vec3(transform * glm::vec4(v1.position, 1.0f));
        glm::vec3 worldV2 = glm::vec3(transform * glm::vec4(v2.position, 1.0f));

        glm::vec3 triMin = glm::min(glm::min(worldV0, worldV1), worldV2);
        glm::vec3 triMax = glm::max(glm::max(worldV0, worldV1), worldV2);

        if (collidertype == "box") {
            glm::vec3 victimMin = victimXYZ - (victimScale * 0.5f);
            glm::vec3 victimMax = victimXYZ + (victimScale * 0.5f);

            bool overlap =
                victimMin.x <= triMax.x && victimMax.x >= triMin.x &&
                victimMin.y <= triMax.y && victimMax.y >= triMin.y &&
                victimMin.z <= triMax.z && victimMax.z >= triMin.z;

            if (!overlap) continue;

            glm::vec3 closest = closestPointOnTriangle(victimXYZ, worldV0, worldV1, worldV2);
            float dist = glm::length(victimXYZ - closest);
            if (dist > maxCollisionDistance) continue;

            if (closest.x >= victimMin.x && closest.x <= victimMax.x &&
                closest.y >= victimMin.y && closest.y <= victimMax.y &&
                closest.z >= victimMin.z && closest.z <= victimMax.z) {

                glm::vec3 dir = victimXYZ - closest;
                if (glm::length(dir) < 0.0001f) dir = glm::vec3(0, 1, 0);
                else dir = glm::normalize(dir);

                float maxHalfScale = glm::compMax(victimScale) * 0.5f;
                lastHit = closest + dir * (maxHalfScale + epsilon);
                return true;
            }
        }

        else if (collidertype == "sphere") {
            float radius = glm::compMax(victimScale) * 0.5f;

            bool overlap =
                victimXYZ.x + radius >= triMin.x && victimXYZ.x - radius <= triMax.x &&
                victimXYZ.y + radius >= triMin.y && victimXYZ.y - radius <= triMax.y &&
                victimXYZ.z + radius >= triMin.z && victimXYZ.z - radius <= triMax.z;

            if (!overlap) continue;

            glm::vec3 closest = closestPointOnTriangle(victimXYZ, worldV0, worldV1, worldV2);
            float dist = glm::length(closest - victimXYZ);

            if (dist > radius || dist > maxCollisionDistance) continue;

            glm::vec3 dir = victimXYZ - closest;
            if (glm::length(dir) < 0.0001f) dir = glm::vec3(0, 1, 0);
            else dir = glm::normalize(dir);

            lastHit = closest + dir * (radius + epsilon);
            return true;
        }

        else if (collidertype == "capsule") {
            float radius = victimScale.x * 0.5f;
            float height = victimScale.y;

            glm::vec3 flatBottomOffset = glm::vec3(0, height * 0.5f - radius, 0);
            glm::vec3 capsuleTop = victimXYZ + flatBottomOffset;
            glm::vec3 capsuleBottom = victimXYZ - flatBottomOffset;


            glm::vec3 capsuleStart = capsuleBottom + glm::vec3(0.0f, radius * 0.5f, 0.0f);
            glm::vec3 capsuleEnd = capsuleTop;

            glm::vec3 capsuleMin = glm::min(capsuleStart, capsuleEnd) - glm::vec3(radius);
            glm::vec3 capsuleMax = glm::max(capsuleStart, capsuleEnd) + glm::vec3(radius);

            bool overlap =
                capsuleMin.x <= triMax.x && capsuleMax.x >= triMin.x &&
                capsuleMin.y <= triMax.y && capsuleMax.y >= triMin.y &&
                capsuleMin.z <= triMax.z && capsuleMax.z >= triMin.z;

            if (!overlap) continue;

            glm::vec3 triClosest = closestPointOnTriangle(victimXYZ, worldV0, worldV1, worldV2);

            glm::vec3 segDir = capsuleEnd - capsuleStart;
            float segLen = glm::length(segDir);
            glm::vec3 segN = segLen > 0.0f ? (segDir / segLen) : glm::vec3(0, 1, 0);
            glm::vec3 ptToStart = triClosest - capsuleStart;
            float t = glm::clamp(glm::dot(ptToStart, segN), 0.0f, segLen);
            glm::vec3 closestOnCapsule = capsuleStart + segN * t;

            float dist = glm::length(triClosest - closestOnCapsule);
            if (dist > radius || dist > maxCollisionDistance) continue;

            glm::vec3 dir = closestOnCapsule - triClosest;
            if (glm::length(dir) < 0.0001f) dir = glm::vec3(0, 1, 0);
            else dir = glm::normalize(dir);

            lastHit = triClosest + dir * (radius + epsilon);
            return true;
        }
    }

    return false;
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

void Mesh::Delete() {
    VAO.Delete();

    for (size_t i = 0; i < textures.size(); i++)
    {textures[i].Delete();}
    textures.clear();

    vertices.clear();
    indices.clear();
}