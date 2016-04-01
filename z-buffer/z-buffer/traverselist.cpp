#include "traverselist.h"

//std::vector<ClassifiedTriangles> vec_classfied_t = classified_triangles[i];
//map<int, vector<ClassifiedLines> > classified_lines;
//vector<ActivatedTriangles> activated_triangles;
//vector<ActivatedLines> activated_lines;
void addTrianglesClassifiedToActivated(
        GLfloat y_now,
        std::vector<ClassifiedTriangles> classified_t,
        std::vector<ClassifiedLines> classified_l,
        std::vector<ActivatedTriangles> &activated_t,
        std::vector<ActivatedLines> &activated_l
){
    for( int i = 0; i < classified_t.size(); i++){
        // 活化多边形表
        ActivatedTriangles acTriangle;
        acTriangle.a = classified_t[i].a;
        acTriangle.b = classified_t[i].b;
        acTriangle.c = classified_t[i].c;
        acTriangle.d = classified_t[i].d;
        acTriangle.id = classified_t[i].id;
        acTriangle.rColor = classified_t[i].rColor;
        acTriangle.gColor = classified_t[i].gColor;
        acTriangle.bColor = classified_t[i].bColor;
        acTriangle.dy = classified_t[i].dy;

        // 活化边表 能找到2个对应的
        // 平面仅呈现一条线
        if( classified_t[i].c == 0 ){
            // TODO : 默认看不到吧~
            continue;
        }

        activated_t.push_back(acTriangle);

        int first = -1, second = -1 , third = -1;
        for( int j = 0; j < classified_l.size(); j++){
            if( classified_l[j].id == classified_t[i].id){
                if( first == -1 ){
                    first = j;
                }
                else if(second == -1){
                    second = j; 
                }
				else{
					third = j;
					break;
				}
            }
        }
		if( third != -1){
			if( classified_l[first].dx == 0){
				int tmp = first;
				first = third;
				third = tmp;
			}
			else if( classified_l[second].dx == 0){
				int tmp = second;
				second = third;
				third = tmp;
			}
			if( classified_l[first].x > classified_l[second].x){
				int tmp = first;
                first = second;
                second = tmp;
			}
			// TODO:
			ActivatedLines special;
			special.id = classified_t[i].id;
			special.xl = classified_l[first].x;
			special.dxl = classified_l[first].dx;
			special.dyl = classified_l[first].dy;
			special.xr = classified_l[second].x;
			special.dxr = classified_l[second].dx;
			special.dyr = classified_l[second].dy;
			special.zl = -(classified_t[i].a * special.xl + classified_t[i].b * y_now + classified_t[i].d)/classified_t[i].c;
            special.dzx = - classified_t[i].a/classified_t[i].c;
            special.dzy = classified_t[i].b/classified_t[i].c;
            activated_l.push_back(special);
			continue;
		}
        ActivatedLines acline;
        acline.id = classified_t[i].id;
        if( classified_l[first].dx < classified_l[second].dx){
            int tmp = first;
            first = second;
            second = tmp;
        }
        acline.xl = classified_l[first].x;
        acline.xr = classified_l[second].x;
        if( acline.xl != acline.xr ){
            printf("Error: %d\n" ,classified_t[i].id);
        }
        acline.dxl = classified_l[second].dx;
        acline.dxr = classified_l[first].dx;
        acline.dyl = classified_l[second].dy;
        acline.dyr = classified_l[first].dy;
        acline.zl = -(classified_t[i].a * acline.xl + classified_t[i].b * y_now + classified_t[i].d)/classified_t[i].c;
        acline.dzx = - classified_t[i].a/classified_t[i].c;
        acline.dzy = classified_t[i].b/classified_t[i].c;
        activated_l.push_back(acline);
    }
}
