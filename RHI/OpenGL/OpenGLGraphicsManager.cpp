#include <iostream>
#include <fstream>
#include <stdio.h>
#include "glad/glad.h"
#include "OpenGLGraphicsManager.hpp"
#include "AssetLoader.hpp"
#include "IApplication.hpp"

const char VS_SHADER_SOURCE_FILE[] = "Shaders/color.vs";
const char PS_SHADER_SOURCE_FILE[] = "Shaders/color.ps";

using namespace My;
using namespace std;

extern struct gladGLversionStruct GLVersion;

namespace My{
    extern AssetLoader* g_pAssetLoader;

    static void OutputShaderErrorMessage(unsigned int shaderId, const char* shaderFilename){
        int logSize,i;
        char* infoLog;
        ofstream fout;

        glGetShaderiv(shaderId,GL_INFO_LOG_LENGTH,&logSize);

        logSize ++;
        infoLog = new char[logSize];
        if(!infoLog){
            return;
        }

        glGetShaderInfoLog(shaderId,logSize,NULL,infoLog);
        fout.open("shader-error.txt");
        for(i=0;i<logSize;i++){
            fout<<infoLog[i];
        }

        fout.close();
        cerr<<"Error compiling shader. check Shader-error.txt for message."<<shaderFilename<<endl;
        return;
    }

    static void OutputLinkerErrorMessage(unsigned int programId){
        int logSize,i;
        char* infoLog;
        ofstream fout;

        glGetProgramiv(programId,GL_INFO_LOG_LENGTH,&logSize);
        logSize++;
        infoLog = new char[logSize];
        if(!infoLog){
            return;
        }
        glGetProgramInfoLog(programId,logSize,NULL,infoLog);

        fout.open("Link-error.txt");
        for(i=0;i<logSize;i++){
            fout<<infoLog[i];
        }
        fout.close();

        cerr<<"Error Compiling linker. check linker-error.txt for message."<<endl;
    }


}

int OpenGLGraphicsManager::Initialize()
{
    int result;

    result = gladLoadGL();
    if (!result) {
        printf("OpenGL load failed!\n");
        result = -1;
    } else {
        result = 0;
        cout<<"OpenGL Version "<<GLVersion.major<<"."<<GLVersion.minor<<" loaded"<<endl;

        if (GLAD_GL_VERSION_4_6) {
            cout<<"4.6 version"<<endl;
            // Set the depth buffer to be entirely cleared to 1.0 values.
            glClearDepth(1.0f);

            // Enable depth testing.
            glEnable(GL_DEPTH_TEST);

            // Set the polygon winding to front facing for the left handed system.
            glFrontFace(GL_CW);

            // Enable back face culling.
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            //initialize world/model matrix to the identity matrix
            BuildIdentityMatrix(m_worldMatrix);
            float fieldOfView = PI/4.0f;
            const GfxConfiguration& conf = g_pApp->GetConfiguration();
            float screenAspect = (float) conf.screenWidth/(float) conf.screenHeight;
            BuildPerspectiveFovLHMatrix(m_projectionMatrix,fieldOfView,screenAspect,screenNear,screenDepth);
        }
        InitializeShader(VS_SHADER_SOURCE_FILE,PS_SHADER_SOURCE_FILE);
        InitializeBuffers();
    }

    return result;
}

void OpenGLGraphicsManager::Finalize()
{
    //disable 2 vertex array attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    //release vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDeleteBuffers(1,&m_vertexBufferId);
    
    //release index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glDeleteBuffers(1,&m_indexBufferId);

    //release vertex array object
    glBindVertexArray(0);
    glDeleteVertexArrays(1,&m_vertexArrayId);

    //Detach the vertex and fragment shaders from the program
    glDetachShader(m_shaderProgram,m_vertexShader);
    glDetachShader(m_shaderProgram,m_fragmentShader);

    //delete the vertex and fragment shaders
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    glDeleteProgram(m_shaderProgram);

    

}

void OpenGLGraphicsManager::Tick()
{
}

void OpenGLGraphicsManager::Draw()
{
    static float rotateAngle = 0.0f;
    //update work matrix to rotate the model

    rotateAngle += PI/3600;
    Matrix4X4f rotationMatrixY;
    Matrix4X4f rotationMatrixZ;
    MatrixRotationY(rotationMatrixY,rotateAngle);
    MatrixRotationZ(rotationMatrixZ,rotateAngle);
    MatrixMultiply(m_worldMatrix,rotationMatrixZ,rotationMatrixY);

    //generate the view matrix based on the camera's postion
    CalculateCameraPosition();

    //set the color shader as the current shader program and set the matrices that it will use for rendering
    glUseProgram(m_shaderProgram);
    SetShaderParameters(m_worldMatrix,m_viewMatrix,m_projectionMatrix);

    //render the model using the color shader
    RenderBuffers();
    glFlush();

}

bool OpenGLGraphicsManager::SetShaderParameters(float *worldMatrix, float *viewMatrix, float *projectionMatrix)
{
    unsigned int location;
    //set the world matrix in the vertex shader
    location = glGetUniformLocation(m_shaderProgram,"worldMatrix");
    if(location == -1){
        return false;
    }
    glUniformMatrix4fv(location,1,false,worldMatrix);

    //set the view matrix in the vertex shader
    location = glGetUniformLocation(m_shaderProgram,"viewMatrix");
    if(location == -1){
        return false;
    }

    glUniformMatrix4fv(location,1,false,viewMatrix);

    //set the projection matrix in the vertex shader
    location = glGetUniformLocation(m_shaderProgram,"projectionMatrix");
    if(location == -1){
        return false;
    }
    glUniformMatrix4fv(location,1,false,projectionMatrix);


    return true;
}

bool OpenGLGraphicsManager::InitializeBuffers()
{
    struct VertexType
    {
        Vector3f position;
        Vector3f color;
    };

    VertexType vertices[] ={
        {{  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }},
        {{  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }},
        {{ -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }},
        {{ -1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }},
        {{  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }},
        {{  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f }},
        {{ -1.0f, -1.0f, -1.0f }, { 0.5f, 1.0f, 0.5f }},
        {{ -1.0f, -1.0f,  1.0f }, { 1.0f, 0.5f, 1.0f }},
    };    
    uint16_t indices[] = { 1, 2, 3, 3, 2, 6, 6, 7, 3, 3, 0, 1, 0, 3, 7, 7, 6, 4, 4, 6, 5, 0, 7, 4, 1, 0, 4, 1, 4, 5, 2, 1, 5, 2, 5, 6 };

    //set the number of vertices in the vertex array
    m_vertexCount = sizeof(vertices)/sizeof(VertexType);
    //set the number of indices in the index array
    m_indexCount = sizeof(indices)/sizeof(uint16_t);

    //allocate an openGL vertex array object
    glGenVertexArrays(1,&m_vertexArrayId);
    //bind the vertex array object to store all the buffers and vertex attributes
    glBindVertexArray(m_vertexArrayId);
    //generate an ID for the vertex buffer
    glGenBuffers(1,&m_vertexBufferId);
    //bind the vertex buffer and load the vertex (position and color) data into the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER,m_vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER,m_vertexCount*sizeof(VertexType),vertices,GL_STATIC_DRAW);
    //enable the two vertex array attibutes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // Specify the location and format of the position portion of the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), 0);

    // Specify the location and format of the color portion of the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(VertexType), (char*)NULL + (3 * sizeof(float)));

    //generate an ID for the index buffer
    glGenBuffers(1,&m_indexBufferId);

    //bind the index buffer and load the index data into it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,m_indexCount* sizeof(uint16_t), indices, GL_STATIC_DRAW);

    return true;
}

void OpenGLGraphicsManager::RenderBuffers()
{
    //bind the vertex array object that stored all the information about the vertex and index buffer
    glBindVertexArray(m_vertexArrayId);
    //render the vertex buffer using the index buffer
    glDrawElements(GL_TRIANGLES,m_indexCount,GL_UNSIGNED_SHORT,0);
    return ;
}

void OpenGLGraphicsManager::CalculateCameraPosition()
{
    Vector3f up,position,lookAt;
    float yaw,pitch,roll;
    Matrix4X4f rotationMatrix;

    //set up the vector that points upwards
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    //set up position of the camera is looking by default
    position.x = m_positionX;
    position.y = m_positionY;
    position.z = m_positionZ;

    //set up where the camera is looking by default
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    //set the yaw(y axis),pitch(x axis) , and roll(z axis) rotations in radians
    pitch = m_rotationX * 0.0174532925f;
    yaw   = m_rotationY * 0.0174532925f;
    roll  = m_rotationZ * 0.0174532925f;

    //create the rotation matrix from the yaw,pitch,and roll values
    MatrixRotationYawPitchRoll(rotationMatrix,yaw,pitch,roll);

    //transform the look at and up vector by the rotation matrix so the view is correctly rotated at the origin
    TransformCoord(lookAt,rotationMatrix);
    TransformCoord(up,rotationMatrix);

    //Translate the rotated camera position to the location of the viewer
    lookAt.x = position.x + lookAt.x;
    lookAt.y = position.y + lookAt.y;
    lookAt.z = position.z + lookAt.z;

    //finally create the view matrix from the three updated vectors
    BuildViewMatrix(m_viewMatrix,position,lookAt,up);

}

bool OpenGLGraphicsManager::InitializeShader(const char *vsFilename, const char *fsFilename)
{
    std::string vertexShaderBuffer;
    std::string fragmentShaderBuffer;
    int status;

    //load the vertex shader source file into a text buffer
    vertexShaderBuffer = g_pAssetLoader->SyncOpenAndReadTextFileToString(vsFilename);
    if(vertexShaderBuffer.empty()){
        return false;
    }

    //load the fragment shader source file into a text buffer
    fragmentShaderBuffer = g_pAssetLoader->SyncOpenAndReadTextFileToString(fsFilename);
    if(fragmentShaderBuffer.empty()){
        return false;
    }

    //create a vertex and fragment shader object
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    //copy the shader source code string into the vertex and fragment shader object
    const char* _v_c_str = vertexShaderBuffer.c_str();
    glShaderSource(m_vertexShader,1,&_v_c_str,NULL);
    const char* _f_c_str = fragmentShaderBuffer.c_str();
    glShaderSource(m_fragmentShader,1,&_f_c_str,NULL);

    //compile the shaders
    glCompileShader(m_vertexShader);
    glCompileShader(m_fragmentShader);

    //check to see if the vertex shader compile successfully
    glGetShaderiv(m_vertexShader,GL_COMPILE_STATUS,&status);
    if(status != 1){
        //if it did not compile then write the syntax error message out to text file for review
        OutputShaderErrorMessage(m_vertexShader,vsFilename);
        return false;
    }

    //check to see if the fragment shader compiled successfully
    glGetShaderiv(m_fragmentShader,GL_COMPILE_STATUS,&status);
    if(status != 1){
        OutputShaderErrorMessage(m_fragmentShader,fsFilename);
        return false;
    }

    //create a shader program object
    m_shaderProgram = glCreateProgram();

    //Attach the vertex and fragment shader to program object
    glAttachShader(m_shaderProgram,m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    //bind the shader input variables
    glBindAttribLocation(m_shaderProgram,0,"inputPosition");
    glBindAttribLocation(m_shaderProgram,1, "inputColor");

    //link the shader program
    glLinkProgram(m_shaderProgram);

    //check the status of the link
    glGetProgramiv(m_shaderProgram,GL_LINK_STATUS,&status);
    if(status != 1){
        //if it did not link then write the syntax error message out ot a text file for review
        OutputLinkerErrorMessage(m_shaderProgram);
        return false;
    }

    return true;

}

void OpenGLGraphicsManager::Clear()
{
    glClearColor(0.2f,0.3f,0.4f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
