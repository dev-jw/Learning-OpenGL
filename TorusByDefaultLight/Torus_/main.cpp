//
//  main.cpp
//  Torus_
//
//  Created by neotv on 2020/7/10.
//  Copyright © 2020 neotv. All rights reserved.
//
#include <GLTools/GLTools.h>
#include <GLTools/GLFrustum.h>
#include <GLTools/GLMatrixStack.h>
#include <GLTools/GLShaderManager.h>
#include <GLTools/GLTriangleBatch.h>
#include <GLTools/GLGeometryTransform.h>
#include <GLTools/StopWatch.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrame viewFrame;

GLFrustum viewFrustum;
GLMatrixStack projectionMatrix;
GLMatrixStack modelViewMatrix;

GLShaderManager shaderManager;
GLGeometryTransform transformPipeLine;
GLTriangleBatch torusBatch;

void SetupRC() {
    glClearColor(0.3, 0.3, 0.3, 1.0);

    shaderManager.InitializeStockShaders();
    
    viewFrame.MoveForward(10.0f);
    
    gltMakeTorus(torusBatch, 1, 0.3, 50, 30);
    
    glPointSize(4.0f);
}

void RenderScene() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    glEnable(GL_DEPTH_TEST);

    
    modelViewMatrix.PushMatrix(viewFrame);
    
    GLfloat vRed[] = {1.0, 0.0, 0.0, 1.0f};
    
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeLine.GetModelViewMatrix(), transformPipeLine.GetProjectionMatrix(), vRed);
    
    torusBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
}

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.f, float(w)/float(h), 1, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeLine.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void SpecialKeys(int key, int x, int y) {
//    static CStopWatch rotTImer;
//    float yRot = rotTImer.GetElapsedSeconds() * 30.f;
//    viewFrame.RotateWorld(m3dDegToRad(yRot), 1.0, 1.0, 1.0);

    if(key == GLUT_KEY_UP)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_DOWN)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
    
    if(key == GLUT_KEY_LEFT)
        viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
    
    glutPostRedisplay();
}



int main(int argc, char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("Torus");
    
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
