#include "Core.h"

void clearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool checkError(const char* func, const char* file, int line)
{
    //std::cout << "checking error" << std::endl;
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        printf("OpenGL error %08x, at %s:%i - for %s\n", err, file, line, func);
        return false;
    }
    return true;
}