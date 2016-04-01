#ifndef LOADFILES_H_INCLUDED
#define LOADFILES_H_INCLUDED

#include "classifiedtriangles.h"
#include <gl\glut.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>
#include "glm/glm.hpp"
#include <stdlib.h>
#include <vector>
#include <limits>

struct Faces3ui
{
    GLint first;
    GLint second;
    GLint third;
};

struct Triangle3vec{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
};

bool loadOBJ(const char * path, std::vector<glm::vec3> & out_vertices, std::vector<Faces3ui> & out_faces, GLfloat & ymin);

bool changeToTriangles(std::vector<glm::vec3> out_vertices, std::vector<Faces3ui> out_faces, std::vector<Triangle3vec> &out_triangles);

void getPlaneEquation(glm::vec3 a, glm::vec3 b, glm::vec3 c, GLfloat &planeA, GLfloat &planeB, GLfloat &planeC, GLfloat &planeD);

void changeToClassified(std::vector<Triangle3vec> out_triangles, std::map<int, std::vector<ClassifiedTriangles> > &classified_t, std::map<int, std::vector<ClassifiedLines> > &classified_l, GLfloat &ymax);

void triMaxAndNum(Triangle3vec t, int &iMax, int &iNum);

void lineMaxAndNum(glm::vec3 a, glm::vec3 b, int &iMax, int &iNum, GLfloat &x);

#endif // LOADFILES_H_INCLUDED
