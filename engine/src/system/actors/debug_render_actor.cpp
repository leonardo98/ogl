#include "debug_render_actor.h"

#include "system/debug_render.h"

#include <assert.h>

using namespace tst;

DebugRenderActor::DebugRenderActor()
    : _programID(0u)
    , _vertexBuffer(0u)
    , _state(ActorState::None)
    , _matrix(glm::mat4(1.f))
{
    _state = ActorState::Loaded;
}

void DebugRenderActor::SetMatrix(const glm::mat4& matrix)
{
    _matrix.SetValue(matrix);
}

// вызывается только из главного потока
void DebugRenderActor::Render(const RenderState& rs) const
{
    if (_state == ActorState::Binded)
    {
        if (DebugRender::Instance()->BatchSize())
        {
            assert(_programID);
            glUseProgram(_programID);
            // Send our transformation to the currently bound shader, 
            // in the "MVP" uniform
            glm::mat4 tmp(rs.matrix * _matrix.GetValueLF());
            glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &tmp[0][0]);


            glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, DebugRender::Instance()->BatchMemSize(), DebugRender::Instance()->BatchData(), GL_DYNAMIC_DRAW);

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(_vertexPosID);


            //// Prepare the triangle coordinate data
            //glVertexAttribPointer(_vertexPosID, COORDS_PER_VERTEX,
            //                         GLES20.GL_FLOAT, false,
            //                         VertexStride, VertexBuffer);


            glVertexAttribPointer(
                _vertexPosID,  // The attribute we want to configure
                3,                            // size
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                sizeof(glm::vec3),            // stride
                (void*)0                      // array buffer offset
            );

            // Draw the triangles !
            glDrawArrays(GL_LINES, 0, 3 * DebugRender::Instance()->BatchSize()); // 12*3 indices starting at 0 -> 12 triangles

            glDisableVertexAttribArray(_vertexPosID);
        }
    }
}

// вызывается только из главного потока
void DebugRenderActor::Update(float dt)
{
    if (_state == ActorState::Binded)
    {
        assert(_programID);
        assert(_vertexBuffer);
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
        assert(_vertexBuffer == 0);
    }

    Actor::Update(dt);
}

bool DebugRenderActor::Valid() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _state == ActorState::Loaded || _state == ActorState::Binded;
}

DebugRenderActor::~DebugRenderActor()
{
    if (_vertexBuffer)
    {
        assert(_programID);
        assert(_state == ActorState::Binded);
        glDeleteBuffers(1, &_vertexBuffer);
        glDeleteProgram(_programID);
        return;
    }
    assert(_state == ActorState::Loaded || _state == ActorState::Failed);
}

void DebugRenderActor::Bind()
{
    assert(_programID == 0);
    assert(_vertexBuffer == 0);

    std::string _vertexShaderCode = "\
    uniform mat4 uMVPMatrix;\
    attribute vec4 vPosition;\
    void main() {\
      gl_Position = uMVPMatrix * vPosition;\
    }";

    std::string _fragmentShaderCode = "\
    void main() {\
      gl_FragColor = vec4(1.0);\
    }";

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
        printf("%s\n", vertexShaderErrorMessage.data());

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
        printf("%s\n", fragmentShaderErrorMessage.data());

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
        printf("%s\n", programErrorMessage.data());

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

    // get handle to vertex shader's vPosition member
    _vertexPosID = glGetAttribLocation(_programID, "vPosition");

    // get handle to shape's transformation matrix
    _matrixID = glGetUniformLocation(_programID, "uMVPMatrix");

    glGenBuffers(1, &_vertexBuffer);

    _state = ActorState::Binded;
}