//
//  main.cpp
//  SphereWorld
//
//  Created by neotv on 2020/7/10.
//  Copyright © 2020 neotv. All rights reserved.
//

#include <GLTools.h>
#include <GLShaderManager.h>
#include <GLFrustum.h>
#include <GLBatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <StopWatch.h>

#include <math.h>
#include <stdio.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLShaderManager     shaderManager;
GLMatrixStack       modelViewMatrix;
GLMatrixStack       projectionMatrix;
GLFrustum           viewFrustum;
GLGeometryTransform transformPipeLine;
GLFrame             cameraFrame;

GLTriangleBatch torusBatch;
GLTriangleBatch sphereBatch;
GLBatch floorBatch;

GLuint uiTexture[3];

void DrawToursAndSphere(GLfloat yRot);

// load Texture TGA
bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;
    
    // Read the texture bits
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if(pBits == NULL)
        return false;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0,
                 eFormat, GL_UNSIGNED_BYTE, pBits);
    
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);
    
    return true;
}

void SetupRC() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // 画地板
    GLfloat texSize = 10.0f;
    floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
    floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    floorBatch.Vertex3f(-20.0f, -0.41f, 20.0f);
        
    floorBatch.MultiTexCoord2f(0, texSize, 0.0f);
    floorBatch.Vertex3f(20.0f, -0.41f, 20.0f);
    
    floorBatch.MultiTexCoord2f(0, texSize, texSize);
    floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);
    
    floorBatch.MultiTexCoord2f(0, 0.0f, texSize);
    floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
    floorBatch.End();
    
    // 圆环
    gltMakeTorus(torusBatch, 0.4f, 0.15f, 40, 20);
    
    // 球体
    gltMakeSphere(sphereBatch, 0.1, 26, 13);
    
    // 生成随机小球
    for (int i = 0; i < NUM_SPHERES; i++) {
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        spheres[i].SetOrigin(x, 0.0f, z);
    }
    
    // 加载纹理
    glGenTextures(3, uiTexture);
    
    glBindTexture(GL_TEXTURE_2D, uiTexture[0]);
    LoadTGATexture("marble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D, uiTexture[1]);
    LoadTGATexture("marslike.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    
    glBindTexture(GL_TEXTURE_2D, uiTexture[1]);
    LoadTGATexture("moonlike.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
}

void RenderScene() {
    
    static CStopWatch    rotTimer;
    GLfloat yRot = rotTimer.GetElapsedSeconds() * 60.0f;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static GLfloat vFloorColor[] = { 1.0f, 1.0f, 1.0f, 0.75f};
    
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera);
    
    glBindTexture(GL_TEXTURE_2D, uiTexture[0]);
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE, transformPipeLine.GetModelViewProjectionMatrix(), vFloorColor, 0);
    floorBatch.Draw();
    
    DrawToursAndSphere(yRot);
    
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    
    glutPostRedisplay();
}

void DrawToursAndSphere(GLfloat yRot) {
    static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat vLightPos[] = {0.0f, 3.0f, 0.0f, 1.0f};
    
    // 自转球
    modelViewMatrix.Translate(0.0f, 0.2f, -2.5);
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot, 0.0, 1.0, 0.0);
    
    glBindTexture(GL_TEXTURE_2D, uiTexture[1]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                 transformPipeLine.GetModelViewMatrix(),
                                 transformPipeLine.GetProjectionMatrix(),
                                 vLightPos,
                                 vWhite,
                                 0);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix();
        
    // 公转球
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Rotate(yRot * -2.0, 0.0, 1.0, 0.0);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    
    glBindTexture(GL_TEXTURE_2D, uiTexture[1]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                 transformPipeLine.GetModelViewMatrix(),
                                 transformPipeLine.GetProjectionMatrix(),
                                 vLightPos,
                                 vWhite,
                                 0);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();
    
    // 随机小球
    for (int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                     transformPipeLine.GetModelViewMatrix(),
                                     transformPipeLine.GetProjectionMatrix(),
                                     vLightPos,
                                     vWhite,
                                     0);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
}

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.f, float(w)/float(h), 1, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeLine.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void SpecialKeys(int key, int x, int y) {
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    if (key == GLUT_KEY_UP) {
        cameraFrame.MoveForward(linear);
    }
    if (key == GLUT_KEY_DOWN) {
        cameraFrame.MoveForward(-linear);
    }
    if (key == GLUT_KEY_LEFT) {
        cameraFrame.RotateWorld(angular, 0.0, 1.0, 0.0);
    }
    if (key == GLUT_KEY_RIGHT) {
        cameraFrame.RotateWorld(-angular, 0.0, 1.0, 0.0);
    }
}

int main(int argc, char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("Sphere World");
    
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    
    return 0;
}
