#include <vid_lib/opengl/debug/debug_messenger.h>

#include <iostream>

namespace vid_lib::opengl::debug
{
namespace
{

void GLAPIENTRY
GlDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
              const void* userParam)
{
    std::cout << message << std::endl;
}

}

void DebugMessenger::Enable()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GlDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}

}