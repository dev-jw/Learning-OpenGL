//
//  main.cpp
//  EarthRotation
//
//  Created by neotv on 2020/7/10.
//  Copyright © 2020 neotv. All rights reserved.
//

#include <GLTools/GLTools.h>
#include <GLTools/GLFrustum.h>
#include <GLTools/GLShaderManager.h>
#include <GLTools/GLTriangleBatch.h>
#include <GLTools/StopWatch.h>
#include <GLTools/GLMatrixStack.h>
#include <GLTools/GLGeometryTransform.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif


GLShaderManager shaderManager;
GLMatrixStack   modelViewMatrix;
GLMatrixStack   projectionMatrix;
GLFrustum       viewFrustum;

GLGeometryTransform transformPipeline;

GLTriangleBatch torusBatch;  // 地球
GLTriangleBatch sphereBatch; // 周围小球
GLBatch         floorBatch; // 地面

GLFrame         cameraFrame; // 摄像机帧
GLFrame         objectFrame; // 角色帧

// 附件随机的球
const GLint num_spheres = 50;
GLFrame spheres[num_spheres];

void SetupRC() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    shaderManager.InitializeStockShaders();
    
    glEnable(GL_DEPTH_TEST);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    floorBatch.Begin(GL_LINES, 324);
    for (GLfloat x = -20.0f; x < 20.0f; x += 0.5) {
        floorBatch.Vertex3f(x, -0.5f, 20.0f);
        floorBatch.Vertex3f(x, -0.5f, -20.0f);
        
        floorBatch.Vertex3f(20.0f, -0.5f, x);
        floorBatch.Vertex3f(-20.0f, -0.5f, x);
    }
    floorBatch.End();
    
    // 打球
    gltMakeSphere(torusBatch, 0.4f, 30, 50);
    
    // 随机球
    gltMakeSphere(sphereBatch, 0.1,  30, 40);
    for (int i = 0; i < num_spheres; i++) {
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        spheres[i].SetOrigin(x, 0.0f, z);
    }
}

void RenderScene() {
    static GLfloat vfloorColor[] = {0.0, 1.0f, 0.0, 1.0};
    static GLfloat vtoursColor[] = {1.0, 0.0f, 0.0, 1.0};
    static GLfloat vsphereColor[] = {0.0, 0.0f, 1.0, 1.0};

    
    static CStopWatch rotTImer;
    float yRot = rotTImer.GetElapsedSeconds() * 30.0f;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // 绘制地板
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vfloorColor);
    floorBatch.Draw();
    
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera);
    
    // 画 - 大球
    // 加 点光源
    M3DVector4f vLightPos = {0.0, 10.0f, 5.0f, 1.0f};
    
    modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);
    modelViewMatrix.PushMatrix();

    modelViewMatrix.Rotate(yRot, 1.0, 1.0, 1.0);
    
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vtoursColor);
    torusBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    
    for (int i = 0; i < num_spheres; i++) {
        // 单位矩阵->平移矩阵->平移矩阵
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vsphereColor);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }
    
    // 公转的球
    // 单位矩阵->平移矩阵 * 旋转矩阵
    modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_POINT_LIGHT_DIFF, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vLightPos, vsphereColor);
    sphereBatch.Draw();
    
    modelViewMatrix.PopMatrix();
    
    glutSwapBuffers();
    
    glutPostRedisplay();
}

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    viewFrustum.SetPerspective(35.f, float(w)/float(h), 1, 100.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void SpecialFunc(int key, int x, int y) {
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));
    
    // 控制角色向前后移动
    if(key == GLUT_KEY_UP)
        cameraFrame.MoveForward(linear);
    
    if(key == GLUT_KEY_DOWN)
        cameraFrame.MoveForward(-linear);
    
    // 控制角色向左右旋转
    if(key == GLUT_KEY_LEFT)
        cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
    
    if(key == GLUT_KEY_RIGHT)
        cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
}


int main(int argc, char * argv[]) {
    
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 600);
    
    glutCreateWindow("EarthRotation");
    
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialFunc);
    
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    
    glutMainLoop();
    
    return 0;
}
