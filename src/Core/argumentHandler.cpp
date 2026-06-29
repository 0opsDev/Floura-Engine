#include "argumentHandler.h"
#include <iostream>
#include "Editor/UI/ImGui/ImGuiWindow.h"

std::vector<ArgumentHandler::argument> ArgumentHandler::arguments;

void ArgumentHandler::processArguments(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        argument newArg = stringComparisonThingy(argv[i]);
        if (newArg == null) continue;
        arguments.push_back(newArg);
    }
    executeArguments();
}

ArgumentHandler::argument ArgumentHandler::stringComparisonThingy(const std::string argument)
{
    
    if (argument == "-") return null;
    else if (argument == "noEditor") return noEditor;
    
    return null;
}

void ArgumentHandler::executeArguments()
{
    for (int i = 0; i < arguments.size(); ++i)
    {
        switch (arguments[i])
        {
        case noEditor:
            FEImGuiWindow::imGuiEnabled = false;
            break;
        
        default:
            break;
        }
    }
    arguments.clear();
}

