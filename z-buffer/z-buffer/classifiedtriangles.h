//
//  classifiedtriangles.h
//
//
//  Created by ycymio on 16/3/25.
//
//
#ifndef CLASSIFIEDTRIANGLES_H_INCLUDED
#define CLASSIFIEDTRIANGLES_H_INCLUDED

#include "gl\glut.h"

struct ClassifiedTriangles{
    // 多边形所在平面的方程系数
    GLfloat a;
    GLfloat b;
    GLfloat c;
    GLfloat d;
    // 多边形的编号
    GLint id;
    // 多边形跨越的扫描线数量
    GLint dy;
    // 多边形颜色 -> 可以换成纹理?
    GLfloat rColor;
    GLfloat gColor;
    GLfloat bColor;
};

struct ClassifiedLines{
    // 边的上端点的x坐标
    GLfloat x;
    // 相邻两条扫描线交点的x坐标差dx (-1/k)
    GLfloat dx;
    // 边跨越的扫描线数量
    GLint dy;
    // 边所属多边形的编号
    GLint id;
};

struct ActivatedTriangles{
    // 多边形所在平面的方程系数
    GLfloat a;
    GLfloat b;
    GLfloat c;
    GLfloat d;
    // 多边形的编号
    GLint id;
    // 多边形跨越的剩余扫描线数量
    GLint dy;
    // 多边形颜色 -> 可以换成纹理?
    GLfloat rColor;
    GLfloat gColor;
    GLfloat bColor;
};

struct ActivatedLines{
    // 左交点
    GLfloat xl;
    GLfloat dxl;
    GLint dyl;
    // 右交点
    GLfloat xr;
    GLfloat dxr;
    GLint dyr;
    // 深度
    GLfloat zl;
    GLfloat dzx;
    GLfloat dzy;
    // 多边形编号
    GLint id;
};

#endif // CLASSIFIEDTRIANGLES_H_INCLUDED
