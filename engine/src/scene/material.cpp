#include "material.h"

#include <fstream>
#include <vector>
using namespace tst;

Material::~Material()
{
    glDeleteProgram(_programID);
}

Material::Material(const char * vertexFilePath, const char * fragmentFilePath)
    : _programID(0u)
{
    // Read the Vertex Shader code from the file
    std::ifstream vertexShaderStream(vertexFilePath, std::ios::in);
    if (vertexShaderStream.is_open())
    {
        std::string Line = "";
        while (std::getline(vertexShaderStream, Line))
        {
            _vertexShaderCode += "\n" + Line;
        }
        vertexShaderStream.close();
    }
    else
    {
        printf("File: %s not found", vertexFilePath);
        _state = ActorState::Failed;
        return;
    }

    // Read the Fragment Shader code from the file
    std::ifstream fragmentShaderStream(fragmentFilePath, std::ios::in);
    if (fragmentShaderStream.is_open())
    {
        std::string Line = "";
        while (getline(fragmentShaderStream, Line))
        {
            _fragmentShaderCode += "\n" + Line;
        }
        fragmentShaderStream.close();
    }
    else
    {
        printf("File: %s not found", fragmentFilePath);
        _vertexShaderCode.clear();
        _state = ActorState::Failed;
        return;
    }

    _state = ActorState::Loaded;
}

void Material::SetMatrix(const glm::mat4& matrix)
{
    _matrix.SetValue(matrix);
}

GLuint Material::GetProgramID() const
{
    return _programID;
}

void Material::Render(const glm::mat4& m) const
{
    glm::mat4 tmp(m * _matrix.GetValueLF());
    if (_state == ActorState::Binded)
    {
        assert(_programID);
        assert(_vertexShaderCode.empty());
        assert(_fragmentShaderCode.empty());
        glUseProgram(_programID);
        // Send our transformation to the currently bound shader, 
        // in the "MVP" uniform
        glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &tmp[0][0]);
        static glm::mat4x4 _rootMatrix; // закешируем единичную матрицу, чтобы не создавать каждый раз в Render
        //todo: render childrens
        Actor::Render(_rootMatrix);
        glUseProgram(0u);
    }
}

void Material::Update(float dt)
{
    if (_state == ActorState::Binded)
    {
        assert(_programID);
        assert(_vertexShaderCode.empty());
        assert(_fragmentShaderCode.empty());
    }
    else if (_state == ActorState::Loaded)
    {
        if (_mutex.try_lock())
        {
            Bind();

            _mutex.unlock();
        }
    }
    else
    {
        assert(_programID == 0);
        assert(_vertexShaderCode.empty());
        assert(_fragmentShaderCode.empty());
    }

    Actor::Update(dt);
}

void Material::Bind()
{
    assert(_programID == 0);
    assert(!_vertexShaderCode.empty());
    assert(!_fragmentShaderCode.empty());

    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int infoLogLength;

    // Compile Vertex Shader
    char const* vertexSourcePointer = _vertexShaderCode.c_str();
    glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
    glCompileShader(vertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        //printf("%s\n", &VertexShaderErrorMessage[0]);

        _vertexShaderCode.clear();
        _fragmentShaderCode.clear();

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        _state = ActorState::Failed;
        return;
    }

    // Compile Fragment Shader
    char const* fragmentSourcePointer = _fragmentShaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
    glCompileShader(fragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
        //printf("%s\n", &FragmentShaderErrorMessage[0]);

        _vertexShaderCode.clear();
        _fragmentShaderCode.clear();

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        _state = ActorState::Failed;
        return;
    }

    // Link the program
    _programID = glCreateProgram();
    glAttachShader(_programID, vertexShaderID);
    glAttachShader(_programID, fragmentShaderID);
    glLinkProgram(_programID);

    // Check the program
    glGetProgramiv(_programID, GL_LINK_STATUS, &result);
    glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(_programID, infoLogLength, NULL, &programErrorMessage[0]);
        //printf("%s\n", &programErrorMessage[0]);

        _vertexShaderCode.clear();
        _fragmentShaderCode.clear();

        glDetachShader(_programID, vertexShaderID);
        glDetachShader(_programID, fragmentShaderID);

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);

        glDeleteProgram(_programID);

        _state = ActorState::Failed;
        return;
    }

    glDetachShader(_programID, vertexShaderID);
    glDetachShader(_programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    _vertexShaderCode.clear();
    _fragmentShaderCode.clear();

    // Get a handle for our "MVP" uniform
    _matrixID = glGetUniformLocation(_programID, "MVP");

    _state = ActorState::Binded;
}