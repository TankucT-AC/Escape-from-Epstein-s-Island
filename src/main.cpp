#include "Engine.hpp"
#include <iostream>
 
int main()
{
    try {
        Engine engine;
        engine.run();
    } catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
 
    return EXIT_SUCCESS;
}