#include"Billboard.h"
#include <utils/SettingsUtil.h>
#include <utils/UF.h>
#include "render/Shader/Cubemap.h"
#include "utils/timeUtil.h"
#include <glm/gtx/string_cast.hpp>

Shader PlaneShader("skip", "");
std::string singleTexturePath;

float s_Plane_Vertices[] = {
	// Positions       // Texture Coordinates
	-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
	 1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
	 1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
	-1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
};

float TimeAccumulatorBillboard;

unsigned int s_Plane_Indices[6] =
{
	0, 1, 2, // First triangle
	2, 3, 0  // Second triangle
};

void BillBoard::init(std::string path) {
	PlaneShader = Shader("Shaders/Db/BillBoard.vert", "Shaders/Db/BillBoard.frag");
	PlaneShader.Activate();
	glUniform1i(glGetUniformLocation(PlaneShader.ID, "skybox"), 0);
	skyboxBuffer(); // create buffer in memory for skybox

	LoadBillBoardTexture(path);
}
void BillBoard::initSeq(std::string path) { 
	PlaneShader = Shader("Shaders/Db/BillBoard.vert", "Shaders/Db/BillBoard.frag");
	PlaneShader.Activate();
	glUniform1i(glGetUniformLocation(PlaneShader.ID, "skybox"), 0);
	skyboxBuffer(); // create buffer in memory for skybox

	LoadSequence(path);
}

void BillBoard::LoadBillBoardTexture(std::string path) {
	singleTexturePath = path;
	glDeleteTextures(1, &BBTexture);

	// Generate and bind the texture
	glGenTextures(1, &BBTexture);
	glBindTexture(GL_TEXTURE_2D, BBTexture);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Load the texture
	int width, height, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(singleTexturePath.c_str(), &width, &height, &numColCh, 0);

	if (data) {
		GLenum format = GL_RGBA; // Default format

		// Determine correct format based on number of color channels
		if (numColCh == 4) {
			format = GL_RGBA;
		}
		else if (numColCh == 3) {
			format = GL_RGB;
		}
		else if (numColCh == 1) {
			format = GL_RED;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	else {
		std::cerr << "Failed to load texture: " << singleTexturePath << std::endl;
	}

	// Reset flip behavior to avoid unexpected issues later
	stbi_set_flip_vertically_on_load(false);
}

void BillBoard::LoadSequence(std::string path) {
	std::ifstream seqFile(path);
	if (seqFile.is_open()) {
		json seqData;
		seqFile >> seqData;
		seqFile.close();

		// Extract TexturePath
		TexturePath = seqData[0]["TexturePath"];
	//	std::cout << "TexturePath: " << TexturePath << std::endl;

		// Extract and sort texture filenames
		TextureNames = seqData[0]["textures"];
		std::sort(TextureNames.begin(), TextureNames.end());
		//iteration = TextureNames.size();
		//std::cout << "interations: " << iteration << std::endl;
		// Print sorted texture filenames
		//std::cout << "Sorted texture filenames:\n";
	}
	else {
		std::cerr << "Failed to open: " << path << std::endl;

	}
}

void BillBoard::UpdateSequence(int tickrate) {
	TimeAccumulatorBillboard += TimeUtil::s_DeltaTime;
	//because of deltatime we wont use a for loop
	if (TimeAccumulatorBillboard >= (1.0f / tickrate)) {
		iteration++;

		if (iteration <= TextureNames.size()) {
		//	std::cout << iteration << std::endl;
		//	std::cout << TexturePath << TextureNames[(iteration - 1)] << std::endl;
			LoadBillBoardTexture(TexturePath + TextureNames[(iteration - 1)]);
		}
		else {
			iteration = 0;

		}
		TimeAccumulatorBillboard = 0;
	}

}

void BillBoard::skyboxBuffer() {
	// Create VAO, VBO, and EBO for the billboard
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_Plane_Vertices), &s_Plane_Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_Plane_Indices), &s_Plane_Indices, GL_STATIC_DRAW);

	// **Position attribute**
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// **Texture coordinate attribute**
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void BillBoard::draw(bool doPitch, float x, float y, float z,
	float ScaleX, float ScaleY, float ScaleZ) {
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Activate shader
	PlaneShader.Activate();

	// Compute the forward vector towards the camera
	glm::vec3 camForward = glm::normalize(Camera::Position - glm::vec3(x, y, z));

	// Lock pitch if `doPitch == false`
	if (!doPitch) {
		camForward.y = 0.0f;
		camForward = glm::normalize(camForward);
	}

	// Compute right & up vectors
	glm::vec3 camRight = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camForward));
	glm::vec3 camUp = glm::normalize(glm::cross(camForward, camRight));

	// Construct billboard rotation matrix
	glm::mat4 billboardRotation = glm::mat4(glm::mat3(camRight, camUp, camForward));

	// Apply transformations: translation -> rotation -> scale
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x, y, z));
	model = model * billboardRotation;  // Ensure billboard rotation before scaling
	model = glm::scale(model, glm::vec3(ScaleX, ScaleY, ScaleZ));

	PlaneShader.Activate();
	// Pass transformations to shader
	glUniformMatrix4fv(glGetUniformLocation(PlaneShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(PlaneShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
	glUniform3f(glGetUniformLocation(PlaneShader.ID, "camPos"), Camera::Position.x, Camera::Position.y, Camera::Position.z);

	// Render the billboard
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, BBTexture);
	glUniform1i(glGetUniformLocation(PlaneShader.ID, "texture0"), 0);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BillBoard::Delete() {
	PlaneShader.Delete();
}
