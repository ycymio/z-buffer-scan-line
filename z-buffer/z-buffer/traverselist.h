#ifndef TRAVERSELIST_H_INCLUDED
#define TRAVERSELIST_H_INCLUDED

#include "classifiedtriangles.h"
#include <stdio.h>
#include <iostream>
// #include "gl/glut.h"
// #include <string.h>
// #include <map>
#include <vector>
// #include "glm/glm.hpp"
// #include <stdlib.h>
// #include <limits>

struct DepthColor{
    GLfloat depth;
    GLfloat rColor;
    GLfloat gColor;
    GLfloat bColor;
};

void addTrianglesClassifiedToActivated( GLfloat y_now, std::vector<ClassifiedTriangles> classified_t, std::vector<ClassifiedLines> classified_l, std::vector<ActivatedTriangles> &activated_t, std::vector<ActivatedLines> &activated_l);

#endif // TRAVERSELIST_H_INCLUDED
