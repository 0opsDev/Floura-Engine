#include "flouraSlang.h"

#include <iostream>
#include <ostream>
#include <vector>
#include <slang/include/slang.h>
#include <slang/include/slang-com-ptr.h>


bool FlouraSlang::compileSlangToGLSL(const std::string& path, std::string& outVertexShader,
    std::string& outFragmentShader)
{
    
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef()))) {
        std::cerr << "Failed to create Slang global session" << std::endl;
        return false;
    }
    

    
    slang::TargetDesc targetDesc;
    targetDesc.format = SLANG_GLSL; // format
    targetDesc.profile = globalSession->findProfile("glsl_460"); // opengl version
    
    slang::SessionDesc sessionDesc;
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;
    //sessionDesc.allowGLSLSyntax = true;

    Slang::ComPtr<slang::ISession> session;
    if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef()))) {
        std::cerr << "Failed to create Slang session" << std::endl;
        return false;
    }
    
    // load .slang file into module
    Slang::ComPtr<slang::IBlob> diagnosticBlob;
    slang::IModule* module = session->loadModule(path.c_str(), diagnosticBlob.writeRef());
    
    // check for syntax errors
    if (!module)
    {
        if (diagnosticBlob){
            std::cerr << "Slang Error:" << path << (const char*)diagnosticBlob->getBufferPointer() << std::endl;
        }
        return false;
    }
    
    Slang::ComPtr<slang::IEntryPoint> vertexEntry;
    if (SLANG_FAILED(module->findEntryPointByName("vertexMain", vertexEntry.writeRef()))) {
        std::cerr << "Failed to find entry point: vertexMain" << std::endl;
        return false;
    }
    
    Slang::ComPtr<slang::IEntryPoint> fragmentEntry;
    if (SLANG_FAILED(module->findEntryPointByName("fragmentMain", fragmentEntry.writeRef()))) {
        std::cerr << "Failed to find entry point: fragmentMain" << std::endl;
        return false;
    }
    
    
    // link components into executable layout program
    std::vector<slang::IComponentType*> components = {module, vertexEntry, fragmentEntry};
    
    Slang::ComPtr<slang::IComponentType> linkedProgram;
    if (SLANG_FAILED(session->createCompositeComponentType(
            components.data(), 
            components.size(), 
            linkedProgram.writeRef(), 
            diagnosticBlob.writeRef()))) 
    {
        if (diagnosticBlob) std::cerr << "Slang Linking Error:\n" << (const char*)diagnosticBlob->getBufferPointer() << std::endl;
        return false;
    }
    
    // extract
    Slang::ComPtr<slang::IBlob> vertexBlob;
    Slang::ComPtr<slang::IBlob> fragmentBlob;
    
    if (SLANG_FAILED(linkedProgram->getEntryPointCode(0, 0, vertexBlob.writeRef(), diagnosticBlob.writeRef()))) return false;
    if (SLANG_FAILED(linkedProgram->getEntryPointCode(1, 0, fragmentBlob.writeRef(), diagnosticBlob.writeRef()))) return false;
    
    // PUT THEM THE DATA INTO THE OUT STRINGS!!!!
    outVertexShader = std::string((const char*)vertexBlob->getBufferPointer(), vertexBlob->getBufferSize());
    outFragmentShader = std::string((const char*)fragmentBlob->getBufferPointer(), fragmentBlob->getBufferSize());

    return true;
}

bool FlouraSlang::compileSlangToGLSLGeometry(const std::string& path, std::string& outVertexShader,
    std::string& outFragmentShader, std::string& outGeometryShader)
{
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef()))) {
        std::cerr << "Failed to create Slang global session" << std::endl;
        return false;
    }
    
    slang::TargetDesc targetDesc;
    targetDesc.format = SLANG_GLSL; // format
    targetDesc.profile = globalSession->findProfile("glsl_460"); // opengl version
    
    slang::SessionDesc sessionDesc;
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;
    
    Slang::ComPtr<slang::ISession> session;
    if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef()))) {
        std::cerr << "Failed to create Slang session" << std::endl;
        return false;
    }
    
    // load .slang file into module
    Slang::ComPtr<slang::IBlob> diagnosticBlob;
    slang::IModule* module = session->loadModule(path.c_str(), diagnosticBlob.writeRef());
    
    // check for syntax errors
    if (!module)
    {
        if (diagnosticBlob){
            std::cerr << "Slang Error:" << path << (const char*)diagnosticBlob->getBufferPointer() << std::endl;
        }
        return false;
    }
    
    Slang::ComPtr<slang::IEntryPoint> vertexEntry;
    if (SLANG_FAILED(module->findEntryPointByName("vertexMain", vertexEntry.writeRef()))) {
        std::cerr << "Failed to find entry point: vertexMain" << std::endl;
        return false;
    }
    
    Slang::ComPtr<slang::IEntryPoint> fragmentEntry;
    if (SLANG_FAILED(module->findEntryPointByName("fragmentMain", fragmentEntry.writeRef()))) {
        std::cerr << "Failed to find entry point: fragmentMain" << std::endl;
        return false;
    }
    
    Slang::ComPtr<slang::IEntryPoint> geometryEntry;
    if (SLANG_FAILED(module->findEntryPointByName("geometryMain", geometryEntry.writeRef()))) {
        std::cerr << "Failed to find entry point: geometryMain" << std::endl;
        return false;
    }
    
    
    // link components into executable layout program
    std::vector<slang::IComponentType*> components = {module, vertexEntry, geometryEntry, fragmentEntry};
    
    Slang::ComPtr<slang::IComponentType> linkedProgram;
    if (SLANG_FAILED(session->createCompositeComponentType(
            components.data(), 
            components.size(), 
            linkedProgram.writeRef(), 
            diagnosticBlob.writeRef()))) 
    {
        if (diagnosticBlob) std::cerr << "Slang Linking Error:\n" << (const char*)diagnosticBlob->getBufferPointer() << std::endl;
        return false;
    }
    
    // extract
    Slang::ComPtr<slang::IBlob> vertexBlob;
    Slang::ComPtr<slang::IBlob> fragmentBlob;
    Slang::ComPtr<slang::IBlob> geometryBlob;
    
if (SLANG_FAILED(linkedProgram->getEntryPointCode(0, 0, vertexBlob.writeRef(), diagnosticBlob.writeRef()))) return false;
    if (SLANG_FAILED(linkedProgram->getEntryPointCode(1, 0, geometryBlob.writeRef(), diagnosticBlob.writeRef()))) return false;
    if (SLANG_FAILED(linkedProgram->getEntryPointCode(2, 0, fragmentBlob.writeRef(), diagnosticBlob.writeRef()))) return false;
    
    // PUT THEM THE DATA INTO THE OUT STRINGS!!!!
    outVertexShader = std::string((const char*)vertexBlob->getBufferPointer(), vertexBlob->getBufferSize());
    outGeometryShader = std::string((const char*)geometryBlob->getBufferPointer(), geometryBlob->getBufferSize());
    outFragmentShader = std::string((const char*)fragmentBlob->getBufferPointer(), fragmentBlob->getBufferSize());

    return true;
}

bool FlouraSlang::compileSlangToGLSLCompute(const std::string& path, std::string& outComputeShader)
{
    Slang::ComPtr<slang::IGlobalSession> globalSession;
    if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef()))) {
        std::cerr << "Failed to create Slang global session" << std::endl;
        return false;
    }
    
    slang::TargetDesc targetDesc;
    targetDesc.format = SLANG_GLSL; // format
    targetDesc.profile = globalSession->findProfile("glsl_460"); // opengl version
    
    slang::SessionDesc sessionDesc;
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;
    
    Slang::ComPtr<slang::ISession> session;
    if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef()))) {
        std::cerr << "Failed to create Slang session" << std::endl;
        return false;
    }
    
    // load .slang file into module
    Slang::ComPtr<slang::IBlob> diagnosticBlob;
    slang::IModule* module = session->loadModule(path.c_str(), diagnosticBlob.writeRef());
    
    // check for syntax errors
    if (!module)
    {
        if (diagnosticBlob){
            std::cerr << "Slang Error:" << path << (const char*)diagnosticBlob->getBufferPointer() << std::endl;
        }
        return false;
    }
    
    Slang::ComPtr<slang::IEntryPoint> computeEntry;
    if (SLANG_FAILED(module->findEntryPointByName("computeMain", computeEntry.writeRef()))) {
        std::cerr << "Failed to find entry point: vertexMain" << std::endl;
        return false;
    }
    
    
    // link components into executable layout program
    std::vector<slang::IComponentType*> components = {module, computeEntry};
    
    Slang::ComPtr<slang::IComponentType> linkedProgram;
    if (SLANG_FAILED(session->createCompositeComponentType(
            components.data(), 
            components.size(), 
            linkedProgram.writeRef(), 
            diagnosticBlob.writeRef()))) 
    {
        if (diagnosticBlob) std::cerr << "Slang Linking Error:\n" << (const char*)diagnosticBlob->getBufferPointer() << std::endl;
        return false;
    }
    
    // extract
    Slang::ComPtr<slang::IBlob> computeBlob;
    
    if (SLANG_FAILED(linkedProgram->getEntryPointCode(0, 0, computeBlob.writeRef(), diagnosticBlob.writeRef()))) return false;
    
    // PUT THEM THE DATA INTO THE OUT STRINGS!!!!
    outComputeShader = std::string((const char*)computeBlob->getBufferPointer(), computeBlob->getBufferSize());

    return true;
}
