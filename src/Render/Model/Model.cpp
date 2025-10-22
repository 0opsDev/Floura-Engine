#include"Model.h"
#include "utils/init.h"
#include <utils/logConsole.h>

void Model::init(const char* file) {
	// Make a JSON object
	std::string text = get_file_contents(file);
	JSON = json::parse(text);

	// Get the binary data
	Model::file = file;
	data = getData();
	//std::cout << " node" << std::endl;
	// Traverse all root nodes
	for (unsigned int i = 0; i < JSON["scenes"][0]["nodes"].size(); i++)
	{
		//std::cout << i << " nodes" << std::endl;
		traverseNode(JSON["scenes"][0]["nodes"][i]);
	}
}

void Model::updatePosition(glm::vec3 translation)
{
	Model::translation = translation;
}

void Model::updateRotation(glm::vec3 rotation)
{
	Model::rotation = rotation;
}

void Model::updateScale(glm::vec3 scale)
{
	Model::scale = scale;
}

void Model::calculateTangents(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
	std::vector<glm::vec3> tempTangents(vertices.size(), glm::vec3(0.0f));
	std::vector<glm::vec3> tempBitangents(vertices.size(), glm::vec3(0.0f));

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		GLuint i1 = indices[i + 0];
		GLuint i2 = indices[i + 1];
		GLuint i3 = indices[i + 2];

		const Vertex& v1 = vertices[i1];
		const Vertex& v2 = vertices[i2];
		const Vertex& v3 = vertices[i3];

		glm::vec3 edge1 = v2.position - v1.position;
		glm::vec3 edge2 = v3.position - v1.position;

		glm::vec2 deltaUV1 = v2.texUV - v1.texUV;
		glm::vec2 deltaUV2 = v3.texUV - v1.texUV;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		tempTangents[i1] += tangent;
		tempTangents[i2] += tangent;
		tempTangents[i3] += tangent;

		tempBitangents[i1] += bitangent;
		tempBitangents[i2] += bitangent;
		tempBitangents[i3] += bitangent;
	}

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		glm::vec3& n = vertices[i].normal;
		glm::vec3& t = tempTangents[i];
		glm::vec3& b = tempBitangents[i];

		vertices[i].tangent = glm::normalize(t - n * glm::dot(n, t));

		float handedness = (glm::dot(glm::cross(n, vertices[i].tangent), b) < 0.0f) ? -1.0f : 1.0f;
	}
}

void Model::Draw(Shader& shader)
{

	glm::mat4 globalTrans = glm::mat4(1.0f);
	glm::mat4 globalRot = glm::mat4(1.0f);
	glm::mat4 globalSca = glm::mat4(1.0f);

	globalTrans = glm::translate(globalTrans, translation);

	globalRot = glm::rotate(globalRot, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	globalRot = glm::rotate(globalRot, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	globalRot = glm::rotate(globalRot, glm::radians(rotation.z), glm::vec3(0, 0, 1));


	globalSca = glm::scale(globalSca, scale);

	modelMatrix = globalTrans * globalRot * globalSca;
	//modelMatrix = globalRot * globalSca;

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		glm::mat4 finalMatrix = modelMatrix * matricesMeshes[i]; // Local mesh transform applied
		// pass these into shader too at some point
		//meshes[i].position = translation;
		//meshes[i].rotation = glm::vec3(rotation.x, rotation.y, rotation.z);
		//meshes[i].scale = scale;
		meshes[i].draw(shader, finalMatrix);
	}
}

void Model::Delete() {
	// Delete mesh first then clear all array inside model
	for (size_t i = 0; i < meshes.size(); i++)
	{meshes[i].Delete();
	meshes.erase(meshes.begin() + i);
	}
	meshes.clear();

	for (size_t i = 0; i < textures2.size(); i++)
	{textures2[i].Delete();
	textures2.erase(textures2.begin() + i);}
	textures2.clear();
	
	for (size_t i = 0; i < loadedTex.size(); i++)
	{loadedTex[i].Delete();
	loadedTex.erase(loadedTex.begin() + i);}
	loadedTex.clear();
	
	translationsMeshes.clear();
	rotationsMeshes.clear();
	scalesMeshes.clear();
	matricesMeshes.clear();
	loadedTexName.clear();
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
	calculateTangents(vertices, indices);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);


	//std::cout << indMesh << std::endl;
	std::vector<Texture> textures = getTexturesForMesh(indMesh); // maybe make vector of these
	//textures2.push_back(textures);

	meshes.push_back(aMesh(vertices, indices, textures));
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

	//if (init::LogALL || init::LogModel) std::cout << "getTextures / initialization - model.cpp" << std::endl;
	if (init::LogALL || init::LogModel) LogConsole::print("getTextures / initialization - model.cpp");
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Go over all images
	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		// uri of current texture
		std::string texPath = JSON["images"][i]["uri"];
		
		if (init::LogALL || init::LogModel) LogConsole::print("model.cpp - Texture path: " + texPath);

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
				if (init::LogALL || init::LogModel) LogConsole::print("model.cpp - Texture type: diffuse");
			}
			else if (texPath.find("metallicRoughness") != std::string::npos)
			{
				texType = "specular";
				if (init::LogALL || init::LogModel) LogConsole::print("model.cpp - Texture type: specular");
			}
			else if (texPath.find("normalTexture") != std::string::npos)
			{
				texType = "normal";
				if (init::LogALL || init::LogModel) LogConsole::print("model.cpp - Texture type: normal");
			}

			// Load the texture
			if (init::LogALL || init::LogModel) LogConsole::print("model.cpp - Loading texture: " + fileDirectory + texPath);
			if (init::LogALL || init::LogModel) std::cout << loadedTex.size() << std::endl;
			Texture texture;
			texture.createTexture((fileDirectory + texPath).c_str(), texType, loadedTex.size());
			textures.push_back(texture);
			loadedTex.push_back(texture);
			loadedTexName.push_back(texPath);
		}
	}

	return textures;
}

std::vector<Texture> Model::getTexturesForMesh(unsigned int indMesh)
{
	//std::cout << indMesh << "indmesh texture" << std::endl;

	std::vector<Texture> textures;
	if (false) {
		// fallback
		std::string fallbackPath = "Assets/Dependants/placeholder/texture/placeholder_unshaded.png";
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
			Texture fallbackTexture;
			fallbackTexture.createTexture(fallbackPath.c_str(), "specular", loadedTex.size());
			textures.push_back(fallbackTexture);
			loadedTex.push_back(fallbackTexture);
			loadedTexName.push_back(fallbackPath);
		}
	}
	else {
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
						Texture texture;
						texture.createTexture((fileDirectory + texturePath).c_str(), "diffuse", loadedTex.size());
						textures.push_back(texture);
						loadedTex.push_back(texture);
						//std::cout << "texturePath " << texturePath << std::endl;
						loadedTexName.push_back(texturePath);
					}
				}
			}

			// normal
			if (material.find("normalTexture") != material.end())
			{
				unsigned int textureIndex = material["normalTexture"]["index"];
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
						Texture texture;
						texture.createTexture((fileDirectory + texturePath).c_str(), "normal", loadedTex.size());
						textures.push_back(texture);
						loadedTex.push_back(texture);
						loadedTexName.push_back(texturePath);
					}
				}
			}
			else // Fallback for missing normal map: Use Purple
			{
				std::string fallbackPath = "Assets/Dependants/placeholder/texture/placeholder_normal_purple.png"; // Purple/Magenta texture for normal map
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
					Texture fallbackTexture;
					// Type must be "normal" for the normal map fallback
					fallbackTexture.createTexture(fallbackPath.c_str(), "normal", loadedTex.size());
					textures.push_back(fallbackTexture);
					loadedTex.push_back(fallbackTexture);
					loadedTexName.push_back(fallbackPath);
				}
			}

			// specular (metallicRoughness)
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
						Texture texture;
						texture.createTexture((fileDirectory + texturePath).c_str(), "specular", loadedTex.size());
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
					Texture fallbackTexture;
					fallbackTexture.createTexture(fallbackPath.c_str(), "specular", loadedTex.size());
					textures.push_back(fallbackTexture);
					loadedTex.push_back(fallbackTexture);
					loadedTexName.push_back(fallbackPath);
				}
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
				texUVs[i],
				//glm::vec3(0.0f, 0.0f, 0.0f)
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