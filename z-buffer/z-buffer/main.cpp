#include "loadfiles.h"
#include "traverselist.h"

#define WIDTH 400
#define HEIGHT 400
//#define MAX_SIZE 100
//#define WINDOW_MIN_X -300
//#define WINDOW_MAX_X 300

using namespace std;

vector<glm::vec3> vertices;
vector<Faces3ui> faces;
vector<Triangle3vec> triangles;
map<int, vector<ClassifiedTriangles> > classified_triangles;
map<int, vector<ClassifiedLines> > classified_lines;
//int face_array[MAX_SIZE][3];
//int face_size;
vector<ActivatedTriangles> activated_triangles;
vector<ActivatedLines> activated_lines;
GLfloat ymax;
GLfloat ymin;
const char* path = "obj_teapot_sample.obj";
float scale = 1;

bool init()
{
    bool res = loadOBJ( path, vertices, faces, ymin);
    if ( res == false ){
        return false;
    }
    res = changeToTriangles(vertices, faces, triangles);
    if ( res == false ){
        return false;
    }
    changeToClassified(triangles, classified_triangles, classified_lines, ymax);
	return true;
    //cout << "The Max of y is " << ymax <<endl;
    //cout << "The Min of y is " << ymin <<endl;
}

void version()
{
		const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
		const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
		const GLubyte* OpenGLVersion =glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
		const GLubyte* gluVersion= gluGetString(GLU_VERSION); //返回当前GLU工具库版本
		printf("\n----------------------------------------\n");
		printf("OpenGL实现厂商的名字：%s\n", name);
		printf("渲染器标识符：%s\n", biaoshifu);
		printf("OOpenGL实现的版本号：%s\n",OpenGLVersion );
		printf("OGLU工具库版本：%s\n", gluVersion);
		printf("----------------------------------------\n");
}

void display()
{
		glClear(GL_COLOR_BUFFER_BIT);
		
		GLfloat line_width = 1.2f*scale/(ymax-ymin);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(line_width);
		glBegin(GL_LINES);
		glEnd();
		glFlush();
		for(GLfloat i = ymax; i >= ymin; i--){
			// 检查分类的多边形表，如果有新的多边形涉及该扫描线，
			// 则把它加入活化的多边形表中，并把该多边形在oxy平面上的投影和扫描线相交的边加入到活化边表中
			if( classified_triangles.find(i) !=  classified_triangles.end() && classified_lines.find(i) != classified_lines.end()){
				vector<ClassifiedTriangles> vec_classfied_t = classified_triangles[i];
				vector<ClassifiedLines> vec_classfied_l = classified_lines[i];
				addTrianglesClassifiedToActivated(i, vec_classfied_t, vec_classfied_l, activated_triangles, activated_lines);
			}
			// 建个map<dz,color>的表
			// vector<T> v(n,i)形式，v包含n 个值为 i 的元素
			DepthColor initialDepthColor;
			initialDepthColor.depth = -std::numeric_limits<float>::min();
			initialDepthColor.rColor = 0;
			initialDepthColor.gColor = 0;
			initialDepthColor.bColor = 0;
			int max_l = 0, max_r = 0;
			for( int j = 0; j < activated_lines.size(); j++){
				ActivatedLines acli = activated_lines[j];
				if( j == 0 ){
					max_l = acli.xl;
					max_r = acli.xr;
				}
				else{
					if( max_l > acli.xl){
						max_l = acli.xl;
					}
					if( max_r < acli.xr){
						max_r = acli.xr;
					}
				}
			}
			vector<DepthColor> vec_depthcolors (max_r-max_l+1, initialDepthColor);
			// vector<DepthColor> vec_depthcolors (WINDOW_MAX_X-WINDOW_MIN_X+1, initialDepthColor);
			for( int j = 0; j < activated_lines.size(); j++){
				// 活化边表不为空, 绘制扫描线上的点(包括深度计算什么的)
				// 取边对，当前扫描线y=i, 当前位置..?
				ActivatedLines acli = activated_lines[j];
				// 初始化帧缓冲器底色和z缓冲器值
				int dep = acli.zl;
				for( int k = acli.xl; k <= acli.xr; k++){
					if( dep > vec_depthcolors[k-max_l].depth){
						 vec_depthcolors[k-max_l].depth = dep;
						 for( int l = 0; l < activated_triangles.size(); l++){
							if( activated_triangles[l].id == acli.id){
								vec_depthcolors[k-max_l].rColor = activated_triangles[l].rColor;
								vec_depthcolors[k-max_l].gColor = activated_triangles[l].gColor;
								vec_depthcolors[k-max_l].bColor = activated_triangles[l].bColor;
								break;
							}
						 }
					}
					dep += acli.dzx;
				}
			}
	//vector<DepthColor> vec_depthcolors (WINDOW_MAX_X-WINDOW_MIN_X+1, initialDepthColor);
			// 画点
		//			glBegin(GL_POINTS);
		//glVertex3f(20,20,1);
		//glEnd();
		//
	 //   glFlush();
			if( activated_lines.size() != 0){
				glBegin(GL_LINES);
				int start_x = 0;
				GLfloat start_r = 0, start_g=0, start_b=0;
				for( int x = max_l; x <= max_r; x++){
					DepthColor tmp = vec_depthcolors[x-max_l];
					if( x == max_l){
						start_x = x;
						start_r = tmp.rColor;
						start_g = tmp.gColor;
						start_b = tmp.bColor;
					}
					else if(tmp.rColor != start_r || tmp.gColor != start_g || tmp.bColor != start_b){
						glColor3f(start_r, start_g, start_b);
						glVertex2f((start_x)*1.2f*scale/(ymax-ymin), i*1.2f*scale/(ymax-ymin));
						glVertex2f((x)*1.2f*scale/(ymax-ymin), i*1.2f*scale/(ymax-ymin));
						//glVertex2f((start_x)*1.8f/WIDTH, i/WIDTH);
						//glVertex2f((x-1)*1.8f/WIDTH, i/WIDTH);
						start_x = x;
						start_r = tmp.rColor;
						start_g = tmp.gColor;
						start_b = tmp.bColor;
					}
					/*if( vec_depthcolors[x-WINDOW_MIN_X].rColor != 0 ||vec_depthcolors[x-WINDOW_MIN_X].gColor != 0 || vec_depthcolors[x-WINDOW_MIN_X].bColor != 0){
						DepthColor dc = vec_depthcolors[x-WINDOW_MIN_X];
						printf("here");
					}*/
					//glColor3f( vec_depthcolors[x-WINDOW_MIN_X].rColor, vec_depthcolors[x-WINDOW_MIN_X].gColor, vec_depthcolors[x-WINDOW_MIN_X].bColor);
					//glVertex2f( x/WIDTH , i/HEIGHT); 
				}
				glColor3f(start_r, start_g, start_b);
				glVertex2f((start_x)*1.2f*scale/(ymax-ymin), i*1.2f*scale/(ymax-ymin));
				glVertex2f((max_r)*1.2f*scale/(ymax-ymin), i*1.2f*scale/(ymax-ymin));
				glEnd();
				glFlush();

			}
			
			for( int j = 0; j < activated_triangles.size(); j++){
				--activated_triangles[j].dy;
				if(activated_triangles[j].dy < 0){
					vector<ActivatedTriangles>::iterator it = activated_triangles.begin()+j;
					activated_triangles.erase(it);
				}
			}
			for( int j = 0; j < activated_lines.size(); j++){
				 // 修改活化边表
	//            activated_lines[j]
				  --activated_lines[j].dyl;
				  --activated_lines[j].dyr;
				  activated_lines[j].xl += activated_lines[j].dxl;
				  activated_lines[j].xr += activated_lines[j].dxr;
				  if( activated_lines[j].dyl < 0 ){
					  if( activated_lines[j].dyr < 0){
							vector<ActivatedLines>::iterator it = activated_lines.begin()+j;
							activated_lines.erase(it);
							continue;
					  }
					  else{
						// 找合适的边替代 左边
						// 每找一次id，耗的时间就心疼一次...
						if( classified_lines.find(i-1) != classified_lines.end()){
								vector<ClassifiedLines> replaceLines = classified_lines[i-1];
								for( int k = 0; k <replaceLines.size(); k++){
									if( replaceLines[k].id == activated_lines[j].id){
										activated_lines[j].xl = replaceLines[k].x;
										activated_lines[j].dxl = replaceLines[k].dx;
										activated_lines[j].dyl = replaceLines[k].dy;
										break;
									}
								}

						}
						else{
							// TODO
							// printf("Error: cannot find one lines respectively.");
						}
					  }
				  }
				  else if( activated_lines[j].dyr < 0){
						// 找合适的边替代 右边
						// 每找一次id，耗的时间就心疼一次...
						if( classified_lines.find(i-1) != classified_lines.end()){
								vector<ClassifiedLines> replaceLines = classified_lines[i-1];
								for( int k = 0; k <replaceLines.size(); k++){
									if( replaceLines[k].id == activated_lines[j].id){
										activated_lines[j].xr = replaceLines[k].x;
										activated_lines[j].dxr = replaceLines[k].dx;
										activated_lines[j].dyr = replaceLines[k].dy;
										break;
									}
								}

						}
						else{
							// printf("Error: cannot find one lines respectively.");
						}
				  }
				  activated_lines[j].zl += activated_lines[j].dzx*activated_lines[j].dxl + activated_lines[j].dzy;
			}
		}
		// printf("Finish?");
}

void test()
{
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_LINE_SMOOTH);
		gluLookAt(0,0,1,0,0,0,0,1,0);
		glLineWidth(8.0f);
		glBegin(GL_LINES);
		    glVertex3f(0.0f,0.0f,0.0f);
			glVertex3f(0.5f,0.5f,1.0f);
		glEnd();
	    glFlush();
		glBegin(GL_LINES);
		    glVertex3f(0.5f,0.0f,0.0f);
			glVertex3f(0.0f,0.5f,1.0f);
		glEnd();
	    glFlush();
		glDisable(GL_LINE_SMOOTH);
		// printf("Finish?");
}

int main(int argc, char* argv[])
{
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_RGBA);
     glutInitWindowPosition(200, 200);
     glutInitWindowSize(WIDTH, HEIGHT);
     glutCreateWindow("Z-Buffer");
     if(init()){
		printf("Please input the radio(max x/ max y). Also can use it to modulate the size. \n");
		scanf("%f", &scale);
		while( scale < 0 ){
			printf("The scale must be positive. Please input another radio(x/y):");
		    scanf("%d", &scale);
		}
		glutDisplayFunc(&display);
		version();
		glutMainLoop();
	 }
	 return 0;
}
