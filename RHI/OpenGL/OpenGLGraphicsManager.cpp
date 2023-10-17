#include <iostream>
#include <fstream>
#include <stdio.h>
#include "glad/glad.h"
#include "OpenGLGraphicsManager.hpp"
#include "AssetLoader.hpp"
#include "IApplication.hpp"
#include "SceneManager.hpp"

const char VS_SHADER_SOURCE_FILE[] = "Shaders/basic.vs";
const char PS_SHADER_SOURCE_FILE[] = "Shaders/basic.ps";

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
     for (auto i = 0; i < m_Buffers.size() - 1; i++) { 
        glDisableVertexAttribArray(i);
    }

    for (auto buf : m_Buffers) {
        if(buf.first == "index") {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glDeleteBuffers(1, &buf.second);
    }

    // Detach the vertex and fragment shaders from the program.
    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);

    // Delete the vertex and fragment shaders.
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    // Delete the shader program.
    glDeleteProgram(m_shaderProgram);

    

}

void OpenGLGraphicsManager::Tick()
{
}

void OpenGLGraphicsManager::Draw()
{
    static float rotateAngle = 0.0f;
    //update work matrix to rotate the model

    rotateAngle += PI/120;
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

void OpenGLGraphicsManager::InitializeBuffers()
{
    auto& scene = g_pSceneManager->GetSceneForRendering();
    auto pGeometry = scene.GetFirstGeometry();
    while (pGeometry)
    {
        auto pMesh = pGeometry->GetMesh().lock();
        if(!pMesh) return;

        //set number of vertex properties
        auto vertexPropertiesCount = pMesh->GetVertexPropertiesCount();
        //set number of vertices in the vertex array
        auto vertexCount = pMesh->GetVertexCount();

        //allocate an opengl vertex array object
        GLuint vao;
        glGenVertexArrays(1,&vao);
        //bind the vertex array object to store all the buffers and vertex attributes 
        glBindVertexArray(vao);

        GLuint buffer_id;
        for(int32_t i=0;i<vertexPropertiesCount;i++){
            const SceneObjectVertexArray& v_property_array = pMesh->GetVertexPropertyArray(i);
            auto v_property_array_data_size = v_property_array.GetDataSize();
            auto v_property_array_data = v_property_array.GetData();

            // Generate an ID for the vertex buffer.
            glGenBuffers(1, &buffer_id);

            // Bind the vertex buffer and load the vertex (position and color) data into the vertex buffer.
            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            glBufferData(GL_ARRAY_BUFFER, v_property_array_data_size, v_property_array_data, GL_STATIC_DRAW);

            glEnableVertexAttribArray(i);

            glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
            switch (v_property_array.GetDataType()) {
                case VertexDataType::kVertexDataTypeFloat1:
                    glVertexAttribPointer(i, 1, GL_FLOAT, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeFloat2:
                    glVertexAttribPointer(i, 2, GL_FLOAT, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeFloat3:
                    glVertexAttribPointer(i, 3, GL_FLOAT, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeFloat4:
                    glVertexAttribPointer(i, 4, GL_FLOAT, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeDouble1:
                    glVertexAttribPointer(i, 1, GL_DOUBLE, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeDouble2:
                    glVertexAttribPointer(i, 2, GL_DOUBLE, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeDouble3:
                    glVertexAttribPointer(i, 3, GL_DOUBLE, false, 0, 0);
                    break;
                case VertexDataType::kVertexDataTypeDouble4:
                    glVertexAttribPointer(i, 4, GL_DOUBLE, false, 0, 0);
                    break;
                default:
                    assert(0);
            }

            m_Buffers[v_property_array.GetAttributeName()] = buffer_id;
        }

        //generate an ID for the index buffer
        glGenBuffers(1,&buffer_id);
        const SceneObjectIndexArray& index_array     = pMesh->GetIndexArray(0);
        auto index_array_size  = index_array.GetDataSize();
        auto index_array_data  = index_array.GetData();

        //bind the index buffer and load the index data into it
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_array_size,index_array_data,GL_STATIC_DRAW);

        //set the number of indices in the index array
        GLsizei indexCount  = static_cast<GLsizei>(index_array.GetIndexCount());
        GLenum mode;
        switch (pMesh->GetPrimitiveType())
        {
        case PrimitiveType::kPrimitiveTypePointList:
                mode = GL_POINTS;
                break;
            case PrimitiveType::kPrimitiveTypeLineList:
                mode = GL_LINES;
                break;
            case PrimitiveType::kPrimitiveTypeLineStrip:
                mode = GL_LINE_STRIP;
                break;
            case PrimitiveType::kPrimitiveTypeTriList:
                mode = GL_TRIANGLES;
                break;
            case PrimitiveType::kPrimitiveTypeTriStrip:
                mode = GL_TRIANGLE_STRIP;
                break;
            case PrimitiveType::kPrimitiveTypeTriFan:
                mode = GL_TRIANGLE_FAN;
                break;
            default:
                // ignore
                continue;
        }

        GLenum type;
        switch(index_array.GetIndexType())
        {
            case IndexDataType::kIndexDataTypeInt8:
                type = GL_UNSIGNED_BYTE;
                break;
            case IndexDataType::kIndexDataTypeInt16:
                type = GL_UNSIGNED_SHORT;
                break;
            case IndexDataType::kIndexDataTypeInt32:
                type = GL_UNSIGNED_INT;
                break;
            default:
                // not supported by OpenGL
                cerr << "Error: Unsupported Index Type " << index_array << endl;
                cerr << "Mesh: " << *pMesh << endl;
                cerr << "Geometry: " << *pGeometry << endl;
                continue;
        }

        m_Buffers["index"] = buffer_id;

        DrawBatchContext& dbc = *(new DrawBatchContext);
        dbc.vao     = vao;
        dbc.mode    = mode;
        dbc.type    = type;
        dbc.count   = indexCount;
        m_VAO.push_back(std::move(dbc));

        pGeometry = scene.GetNextGeometry();

    }
    return ;


}

void OpenGLGraphicsManager::RenderBuffers()
{
    //bind the vertex array object that stored all the information about the vertex and index buffer
   for (auto dbc : m_VAO)
    {
	    glBindVertexArray(dbc.vao);

        // Render the vertex buffer using the index buffer.
        glDrawElements(dbc.mode, dbc.count, dbc.type, 0);
    }
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
    glBindAttribLocation(m_shaderProgram,1, "inputNormal");

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
