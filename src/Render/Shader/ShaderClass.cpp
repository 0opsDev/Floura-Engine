#include "shaderClass.h"
#include "utils/init.h"
#include <utils/logConsole.h>


std::string get_file_contents(const char* filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();

        // Check for BOM and remove it if present
        const std::string BOM = "\xEF\xBB\xBF";
        if (contents.compare(0, BOM.size(), BOM) == 0) {
            contents.erase(0, BOM.size());
        }

        return contents;
    }
    throw std::runtime_error("Failed to open file: " + std::string(filename));
}

void Shader::LoadShader(const char* vertexFile, const char* fragmentFile)
{
        // uses vertexFile which holds the shader file path and gets the contents of the file which is dumped into vertexCode
        std::string vertexCode = get_file_contents(vertexFile); 
        std::string fragmentCode = get_file_contents(fragmentFile);

        const char* vertexSource = vertexCode.c_str();
        const char* fragmentSource = fragmentCode.c_str();

        //CREATE VERTEX SHADER
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //ep2
        //feed vert shader data at line 5
        glShaderSource(vertexShader, 1, &vertexSource, NULL); //ep2
        //compile into machine code
        glCompileShader(vertexShader); //ep2
        //error checking
        compileErrors(vertexShader, "VERTEX");

        //CREATE FRAGMENT SHADER
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        //feed vert shader data at line 5
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        //compile into machine code
        glCompileShader(fragmentShader); //ep2
        //error checking
        compileErrors(fragmentShader, "FRAGMENT");

        //wrap then into shader 
        //create shader program
        ID = glCreateProgram(); //ep2
        //attach vert shader and frag shader
        glAttachShader(ID, vertexShader); //ep2
        glAttachShader(ID, fragmentShader);//ep2
        //wrap
        glLinkProgram(ID);//ep2
        //error checking
        compileErrors(ID, "PROGRAM");

        //delete shaders because its already in the program
        glDeleteShader(vertexShader);//ep2
        glDeleteShader(fragmentShader);//ep2
        if (init::LogALL || init::LogSystems)
        {
            LogConsole::print(("Vert: ", vertexFile, "Frag: ", fragmentFile));
        }

   
}

void Shader::LoadShaderGeom(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
    // uses vertexFile which holds the shader file path and gets the contents of the file which is dumped into vertexCode
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);
	std::string geometryCode = get_file_contents(geometryFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();
	const char* geometrySource = geometryCode.c_str();

    //CREATE VERTEX SHADER
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //feed vert shader data at line 5
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //compile into machine code
    glCompileShader(vertexShader);
    //error checking
    compileErrors(vertexShader, "VERTEX");

    //CREATE FRAGMENT SHADER
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //feed vert shader data at line 5
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    //compile into machine code
    glCompileShader(fragmentShader);
    //error checking
    compileErrors(fragmentShader, "FRAGMENT");

    //CREATE GEOMETRY SHADER
    GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    //feed vert shader data at line 5
    glShaderSource(geometryShader, 1, &geometrySource, NULL);
    //compile into machine code
    glCompileShader(geometryShader);
    //error checking
    compileErrors(geometryShader, "GEOMETRY");

    //wrap then into shader 
    //create shader program
    ID = glCreateProgram(); //ep2
    //attach vert shader and frag shader
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
	glAttachShader(ID, geometryShader);
    //wrap
    glLinkProgram(ID);//ep2
    //error checking
    compileErrors(ID, "PROGRAM");

    //delete shaders because its already in the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);
    if (init::LogALL || init::LogSystems)
    {LogConsole::print(("Vert: ", vertexFile, "Frag: ", fragmentFile, "Geom", geometryFile));}

}

void Shader::LoadComputeShader(const char* computeFile) {
    std::string computeCode = get_file_contents(computeFile);
    const char* computeSource = computeCode.c_str();

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeSource, NULL);
    glCompileShader(computeShader);
    compileErrors(computeShader, "COMPUTE");

    ID = glCreateProgram();
    glAttachShader(ID, computeShader);
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    glDeleteShader(computeShader);
    /*
    int work_grp_cnt[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
    std::cout << "Max work groups per compute shader" <<
    	" x:" << work_grp_cnt[0] <<
    	" y:" << work_grp_cnt[1] <<
    	" z:" << work_grp_cnt[2] << "\n";

    int work_grp_size[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
    std::cout << "Max work group sizes" <<
    	" x:" << work_grp_size[0] <<
    	" y:" << work_grp_size[1] <<
    	" z:" << work_grp_size[2] << "\n";

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";

    if (init::LogALL || init::LogSystems) {
        std::cout << "Compute: " << computeFile << std::endl;
    }
    */
}

void Shader::Activate()
{
    glUseProgram(ID);
}
// just making a seperate function for compute shaders to skip a if check
void Shader::ActivateCompute(int x, int y, int z) {
	glUseProgram(ID);
	// Dispatch the compute shader with the specified work group size
	glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type) {
    GLint hasCompiled;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "SHADER_COMPILATION_ERROR for: " << type << "\n" << infoLog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "SHADER_LINKING_ERROR for: " << type << "\n" << infoLog << std::endl;
        }
    }
}

void Shader::setInt(const char* uniform, int uniforIint) {
    glUniform1i(glGetUniformLocation(ID, uniform), uniforIint);
}
void Shader::setInt2(const char* uniform, int uniforIint1, int uniforIint2) {
    glUniform2i(glGetUniformLocation(ID, uniform), uniforIint1, uniforIint2);
}
void Shader::setInt3(const char* uniform, int uniforIint1, int uniforIint2, int uniforIint3) {
    glUniform3i(glGetUniformLocation(ID, uniform), uniforIint1, uniforIint2, uniforIint3);
}
void Shader::setInt4(const char* uniform, int uniforIint1, int uniforIint2, int uniforIint3, int uniforIint4) {
    glUniform4i(glGetUniformLocation(ID, uniform), uniforIint1, uniforIint2, uniforIint3, uniforIint4);
}

void Shader::setIntVector(const char* uniform, GLsizei count, const GLint* value) {
    glUniform1iv(glGetUniformLocation(ID, uniform), count, value);
}

void Shader::setFloat(const char* uniform, GLfloat uniFloat)
{
    glUniform1f(glGetUniformLocation(ID, uniform), uniFloat);
}
void Shader::setFloat2(const char* uniform, GLfloat uniFloat, GLfloat uniFloat2)
{
    glUniform2f(glGetUniformLocation(ID, uniform), uniFloat, uniFloat2);
}
void Shader::setFloat3(const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3)
{
    glUniform3f(glGetUniformLocation(ID, uniform), uniFloat, uniFloat2, uniFloat3);
}
void Shader::setFloat4(const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3, GLfloat uniFloat4)
{
    glUniform4f(glGetUniformLocation(ID, uniform), uniFloat, uniFloat2, uniFloat3, uniFloat4);
}

void Shader::setFloatVector(const char* uniform, GLsizei count, const GLfloat* value)
{
    glUniform1fv(glGetUniformLocation(ID, uniform), count, value);
}
void Shader::setFloat2Vector(const char* uniform, GLsizei count, const GLfloat* value)
{
    glUniform2fv(glGetUniformLocation(ID, uniform), count, value);
}
void Shader::setFloat3Vector(const char* uniform, GLsizei count, const GLfloat* value)
{
    glUniform3fv(glGetUniformLocation(ID, uniform), count, value);
}
void Shader::setFloat4Vector(const char* uniform, GLsizei count, const GLfloat* value)
{
    glUniform4fv(glGetUniformLocation(ID, uniform), count, value);
}

void Shader::setMat4(const char* uniform, glm::mat4 uniformMat4)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, uniform), 1, GL_FALSE, glm::value_ptr(uniformMat4));
}

void Shader::setMat3(const char* uniform, glm::mat4 uniformMat3)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, uniform), 1, GL_FALSE, glm::value_ptr(uniformMat3));
}

void Shader::setBool(const char* uniform, bool uniformBool)
{
    glUniform1i(glGetUniformLocation(ID, uniform), uniformBool ? 1 : 0);
}
