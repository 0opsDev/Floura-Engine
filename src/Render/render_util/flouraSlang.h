#ifndef FLOURA_SLANG_CLASS_H
#define FLOURA_SLANG_CLASS_H

#include <string>

class FlouraSlang {
public:
    
    // bool returns if the conversion was sucessful
    static bool compileSlangToGLSL(const std::string &path, std::string & outVertexShader, std::string &outFragmentShader);
    static bool compileSlangToGLSLGeometry(const std::string &path, std::string & outVertexShader, std::string &outFragmentShader, std::string &outGeometryShader);
    static bool compileSlangToGLSLCompute(const std::string &path, std::string & outComputeShader);
    
    
    
private:
    
    //bool earlyCompliationStep(const std::string &path, );

};

#endif
