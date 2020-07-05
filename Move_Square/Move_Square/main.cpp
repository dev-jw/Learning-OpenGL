//
//  main.cpp
//  Move_Square
//
//  Created by Zsy on 2020/7/3.
//  Copyright © 2020 Zsy. All rights reserved.
//

#include "GLTools.h"
#include "GLShaderManager.h"

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

GLBatch batch;
GLShaderManager shaderManager;

GLfloat blockSize = 0.1f;

GLfloat verts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize,  -blockSize, 0.0f,
    blockSize,  blockSize, 0.0f,
    -blockSize,  blockSize, 0.0f,
};

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
}

void SetupRC() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    shaderManager.InitializeStockShaders();
    batch.Begin(GL_TRIANGLE_FAN, 4);
    batch.CopyVertexData3f(verts);
    batch.End();
}

void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    GLfloat colors[] = {0.5f, 0.23f, 0.1f, 1.0f};
    
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY, colors);
    batch.Draw();
    glutSwapBuffers();
}

void SpecialKeys(int key, int x, int y) {
    GLfloat stepSize = 0.25f;
    
    GLfloat blockX = verts[0];
    GLfloat blockY = verts[10];
    
    if (key == GLUT_KEY_UP) {
        blockY += stepSize;
    }
    if (key == GLUT_KEY_DOWN) {
        blockY -= stepSize;
    }
    if (key == GLUT_KEY_LEFT) {
        blockX -= stepSize;
    }
    if (key == GLUT_KEY_RIGHT) {
        blockX += stepSize;
    }

    //当正方形移动超过最左边的时候
    if (blockX < -1.0f) {
        blockX = -1.0f;
    }
    //当正方形移动到最右边时
    //1.0 - blockSize * 2 = 总边长 - 正方形的边长 = 最左边点的位置
    if (blockX > (1.0f - blockSize * 2)) {
        blockX = 1.0f - blockSize * 2;
    }
    //当正方形移动到最下面时
    //-1.0 - blockSize * 2 = Y（负轴边界） - 正方形边长 = 最下面点的位置
    if (blockY < -1.0f + blockSize * 2) {
        blockY = -1.0f + blockSize * 2;
    }
    //当正方形移动到最上面时
    if (blockY > 1.0f) {
        blockY = 1.0f;
    }
    
    verts[0] = blockX;
    verts[1] = blockY - blockSize * 2;
    
    verts[3] = blockX + blockSize * 2;
    verts[4] = blockY - blockSize * 2;
    
    verts[6] = blockX + blockSize * 2;
    verts[7] = blockY;
    
    verts[9] = blockX;
    verts[10] = blockY;

    batch.CopyVertexData3f(verts);
    glutPostRedisplay();
}

int main(int argc, char* argv[]) {

    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    
    glutInitWindowSize(800, 800);
    
    glutCreateWindow("Square");
    
    glutReshapeFunc(ChangeSize);
    
    glutDisplayFunc(RenderScene);

    glutSpecialFunc(SpecialKeys);
    
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr,"glew error:%s\n",glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    glutMainLoop();
    
    return 0;
}
