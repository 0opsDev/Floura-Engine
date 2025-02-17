#include "shaderClass.h"

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
        return(contents);
    }
    throw std::runtime_error("Failed to open file: " + std::string(filename));
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) 
{
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
}

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}

void Shader::compileErrors(unsigned int shader, const char* type) 
{
    GLint hasCompiled;
    char infoLog[1024];
    if (type != "PROGRAM") 
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            //error checking
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << std::endl;
        }
    }
    else 
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            //error checking
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << std::endl;
        }
    }
}
