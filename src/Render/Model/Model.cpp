#include"Model.h"
#include"Core/Main.h"
#include "utils/init.h"

int totalVert = 0;

Model::Model(const char* file)
{
	// Make a JSON object
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	// Get the binary data
	Model::file = file;
	data = getData();

	// Traverse all root nodes
	for (unsigned int i = 0; i < JSON["scenes"][0]["nodes"].size(); i++)
	{
		traverseNode(JSON["scenes"][0]["nodes"][i]);
	}
}

void Model::Draw(Shader& shader, glm::vec3 translation, glm::vec4 rotation, glm::vec3 scale)
{
	// Convert rotation data into a quaternion
	glm::quat newrotation = glm::angleAxis(glm::radians(rotation.x), glm::vec3(rotation.y, rotation.z, rotation.w));

	// Convert quaternion to rotation matrix
	glm::mat4 rotationMatrix = glm::toMat4(newrotation);

	// Construct the model matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translation) *
		rotationMatrix *
		glm::scale(glm::mat4(1.0f), scale);

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		glm::mat4 finalMatrix = modelMatrix * matricesMeshes[i]; // Local mesh transform applied
		meshes[i].Draw(shader, finalMatrix);
	}
}

int Model::getVertexCount() const
{
	int size = 0;
	for (size_t i = 0; i < meshes.size(); i++) {
		for (size_t j = 0; j < meshes[i].vertices.size(); j++) {
			size++;
		}
	}
	return size;
}
int Model::getIndiceCount() const
{
	int x = 0;
	for (size_t i = 0; i < meshes.size(); i++) {
		for (size_t j = 0; j < meshes[i].indices.size(); j++) {
			x++;
		}
	}
	return x;
}

glm::vec3 closestPointOnTriangle(glm::vec3 P, glm::vec3 A, glm::vec3 B, glm::vec3 C) {
	glm::vec3 ab = B - A;
	glm::vec3 ac = C - A;
	glm::vec3 ap = P - A;

	float d1 = glm::dot(ab, ap);
	float d2 = glm::dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return A; // Closest to A

	glm::vec3 bp = P - B;
	float d3 = glm::dot(ab, bp);
	float d4 = glm::dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) return B; // Closest to B

	glm::vec3 cp = P - C;
	float d5 = glm::dot(ab, cp);
	float d6 = glm::dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) return C; // Closest to C

	// **Compute projection onto the triangle plane**
	glm::vec3 normal = glm::normalize(glm::cross(ab, ac));
	float distance = glm::dot(normal, P - A);
	glm::vec3 projectedPoint = P - normal * distance;

	// **Check barycentric coordinates to determine if inside the triangle**
	glm::vec3 edge0 = B - A;
	glm::vec3 edge1 = C - A;
	glm::vec3 vp = projectedPoint - A;

	float d00 = glm::dot(edge0, edge0);
	float d01 = glm::dot(edge0, edge1);
	float d11 = glm::dot(edge1, edge1);
	float d20 = glm::dot(vp, edge0);
	float d21 = glm::dot(vp, edge1);

	float denom = d00 * d11 - d01 * d01;
	float u = (d11 * d20 - d01 * d21) / denom;
	float v = (d00 * d21 - d01 * d20) / denom;

	if (u >= -0.05f && v >= -0.05f && (u + v) <= 1.05f) { // Tolerance for floating point error
		return projectedPoint; // Closest point inside triangle
	}

	return A + u * ab + v * ac; // Closest edge point
}

bool triangleSphereIntersection(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 sphereCenter, float radius) {
	glm::vec3 closestPoint = closestPointOnTriangle(sphereCenter, v0, v1, v2);
	return glm::distance(sphereCenter, closestPoint) <= radius;
}

bool Model::checkCollide(glm::vec3 point, glm::vec3 globalTranslation, glm::quat globalRotation, glm::vec3 globalScale, float checkRadius) {
	bool collisionDetected = false;

	for (size_t i = 0; i < meshes.size(); i++) {
		glm::mat4 nodeTransformMatrix = glm::translate(glm::mat4(1.0f), translationsMeshes[i]) *
			glm::mat4_cast(rotationsMeshes[i]) *
			glm::scale(glm::mat4(1.0f), scalesMeshes[i]);

		glm::mat4 fullTransformMatrix = glm::translate(glm::mat4(1.0f), globalTranslation) *
			glm::mat4_cast(globalRotation) *
			glm::scale(glm::mat4(1.0f), globalScale) *
			nodeTransformMatrix;



		for (size_t j = 0; j < meshes[i].indices.size(); j += 3) {
			// **Transform polygon vertices**
			glm::vec3 v0 = glm::vec3(fullTransformMatrix * glm::vec4(meshes[i].vertices[meshes[i].indices[j]].position, 1.0f));
			glm::vec3 v1 = glm::vec3(fullTransformMatrix * glm::vec4(meshes[i].vertices[meshes[i].indices[j + 1]].position, 1.0f));
			glm::vec3 v2 = glm::vec3(fullTransformMatrix * glm::vec4(meshes[i].vertices[meshes[i].indices[j + 2]].position, 1.0f));

			// **Compute triangle center and distance from player**
			glm::vec3 triangleCenter = (v0 + v1 + v2) / 3.0f;
			float distanceToFeet = glm::distance(point, triangleCenter);

			// **Early exit: Skip triangles outside the radius**
			if (distanceToFeet > 5.0f) continue;

			if (triangleSphereIntersection(v0, v1, v2, point, checkRadius)) {
				collisionDetected = true;
				lastCollisionPoint = point;
				lastCollisionFace[0] = v0;
				lastCollisionFace[1] = v1;
				lastCollisionFace[2] = v2;
			}
		}
	}
	return collisionDetected;
}

void Model::loadMesh(unsigned int indMesh)
{
	// Get all accessor indices
	unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

	// Debug output for mesh
	if (init::LogALL || init::LogModel) std::cout << "Loading mesh: " << indMesh << std::endl;

	// Use accessor indices to get all vertices components
	std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
	std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
	std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

	// Combine all the vertex components and also get the indices and textures
	std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
	std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	std::vector<Texture> textures = getTexturesForMesh(indMesh);
	//std::vector<Texture> textures = getTextures();

	// Combine the vertices, indices, and textures into a mesh
	meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	// Current node
	json node = JSON["nodes"][nextNode];

	// Debug output for node
	if (init::LogALL || init::LogModel) std::cout << "Processing node: " << nextNode << std::endl;

	// Get translation if it exists
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}
	// Get quaternion if it exists
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}
	// Get scale if it exists
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Multiply all matrices together
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		if (init::LogALL || init::LogModel) std::cout << "Loading mesh: " << node["mesh"] << std::endl;
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

std::vector<unsigned char> Model::getData()
{
	// Create a place to store the raw text, and get the uri of the .bin file
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	// Store raw text data into bytesText
	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	// Transform the raw text data into bytes and put them in a vector
	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floatVec;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Interpret the type and store it into numPerVert
	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4)
	{
		unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices;
	if (init::LogALL || init::LogModel) std::cout << "\nModel.cpp" << std::endl;
	if (init::LogALL || init::LogModel) std::cout << "getIndices / initialization - model.cpp" << std::endl;


	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];
	totalVert += count;
	if (init::LogALL || init::LogModel) std::cout << totalVert << std::endl;

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i += 4)
		{
			unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i + 1] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i += 2)
		{
			unsigned char bytes[] = { data[i], data[i + 1] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}

	return indices;
}

std::vector<Texture> Model::getTextures()
{

	if (init::LogALL || init::LogModel) std::cout << "getTextures / initialization - model.cpp" << std::endl;
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Go over all images
	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		// uri of current texture
		std::string texPath = JSON["images"][i]["uri"];
		if (init::LogALL || init::LogModel) std::cout << "model.cpp - Texture path: " << texPath << std::endl;

		// Check if the texture has already been loaded
		bool skip = false;
		for (unsigned int j = 0; j < loadedTexName.size(); j++)
		{
			if (loadedTexName[j] == texPath)
			{
				textures.push_back(loadedTex[j]);
				skip = true;
				break;
			}
		}

		// If the texture has been loaded, skip this
		if (!skip)
		{
			if (init::LogALL || init::LogModel) std::cout << "\nModel.cpp" << std::endl;
			// Determine the texture type based on the name
			const char* texType;
			if (texPath.find("baseColor") != std::string::npos)
			{
				texType = "diffuse";
				if (init::LogALL || init::LogModel) std::cout << "model.cpp - Texture type: diffuse" << std::endl;
			}
			else if (texPath.find("metallicRoughness") != std::string::npos)
			{
				texType = "specular";
				if (init::LogALL || init::LogModel) std::cout << "model.cpp - Texture type: specular" << std::endl;
			}

			// Load the texture
			if (init::LogALL || init::LogModel) std::cout << "model.cpp - Loading texture: " << fileDirectory + texPath << std::endl;
			if (init::LogALL || init::LogModel) std::cout << loadedTex.size() << std::endl;
			Texture texture = Texture((fileDirectory + texPath).c_str(), texType, loadedTex.size());
			textures.push_back(texture);
			loadedTex.push_back(texture);
			loadedTexName.push_back(texPath);
		}
	}

	return textures;
}

std::vector<Texture> Model::getTexturesForMesh(unsigned int indMesh)
{
	std::vector<Texture> textures;
	// Check if the mesh has a material
	if (JSON["meshes"][indMesh]["primitives"][0].find("material") != JSON["meshes"][indMesh]["primitives"][0].end())
	{
		unsigned int materialIndex = JSON["meshes"][indMesh]["primitives"][0]["material"];
		json material = JSON["materials"][materialIndex];

		std::string fileStr = std::string(file);
		std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

		// diffuse
		if (material["pbrMetallicRoughness"].find("baseColorTexture") != material["pbrMetallicRoughness"].end())
		{
			unsigned int textureIndex = material["pbrMetallicRoughness"]["baseColorTexture"]["index"];
			if (!JSON["images"][textureIndex]["uri"].is_null())
			{
				std::string texturePath = JSON["images"][textureIndex]["uri"];
				bool skip = false;
				for (unsigned int i = 0; i < loadedTexName.size(); i++)
				{
					if (loadedTexName[i] == texturePath)
					{
						textures.push_back(loadedTex[i]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{
					Texture texture((fileDirectory + texturePath).c_str(), "diffuse", loadedTex.size());
					textures.push_back(texture);
					loadedTex.push_back(texture);
					loadedTexName.push_back(texturePath);
				}
			}
		}

		// specular
		if (material["pbrMetallicRoughness"].find("metallicRoughnessTexture") != material["pbrMetallicRoughness"].end())
		{
			unsigned int textureIndex = material["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
			if (!JSON["images"][textureIndex]["uri"].is_null())
			{
				std::string texturePath = JSON["images"][textureIndex]["uri"];
				bool skip = false;
				for (unsigned int i = 0; i < loadedTexName.size(); i++)
				{
					if (loadedTexName[i] == texturePath)
					{
						textures.push_back(loadedTex[i]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{
					Texture texture((fileDirectory + texturePath).c_str(), "specular", loadedTex.size());
					textures.push_back(texture);
					loadedTex.push_back(texture);
					loadedTexName.push_back(texturePath);
				}
			}
		}
		else
		{
			// fallback
			std::string fallbackPath = "Assets/Dependants/black.png";
			bool skip = false;
			for (unsigned int i = 0; i < loadedTexName.size(); i++)
			{
				if (loadedTexName[i] == fallbackPath)
				{
					textures.push_back(loadedTex[i]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture fallbackTexture(fallbackPath.c_str(), "specular", loadedTex.size());
				textures.push_back(fallbackTexture);
				loadedTex.push_back(fallbackTexture);
				loadedTexName.push_back(fallbackPath);
			}
		}
	}

	return textures;
}


std::vector<Vertex> Model::assembleVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> texUVs
)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			Vertex
			{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
	const unsigned int floatsPerVector = 2;

	std::vector<glm::vec2> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec2(0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVec[i + j];
		}
	}
	return vectors;
}
std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
	const unsigned int floatsPerVector = 3;

	std::vector<glm::vec3> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec3(0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVec[i + j];
		}
	}
	return vectors;
}
std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
	const unsigned int floatsPerVector = 4;

	std::vector<glm::vec4> vectors;
	for (unsigned int i = 0; i < floatVec.size(); i += floatsPerVector)
	{
		vectors.push_back(glm::vec4(0, 0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++)
		{
			vectors.back()[j] = floatVec[i + j];
		}
	}
	return vectors;
}