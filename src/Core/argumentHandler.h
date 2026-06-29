#ifndef ARGUMENT_HANDLER_CLASS_H
#define ARGUMENT_HANDLER_CLASS_H

#include <vector>
#include <string>

class ArgumentHandler
{
public:
    enum argument
    {
        null = -1,
        noEditor = 0
    };
    
    static std::vector<ArgumentHandler::argument> arguments;
    
    static void processArguments(int argc, char* argv[]);
    
private:
    
    static argument stringComparisonThingy(const std::string argument); // returns index
    static void executeArguments();
    
};
#endif