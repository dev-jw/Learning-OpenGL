//
//  main.cpp
//  Torus
//
//  Created by neotv on 2020/7/9.
//  Copyright © 2020 neotv. All rights reserved.
//

#include <GLTools/GLTools.h>
#include <GLTools/GLFrustum.h>
#include <GLTools/GLShaderManager.h>
#include <GLTools/GLTriangleBatch.h>
#include <GLTools/StopWatch.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLFrustum viewFrustum;

GLShaderManager shaderManager;

GLTriangleBatch torusBatch;

void SetupRC() {
    glClearColor(0.0f,0.0f,0.0f,1.0f);

    shaderManager.InitializeStockShaders();
    
    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30.0, 30.0);
    
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_BACK, GL_FILL);
//    glEnable(GL_DEPTH_TEST);
}

void RenderScene() {
    
    static CStopWatch rotTImer;
    float yRot = rotTImer.GetElapsedSeconds() * 30.f;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    M3DMatrix44f mTranslate, mRotate, mModelView;
    
    m3dTranslationMatrix44(mTranslate, 0.0f, 0.0f, -2.5f);
    
    m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 1.0f, 1.0, 1.0f);

    m3dMatrixMultiply44(mModelView, mTranslate, mRotate);
    
//    m3dMatrixMultiply44(mModelViewProjection, viewFrustum.GetProjectionMatrix(), mModelView);
    
    GLfloat vRed[] = {1.0, 0.0, 0.0, 1.0f};
    
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, mModelView, viewFrustum.GetProjectionMatrix(), vRed);
    torusBatch.Draw();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.f, float(w)/float(h), 1, 500.0f);
}

int main(int argc, char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("Torus");
    
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    
    return 0;
}
