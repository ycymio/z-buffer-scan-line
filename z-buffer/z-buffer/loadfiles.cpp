#include "loadfiles.h"
// #include <cstring>

bool loadOBJ(
    const char * path,
    std::vector<glm::vec3> & out_vertices,
    std::vector<Faces3ui> & out_faces,
    GLfloat & ymin
){
	printf("Please input the file(.obj) operated. If you want to use the default file(%s), please press the enter.\n", path);
	char tmp_path[50];
	gets(tmp_path);
	if( tmp_path[0] != '\0'){
		path = tmp_path;
	}
	printf("Loading OBJ file %s...\n", path);

    std::vector<Faces3ui> vertexIndices;
    std::vector<glm::vec3> temp_vertices;
    ymin = std::numeric_limits<float>::max();
    FILE * file = fopen(path, "r");
    if( file == NULL )
    {
        printf("The fopen cannot read the file.\n");
        return false;
    }

    while( 1 )
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        if ( strcmp( lineHeader, "v" ) == 0 )
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            //TODO: 化了个整
            vertex.x = (int)vertex.x;
            vertex.y = (int)vertex.y;
            vertex.z = (int)vertex.z;
            if( vertex.y < ymin){
                ymin = vertex.y;
            }
            temp_vertices.push_back(vertex);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 )
        {
            printf("The load function does not support vt param");
        }
        else if ( strcmp( lineHeader, "vn" ) == 0 )
        {
            printf("The load function does not support vn param");
        }
        else if ( strcmp( lineHeader, "f" ) == 0 )
        {
            // unsigned int vertexIndex[3];
            Faces3ui vertexIndex;
            int matches = fscanf(file, "%d %d %d\n", &vertexIndex.first,  &vertexIndex.second, &vertexIndex.third);
            if (matches != 3)
            {
                printf("The face format needs to be like'1// 2// 3//\n" );
                return false;
            }
            vertexIndices.push_back(vertexIndex);
        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }
    fclose(file);
    out_faces = vertexIndices;
    out_vertices = temp_vertices;
    return true;
}

bool changeToTriangles(std::vector<glm::vec3> out_vertices, std::vector<Faces3ui> out_faces, std::vector<Triangle3vec> &out_triangles){
    std::vector<Triangle3vec> triangles;
    for( int i = 0; i < out_faces.size(); i++){
        Triangle3vec triangle;
        Faces3ui face = out_faces[i];
        triangle.a = out_vertices[face.first-1];
        triangle.b = out_vertices[face.second-1];
        triangle.c = out_vertices[face.third-1];
        triangles.push_back(triangle);
    }
    out_triangles = triangles;
    return true;
}

void getPlaneEquation(glm::vec3 a, glm::vec3 b, glm::vec3 c, GLfloat &planeA, GLfloat &planeB, GLfloat &planeC, GLfloat &planeD){
    // 假设不共线？
    planeA = (b.y-a.y)*(c.z-a.z) - (b.z-a.z)*(c.y-a.y);
    planeB = (c.x-a.x)*(b.z-a.z) - (b.x-a.x)*(c.z-a.z);
    planeC = (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    planeD = 0 - planeA*a.x - planeB*a.y - planeC*a.z;
//    printf("%f-%f-%f-%f-%f-%f-%f-%f", b.y,a.y,c.z,a.z,b.z,a.z,c.y,a.y);
}

void changeToClassified(std::vector<Triangle3vec> out_triangles, std::map<int, std::vector<ClassifiedTriangles> > &classified_t, std::map<int, std::vector<ClassifiedLines> > &classified_l, GLfloat &ymax){
    ymax = -std::numeric_limits<float>::min();
    std::map<int, std::vector<ClassifiedLines> > map_classified_l;
    for( int i = 0; i < out_triangles.size(); i++){
        Triangle3vec triangle = out_triangles[i];
        ClassifiedTriangles ctriangle;
        getPlaneEquation(triangle.a, triangle.b, triangle.c, ctriangle.a, ctriangle.b, ctriangle.c, ctriangle.d);
        ctriangle.id = i;
        int start_y;
        triMaxAndNum(triangle, start_y, ctriangle.dy);
        if( start_y > ymax ){
            ymax = start_y;
        }
        ctriangle.rColor = rand()*1.0/RAND_MAX;
        ctriangle.gColor = rand()*1.0/RAND_MAX;
        ctriangle.bColor = rand()*1.0/RAND_MAX;
		if(ctriangle.rColor < 0.3){
			ctriangle.rColor += 0.3;
		}
		if(ctriangle.gColor < 0.3){
			ctriangle.gColor += 0.3;
		}
		if(ctriangle.bColor < 0.3){
			ctriangle.bColor += 0.3;
		}
        // map<int,vector<classifiedTriangles>>::iterator iter = classified_t.find(start_y);
        if( classified_t.find(start_y) ==  classified_t.end()){
            std::vector<ClassifiedTriangles> vec_classfied_t;
            vec_classfied_t.push_back(ctriangle);
            classified_t.insert( std::map<int, std::vector<ClassifiedTriangles> >::value_type(start_y, vec_classfied_t));
        }
        else{
            std::vector<ClassifiedTriangles> vec_classified_t = classified_t[start_y];
            vec_classified_t.push_back(ctriangle);
            classified_t[start_y] = vec_classified_t;
        }
        // 边表要注意顶点处理 T__T
        // ClassifiedLines
        int keyA, keyB, keyC;
        ClassifiedLines clineA;
        ClassifiedLines clineB;
        ClassifiedLines clineC;
        // a-b A
        lineMaxAndNum(triangle.a, triangle.b, keyA, clineA.dy, clineA.x);
        clineA.id = i;
        if( (triangle.a.y - triangle.b.y) == 0){
            clineA.dx = 0;
        }
        else{
            clineA.dx = -(triangle.a.x - triangle.b.x)/(triangle.a.y - triangle.b.y);
        }
        // b-c B
        lineMaxAndNum(triangle.b, triangle.c, keyB, clineB.dy, clineB.x);
        clineB.id = i;
        if( (triangle.b.y - triangle.c.y) == 0){
            clineB.dx = 0;
        }
        else{
            clineB.dx = -(triangle.b.x - triangle.c.x)/(triangle.b.y - triangle.c.y);
        }
        // c-a C
        lineMaxAndNum(triangle.c, triangle.a, keyC, clineC.dy, clineC.x);
        clineC.id = i;
        if( (triangle.c.y - triangle.a.y) == 0){
            clineC.dx = 0;
        }
        else{
            clineC.dx = -(triangle.c.x - triangle.a.x)/(triangle.c.y - triangle.a.y);
        }
        if( keyA != start_y ){
            if( clineA.dy == 0 ){

            }
            else{
                --keyA;
                --clineA.dy;
                clineA.x = clineA.x + clineA.dx;
            }
        }
        else if( keyB != start_y){
            if( clineB.dy == 0 ){

            }
            else{
                --keyB;
                --clineB.dy;
                clineB.x = clineB.x + clineB.dx;
            }
        }
        else if( keyC != start_y){
            if( clineC.dy == 0 ){

            }
            else{
                --keyC;
                --clineC.dy;
                clineC.x = clineC.x + clineC.dx;
            }
        }
        // push push push
        if( classified_l.find(keyA) ==  classified_l.end()){
            std::vector<ClassifiedLines> vec_classfied_l;
            vec_classfied_l.push_back(clineA);
            classified_l.insert( std::map<int, std::vector<ClassifiedLines> >::value_type(keyA, vec_classfied_l));
        }
        else{
            std::vector<ClassifiedLines> vec_classfied_l = classified_l[keyA];
            vec_classfied_l.push_back(clineA);
            classified_l[keyA] = vec_classfied_l;
        }
        if( classified_l.find(keyB) ==  classified_l.end()){
            std::vector<ClassifiedLines> vec_classfied_l;
            vec_classfied_l.push_back(clineB);
            classified_l.insert( std::map<int, std::vector<ClassifiedLines> >::value_type(keyB, vec_classfied_l));
        }
        else{
            std::vector<ClassifiedLines> vec_classfied_l = classified_l[keyB];
            vec_classfied_l.push_back(clineB);
            classified_l[keyB] = vec_classfied_l;
        }
        if( classified_l.find(keyC) ==  classified_l.end()){
            std::vector<ClassifiedLines> vec_classfied_l;
            vec_classfied_l.push_back(clineC);
            classified_l.insert( std::map<int, std::vector<ClassifiedLines> >::value_type(keyC, vec_classfied_l));
        }
        else{
            std::vector<ClassifiedLines> vec_classfied_l = classified_l[keyC];
            vec_classfied_l.push_back(clineC);
            classified_l[keyC] = vec_classfied_l;
        }
    }
}

void triMaxAndNum(Triangle3vec t, int &iMax, int &iNum){
    //TODO:
    int a_y = (int)t.a.y;
    int b_y = (int)t.b.y;
    int c_y = (int)t.c.y;
    int iMin;
    if( a_y >= b_y ){
        iMax = a_y;
        iMin = b_y;
    }
    else{
        iMax = b_y;
        iMin = a_y;
    }
    if( c_y > iMax){
        iMax = c_y;
    }
    else if( c_y < iMin){
        iMin = c_y;
    }
    iNum = iMax - iMin;
}

void lineMaxAndNum(glm::vec3 a, glm::vec3 b, int &iMax, int &iNum, GLfloat &x){
    if(a.y >= b.y){
        iMax = a.y;
        iNum = a.y-b.y;
        x = a.x;
    }
    else{
        iMax = b.y;
        iNum = b.y-a.y;
        x = b.x;
    }
}
