#include "shaderClass.h"
#include "utils/init.h"


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
    if (vertexFile == "skip" || fragmentFile == "skip") {
        if (init::LogALL || init::LogSystems) { std::cout << "Shader: Skip" << std::endl; }
        return;
    }
    else if (vertexFile == "") {
        if (init::LogALL || init::LogSystems) { std::cout << "vertexFile Path is Empty" << std::endl; }
        return;
    }
    else if (fragmentFile == "") {
        if (init::LogALL || init::LogSystems) { std::cout << "fragmentFile Path is Empty" << std::endl; }
        return;
    }

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
        if (init::LogALL || init::LogSystems) { std::cout << "Vert: " << vertexFile << "Frag: " << fragmentFile << std::endl; }

   
}

void Shader::Activate()
{
    glUseProgram(ID);
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


void Shader::setBool(const char* uniform, bool uniformBool)
{
    GLint uniformLocation = glGetUniformLocation(ID, uniform);
    glUniform1i(uniformLocation, uniformBool ? 1 : 0);
}