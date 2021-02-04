// 12141679 김정훈 컴퓨터그래픽스 최종프로젝트

#include <stdio.h>

#include <iostream>

#include "glut.h"
#include "glext.h"
//#include "freeglut.h"

#include <time.h>

#include <vector>
#include "Tokenizer.h"

#define vDetail 19
#define hDetail 37
#define SpaceBar 32

enum eBlockType
{
	E_BLOCK_TYPE0 = 0,
	E_BLOCK_TYPE1,
	E_BLOCK_TYPE2,
	E_BLOCK_TYPE3,
	E_BLOCK_TYPE4,
	E_BLOCK_TYPE5
};

enum eInsertMode
{
	E_MODEL_SELECT = 0,
	E_MODEL_INSERT,
};


// 글로벌 구조체
// Position, Vector, color
struct GLVector3
{
	float x;
	float y;
	float z;

	GLVector3()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	GLVector3(float nx, float ny, float nz)
	{
		x = nx;
		y = ny;
		z = nz;
	}
};

// 레고 블럭의 정보 
struct sLegoBlockInfo
{
	eBlockType type;
	float color[3];
	GLVector3 center;
	int nRotation;

	sLegoBlockInfo()
	{
		type = E_BLOCK_TYPE0;
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;

		nRotation = 0;
	}

	void reset()
	{
		type = E_BLOCK_TYPE0;
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;

		center.x = 0.f;
		center.y = 0.f;
		center.z = 0.f;

		nRotation = 0;
	}
};

// 모델링(큐브, 실린더) 데이터의 반지름, 높이 및 위치 (Trasposition)
float g_fBlockWidth = 0.f; 
float g_fBlockHeight = 0.f;

float g_fCylinderRadius = 0.f;
float g_fCylinderHeight = 0.f;

// 그라운드 블럭 사이즈 40 x 40
int g_nBlockCount = 40;

// 모델 갯수 6가지
int g_nModelCount = 6;

// 현재 모델 인덱스
int g_nCurrentModelIndex = 0;


// 마우스 클릭 및 이동좌표
int g_nMousePrePosX = 0;
int g_nMousePrePosY = 0;

// 카메라 회전 각도(왼쪽 버튼 드래그)
float g_fAngleX = 0.f;
float g_fAngleY = 0.f;

// 휠드래그 zoom in/out
float g_fWheelScale = 1.f;

// 스페이스바 선택시 레고블럭 추가 모드
eInsertMode g_currentInsertMode;

// 0 : lt, 1 : rb
GLVector3 g_boundingBox[2];

// ******** 모델 레고 벡터(배열) (상단 선택)
std::vector<sLegoBlockInfo> g_vecModelBlock;

// *************** 메인 레고 벡터(배열) (배치되는 레고)
std::vector<sLegoBlockInfo> g_vecMainBlock;

// 원점 정보
GLVector3 g_ptZero;

// PolygonMode && WireFrame 모드 
GLenum    g_ePolygonMode = GL_FILL;
BOOL      g_bLight = TRUE;

// 마우스 좌클릭시 True
BOOL	  g_bLbuttonDown = FALSE;

// 마우스 휠클릭시 True
BOOL	  g_bMbuttonDown = FALSE;

//Shading 관련변수 Ambient, Diffuse, Specular 
static const GLfloat afAmbientGreen[] = { 0.00, 0.50, 0.00, 1.00 };
static const GLfloat afAmbientBlue[] = { 0.00, 0.00, 0.50, 1.00 };
static const GLfloat afDiffuseGreen[] = { 0.00, 0.75, 0.00, 1.00 };
static const GLfloat afSpecularWhite[] = { 0.5, 0.5, 0.5, 1.00 };

// 함수 선언 (

// 기본 레고 블럭 초기화
void InitLegoBlockInfo();

// 3D객체 그리기
void Draw3DObject();

// 세로 원기둥
void GenerationCylinder(GLVector3 vecNormal, float fRadius, float fHeight);

// 박스
void GenerationCube(GLVector3 ptCenter, float fWidth, float fHeight);

// 삼각기둥
void GenerationPrism(GLVector3 ptStart, float fWidth, float fHeight);

// 모델 1~6
void GenerationLegoModel1(sLegoBlockInfo item, float fWidth, float fHeight);
void GenerationLegoModel2(sLegoBlockInfo item, float fWidth, float fHeight);
void GenerationLegoModel3(sLegoBlockInfo item, float fWidth, float fHeight);
void GenerationLegoModel4(sLegoBlockInfo item, float fWidth, float fHeight);
void GenerationLegoModel5(sLegoBlockInfo item, float fWidth, float fHeight);
void GenerationLegoModel6(sLegoBlockInfo item, float fWidth, float fHeight);


// 구체 
void GenerationSphere();

// 레고 바닥 그리기
void DrawLegoBlockGround();

// 레고 모델 상단(모델)
void DrawLegoSelectModel();

// 레고 메인 그리기
void DrawLegoMainModel();

// 불러오기
BOOL ImportCSVFile();

// 저장하기
BOOL ExportCSVFile();


/********************************************************/

// 화면 사이즈 조절
void ViewResize(GLsizei iWidth, GLsizei iHeight);

// 반복 호출을 위한 이벤트 (Thread 개념)
void IdleEvent();
void KeyboardEvent(unsigned char cKey, int iX, int iY);
void SpecialKeyboardEvent(int iKey, int iX, int iY);
void MouseClickedEvent(int button, int state, int iX, int iY);
void MouseMotionEvent(int iX, int iY);

// Vector Nomalize
void NormalizeVector(GLVector3 &rfVectorResult, GLVector3 &rfVectorSource);

// 두 점의 거리 계산
float CalcDistance3D(GLVector3 v1, GLVector3 v2);


float RADIAN(int x) { return float(x)*0.017453292519943295769236907684885f; }
float RADIAN(float x) { return (x)*0.017453292519943295769236907684885f; }
double RADIAN(double x) { return (x)*0.017453292519943295769236907684885; }
float DEGREE(float x) { return (x)*57.295779513082320876798154814114f; }
double DEGREE(double x) { return (x)*57.295779513082320876798154814114; }


int main()
{
	GLsizei iWidth = 1024.0;
	GLsizei iHeight = 768.0;

	//glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);

	// 화면(WindowSize)
	glutInitWindowSize(iWidth, iHeight);

	// 화면 옵션 (색상, 3D, 소수 사용)
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	glutCreateWindow("Lego Block Park");

	// 초기화
	InitLegoBlockInfo();

	///////////////////////////////////////////////////////////
	// ****************************

	// 백그라운드 화면 초기화 (r, g, b, a  0.f ~ 1.f)
	glClearColor(1.0, 1.0, 1.0, 1.0);


	// 3D 그리기.
	glutDisplayFunc(Draw3DObject);

	// 반복 호출을 위한 이벤트 (Thread 개념)
	glutIdleFunc(IdleEvent);

	// World Matrix 초기화
	glutReshapeFunc(ViewResize);

	// 일반 키보드 이벤트 호출
	glutKeyboardFunc(KeyboardEvent);

	// 마우스 클릭 이벤트 호출
	glutMouseFunc(MouseClickedEvent);


	// 마우스 이동 이벤트 호출
	glutMotionFunc(MouseMotionEvent);

	// Home/Insert 등과같은 스페셜 키보드 이벤트 호출
	glutSpecialFunc(SpecialKeyboardEvent);


	// 실제 그리는 부분 ==========================================================

	// 3D Depth 초기화
	glClearDepth(1.0);


	// World Matrix 초기화
	ViewResize(iWidth, iHeight);

	// Glut Window Loop 
	glutMainLoop();


	///////////////////////////////////////////////////////////
	// ****************************

}

// 외적 계산
void CrossProduct(GLVector3 &vecOutput, GLVector3& vec0, GLVector3& vec1)
{
	vecOutput.x = vec0.y*vec1.z - vec0.z*vec1.y;
	vecOutput.y = vec0.z*vec1.x - vec0.x*vec1.z;
	vecOutput.z = vec0.x*vec1.y - vec0.y*vec1.x;
}


void NormalizeVector(GLVector3 &rfVectorResult, GLVector3 &rfVectorSource)
{
	GLfloat fOldLength;
	GLfloat fScale;

	fOldLength = sqrtf((rfVectorSource.x * rfVectorSource.x) +
		(rfVectorSource.y * rfVectorSource.y) +
		(rfVectorSource.z * rfVectorSource.z));

	if (fOldLength == 0.0)
	{
		rfVectorResult.x = rfVectorSource.x;
		rfVectorResult.y = rfVectorSource.y;
		rfVectorResult.z = rfVectorSource.z;
	}
	else
	{
		fScale = 1.0 / fOldLength;
		rfVectorResult.x = rfVectorSource.x*fScale;
		rfVectorResult.y = rfVectorSource.y*fScale;
		rfVectorResult.z = rfVectorSource.z*fScale;
	}
}


float CalcDistance3D(GLVector3 v1, GLVector3 v2)
{
	float fRes = sqrt((v2.x - v1.x) * (v2.x - v1.x) +
		(v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z));
	return fRes;
}//두 점 사이 거리 계산


void InitLegoBlockInfo()
{
	g_fBlockWidth = 0.03f;
	g_fBlockHeight = 0.03f;

	g_fCylinderRadius = 0.008f;
	g_fCylinderHeight = 0.01f;

	g_nBlockCount = 31;
	g_nModelCount = 6;

	// 0 : lt, 1 : rb
	g_boundingBox[0].x = -float(g_nBlockCount) * g_fBlockWidth * 0.5f;
	g_boundingBox[0].y = 0.4f;
	g_boundingBox[0].z = -float(g_nBlockCount) * g_fBlockWidth * 0.5f;

	g_boundingBox[1].x = float(g_nBlockCount) * g_fBlockWidth * 0.5f;
	g_boundingBox[1].y = -0.4f;
	g_boundingBox[1].z = float(g_nBlockCount) * g_fBlockWidth * 0.5f;


	g_vecModelBlock.clear();
	for (int n = 0; n < g_nModelCount; n++)
	{
		// 선택용 블럭 정보 입력
		sLegoBlockInfo info;
		info.center.x = 0.f;
		info.center.y = g_boundingBox[0].y;
		info.center.z = 0.f;

		info.type = (eBlockType)n;

		if (E_BLOCK_TYPE0 == info.type)
		{
			info.color[0] = 1.f;
			info.color[1] = 0.f;
			info.color[2] = 0.f;
		}
		else if (E_BLOCK_TYPE1 == info.type)
		{
			info.color[0] = 1.f;
			info.color[1] = 1.f;
			info.color[2] = 0.f;
		}
		else if (E_BLOCK_TYPE2 == info.type)
		{
			info.color[0] = 0.f;
			info.color[1] = 0.5f;
			info.color[2] = 0.f;
		}
		else if (E_BLOCK_TYPE3 == info.type)
		{
			info.color[0] = 0.f;
			info.color[1] = 1.f;
			info.color[2] = 1.f;
		}
		else if (E_BLOCK_TYPE4 == info.type)
		{
			info.color[0] = 0.f;
			info.color[1] = 0.f;
			info.color[2] = 1.f;
		}
		else if (E_BLOCK_TYPE5 == info.type)
		{
			info.color[0] = 1.f;
			info.color[1] = 0.5f;
			info.color[2] = 0.f;
		}

		g_vecModelBlock.push_back(info);
	}

	g_currentInsertMode = E_MODEL_SELECT;
}

void GenerationLegoModel1(sLegoBlockInfo item, float fWidth, float fHeight)
{	
	// Matrix 그룹
	glPushMatrix();

	float fAngle = float(item.nRotation) * 90.f;

	glRotatef(fAngle, 0.f, 1.f, 0.f);


	GLVector3 ptCenter = item.center;
	for (int z = 0; z < 2; z++)
	{
		for (int x = 0; x < 2; x++)
		{
			// shading 처리된 색상 변경
			GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

			GLVector3 pos;
			pos.x = ptCenter.x + float(x) * fWidth;
			pos.y = ptCenter.y;
			pos.z = ptCenter.z + float(z) * fWidth;

			GenerationCube(pos, fWidth, fHeight);

			GLfloat afDiffuseColor2[] = { item.color[0] * 0.7f, item.color[1] * 0.7f, item.color[2] * 0.7f, 1.f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor2);

			pos.x += g_fBlockWidth * 0.5f;
			pos.z += g_fBlockWidth * 0.5f;

			pos.y += g_fBlockWidth;
			GenerationCylinder(pos, g_fCylinderRadius, g_fCylinderHeight);
		}
	}


	glPopMatrix();

}

void GenerationLegoModel2(sLegoBlockInfo item, float fWidth, float fHeight)
{
	// Matrix 그룹
	glPushMatrix();

	float fAngle = float(item.nRotation) * 90.f;

	glRotatef(fAngle, 0.f, 1.f, 0.f);

	GLVector3 ptCenter = item.center;
	for (int z = 0; z < 2; z++)
	{
		for (int x = 0; x < 2; x++)
		{
			if (z == 1 && x == 1)
				continue;

			GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

			GLVector3 pos;
			pos.x = ptCenter.x + float(x) * fWidth;
			pos.y = ptCenter.y;
			pos.z = ptCenter.z + float(z) * fWidth;

			GenerationCube(pos, fWidth, fHeight);

			GLfloat afDiffuseColor2[] = { item.color[0] * 0.7f, item.color[1] * 0.7f, item.color[2] * 0.7f, 1.f };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor2);

			pos.x += g_fBlockWidth * 0.5f;
			pos.z += g_fBlockWidth * 0.5f;

			pos.y += g_fBlockWidth;
			GenerationCylinder(pos, g_fCylinderRadius, g_fCylinderHeight);
		}
	}

	glPopMatrix();

}

void GenerationLegoModel3(sLegoBlockInfo item, float fWidth, float fHeight)
{
	GLVector3 ptCenter = item.center;
	for (int x = 0; x < 4; x++)
	{
		GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

		GLVector3 pos;
		pos.x = ptCenter.x + float(x) * fWidth;
		pos.y = ptCenter.y;
		pos.z = ptCenter.z;

		GenerationCube(pos, fWidth, fHeight);

		GLfloat afDiffuseColor2[] = { item.color[0] * 0.7f, item.color[1] * 0.7f, item.color[2] * 0.7f, 1.f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor2);

		pos.x += g_fBlockWidth * 0.5f;
		pos.z += g_fBlockWidth * 0.5f;

		pos.y += g_fBlockWidth;
		GenerationCylinder(pos, g_fCylinderRadius, g_fCylinderHeight);
	}

}

void GenerationLegoModel4(sLegoBlockInfo item, float fWidth, float fHeight)
{
	GLVector3 ptCenter = item.center;

	GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

	GLVector3 pos;
	pos.x = ptCenter.x;
	pos.y = ptCenter.y;
	pos.z = ptCenter.z;
	GenerationCube(pos, fWidth, fHeight);

	pos.y += g_fBlockHeight;
	GenerationCube(pos, fWidth, fHeight);

	GLfloat afDiffuseColor2[] = { item.color[0] * 0.7f, item.color[1] * 0.7f, item.color[2] * 0.7f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor2);

	pos.x += g_fBlockWidth * 0.5f;
	pos.z += g_fBlockWidth * 0.5f;

	pos.y += g_fBlockWidth;
	GenerationCylinder(pos, g_fCylinderRadius, g_fCylinderHeight);
}


void GenerationLegoModel5(sLegoBlockInfo item, float fWidth, float fHeight)
{
	GLVector3 ptCenter = item.center;

	GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

	GLVector3 pos;
	pos.x = ptCenter.x;
	pos.y = ptCenter.y;
	pos.z = ptCenter.z;
	GenerationCube(pos, fWidth, fHeight);

	pos.y += g_fBlockHeight;
	GenerationCube(pos, fWidth, fHeight);

	pos.x += g_fBlockWidth * 0.5f;
	pos.z += g_fBlockWidth * 0.5f;

	pos.y += g_fBlockWidth;
	GenerationCylinder(pos, g_fCylinderRadius, g_fCylinderHeight);

	pos.x += g_fBlockWidth * 0.5f;
	pos.z += g_fBlockWidth * 0.5f;


	GLfloat afDiffuseColor2[] = { item.color[0] * 0.7f, item.color[1] * 0.7f, item.color[2] * 0.7f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor2);

	GLVector3 pos2;
	pos2.x = ptCenter.x + fWidth;
	pos2.y = ptCenter.y + fHeight * 2;
	pos2.z = ptCenter.z;

	GenerationPrism(pos2, fWidth * 2, fHeight * 2);

}

void GenerationLegoModel6(sLegoBlockInfo item, float fWidth, float fHeight)
{
	// Matrix 그룹
	glPushMatrix();

	GLVector3 ptCenter = item.center;

	GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

	GLVector3 pos;
	pos.x = ptCenter.x + g_fBlockWidth * 0.5f;
	pos.y = ptCenter.y;
	pos.z = ptCenter.z + g_fBlockWidth * 0.5f;

	float fCylinderRadius = 0.015f;
	float fCylinderHeight = g_fBlockHeight * 4.f;


	GenerationCylinder(pos, fCylinderRadius, fCylinderHeight);


	GLfloat afDiffuseColor2[] = { 0.f, 0.5f, 0.f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor2);


	pos.y += g_fBlockHeight * 2.f;
	fCylinderRadius = 0.015f * 4.f;
	fCylinderHeight = g_fBlockHeight * 0.2f;

	GenerationCylinder(pos, fCylinderRadius, fCylinderHeight);

	pos.y += g_fBlockHeight * 0.5f;
	fCylinderRadius = 0.015f * 3.3f;

	GenerationCylinder(pos, fCylinderRadius, fCylinderHeight);

	pos.y += g_fBlockHeight * 0.5f;
	fCylinderRadius = 0.015f * 2.5f;

	GenerationCylinder(pos, fCylinderRadius, fCylinderHeight);

	pos.y += g_fBlockHeight * 0.5f;
	fCylinderRadius = 0.015f * 1.5f;

	GenerationCylinder(pos, fCylinderRadius, fCylinderHeight);

	pos.y += g_fBlockHeight * 0.5f;
	fCylinderRadius = 0.015f;

	GenerationCylinder(pos, fCylinderRadius, fCylinderHeight);

	glPopMatrix();
}


// 큐브 생성
void GenerationCube(GLVector3 ptCenter, float fWidth, float fHeight)
{
	srand((unsigned)time(NULL));


	int nRed = rand() % 255;
	int nGreen = rand() % 255;
	int nBlue = rand() % 255;

	float fRed = float(nRed) / 255.f;
	float fGreen = float(nGreen) / 255.f;
	float fBlue = float(nBlue) / 255.f;

	// 큐브 면 1-1
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y;
		pt0.z = ptCenter.z;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 1-2
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y;
		pt0.z = ptCenter.z;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y + fHeight;
		pt1.z = ptCenter.z;

		GLVector3 pt2;
		pt2.x = ptCenter.x;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt2.x - pt0.x;
		vec0.y = pt2.y - pt0.y;
		vec0.z = pt2.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt1.x - pt2.x;
		vec1.y = pt1.y - pt2.y;
		vec1.z = pt1.z - pt2.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 큐브 면 2-1
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y;
		pt0.z = ptCenter.z;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y;
		pt2.z = ptCenter.z + fHeight;

		GLVector3 vec0;
		vec0.x = pt1.x - pt0.x;
		vec0.y = pt1.y - pt0.y;
		vec0.z = pt1.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 2-2
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y;
		pt0.z = ptCenter.z;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z + fWidth;

		GLVector3 pt2;
		pt2.x = ptCenter.x;
		pt2.y = ptCenter.y;
		pt2.z = ptCenter.z + fWidth;

		GLVector3 vec0;
		vec0.x = pt2.x - pt0.x;
		vec0.y = pt2.y - pt0.y;
		vec0.z = pt2.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt1.x - pt2.x;
		vec1.y = pt1.y - pt2.y;
		vec1.z = pt1.z - pt2.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 3-1
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y;
		pt0.z = ptCenter.z;

		GLVector3 pt1;
		pt1.x = ptCenter.x;
		pt1.y = ptCenter.y + fHeight;
		pt1.z = ptCenter.z;

		GLVector3 pt2;
		pt2.x = ptCenter.x;
		pt2.y = ptCenter.y;
		pt2.z = ptCenter.z + fWidth;

		GLVector3 vec0;
		vec0.x = pt1.x - pt0.x;
		vec0.y = pt1.y - pt0.y;
		vec0.z = pt1.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt0.x;
		vec1.y = pt2.y - pt0.y;
		vec1.z = pt2.z - pt0.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 3-2
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z + fWidth;

		GLVector3 pt2;
		pt2.x = ptCenter.x;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt2.x - pt0.x;
		vec0.y = pt2.y - pt0.y;
		vec0.z = pt2.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt1.x - pt0.x;
		vec1.y = pt1.y - pt0.y;
		vec1.z = pt1.z - pt0.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 큐브 면 4-1
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x + fWidth;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x;
		pt1.y = ptCenter.y + fHeight;
		pt1.z = ptCenter.z + fWidth;

		GLVector3 pt2;
		pt2.x = ptCenter.x;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 4-2
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x + fWidth;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x;
		pt1.y = ptCenter.y + fHeight;
		pt1.z = ptCenter.z;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt0.x - pt2.x;
		vec0.y = pt0.y - pt2.y;
		vec0.z = pt0.z - pt2.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt1.x - pt2.x;
		vec1.y = pt1.y - pt2.y;
		vec1.z = pt1.z - pt2.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 큐브 면 5-1
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x + fWidth;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z + fWidth;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt0.x - pt2.x;
		vec0.y = pt0.y - pt2.y;
		vec0.z = pt0.z - pt2.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 5-2
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x + fWidth;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z;

		GLVector3 vec0;
		vec0.x = pt1.x - pt0.x;
		vec0.y = pt1.y - pt0.y;
		vec0.z = pt1.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt0.x;
		vec1.y = pt2.y - pt0.y;
		vec1.z = pt2.z - pt0.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 큐브 면 6-1
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z + fWidth;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y;
		pt2.z = ptCenter.z + fWidth;

		GLVector3 vec0;
		vec0.x = pt1.x - pt0.x;
		vec0.y = pt1.y - pt0.y;
		vec0.z = pt1.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}


	// 큐브 면 6-2
	{
		GLVector3 pt0;
		pt0.x = ptCenter.x;
		pt0.y = ptCenter.y + fHeight;
		pt0.z = ptCenter.z + fWidth;

		GLVector3 pt1;
		pt1.x = ptCenter.x + fWidth;
		pt1.y = ptCenter.y;
		pt1.z = ptCenter.z + fWidth;

		GLVector3 pt2;
		pt2.x = ptCenter.x + fWidth;
		pt2.y = ptCenter.y + fHeight;
		pt2.z = ptCenter.z + fWidth;

		GLVector3 vec0;
		vec0.x = pt0.x - pt2.x;
		vec0.y = pt0.y - pt2.y;
		vec0.z = pt0.z - pt2.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt1.x - pt2.x;
		vec1.y = pt1.y - pt2.y;
		vec1.z = pt1.z - pt2.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		nRed = rand() % 255;
		nGreen = rand() % 255;
		nBlue = rand() % 255;

		fRed = float(nRed) / 255.f;
		fGreen = float(nGreen) / 255.f;
		fBlue = float(nBlue) / 255.f;

		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}
}

// 프리즘 생성
void GenerationPrism(GLVector3 ptStart, float fWidth, float fHeight)
{
	srand((unsigned)time(NULL));


	int nRed = rand() % 255;
	int nGreen = rand() % 255;
	int nBlue = rand() % 255;

	float fRed = float(nRed) / 255.f;
	float fGreen = float(nGreen) / 255.f;
	float fBlue = float(nBlue) / 255.f;

	// 프리즘 면 1-1
	{
		GLVector3 pt0;
		pt0.x = ptStart.x;
		pt0.y = ptStart.y;
		pt0.z = ptStart.z;

		GLVector3 pt1;
		pt1.x = ptStart.x;
		pt1.y = ptStart.y - fHeight;
		pt1.z = ptStart.z;

		GLVector3 pt2;
		pt2.x = ptStart.x + fWidth;
		pt2.y = ptStart.y - fHeight;
		pt2.z = ptStart.z;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 프리즘 면 1-2
	{
		GLVector3 pt0;
		pt0.x = ptStart.x;
		pt0.y = ptStart.y;
		pt0.z = ptStart.z + g_fBlockWidth;

		GLVector3 pt1;
		pt1.x = ptStart.x;
		pt1.y = ptStart.y - fHeight;
		pt1.z = ptStart.z + g_fBlockWidth;

		GLVector3 pt2;
		pt2.x = ptStart.x + fWidth;
		pt2.y = ptStart.y - fHeight;
		pt2.z = ptStart.z + g_fBlockWidth;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);
		NormalizeVector(vecNormal, vecNormal);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 사각형 면 2-1
	{
		GLVector3 pt0;
		pt0.x = ptStart.x;
		pt0.y = ptStart.y;
		pt0.z = ptStart.z;

		GLVector3 pt1;
		pt1.x = ptStart.x;
		pt1.y = ptStart.y;
		pt1.z = ptStart.z + g_fBlockWidth;

		GLVector3 pt2;
		pt2.x = ptStart.x + fWidth;
		pt2.y = ptStart.y - fHeight;
		pt2.z = ptStart.z + g_fBlockWidth;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);
		NormalizeVector(vecNormal, vecNormal);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 사각형 면 2-2
	{
		GLVector3 pt0;
		pt0.x = ptStart.x;
		pt0.y = ptStart.y;
		pt0.z = ptStart.z;

		GLVector3 pt1;
		pt1.x = ptStart.x + fWidth;
		pt1.y = ptStart.y - fHeight;
		pt1.z = ptStart.z + g_fBlockWidth;

		GLVector3 pt2;
		pt2.x = ptStart.x + fWidth;
		pt2.y = ptStart.y - fHeight;
		pt2.z = ptStart.z;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal1, vecNormal0);
		NormalizeVector(vecNormal, vecNormal);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

	// 사각형 면 3-1
	{
		GLVector3 pt0;
		pt0.x = ptStart.x;
		pt0.y = ptStart.y - fHeight;
		pt0.z = ptStart.z;

		GLVector3 pt1;
		pt1.x = ptStart.x;
		pt1.y = ptStart.y - fHeight;
		pt1.z = ptStart.z + g_fBlockWidth;

		GLVector3 pt2;
		pt2.x = ptStart.x + fWidth;
		pt2.y = ptStart.y - fHeight;
		pt2.z = ptStart.z + g_fBlockWidth;

		GLVector3 vec0;
		vec0.x = pt0.x - pt1.x;
		vec0.y = pt0.y - pt1.y;
		vec0.z = pt0.z - pt1.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt2.x - pt1.x;
		vec1.y = pt2.y - pt1.y;
		vec1.z = pt2.z - pt1.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);
		NormalizeVector(vecNormal, vecNormal);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}
	// 사각형 면 3-2
	{
		GLVector3 pt0;
		pt0.x = ptStart.x;
		pt0.y = ptStart.y - fHeight;
		pt0.z = ptStart.z;

		GLVector3 pt1;
		pt1.x = ptStart.x + fWidth;
		pt1.y = ptStart.y - fHeight;
		pt1.z = ptStart.z + g_fBlockWidth;

		GLVector3 pt2;
		pt2.x = ptStart.x + fWidth;
		pt2.y = ptStart.y - fHeight;
		pt2.z = ptStart.z;

		GLVector3 vec0;
		vec0.x = pt2.x - pt0.x;
		vec0.y = pt2.y - pt0.y;
		vec0.z = pt2.z - pt0.z;

		GLVector3 vecNormal0;
		NormalizeVector(vecNormal0, vec0);


		GLVector3 vec1;
		vec1.x = pt1.x - pt2.x;
		vec1.y = pt1.y - pt2.y;
		vec1.z = pt1.z - pt2.z;

		GLVector3 vecNormal1;
		NormalizeVector(vecNormal1, vec1);

		GLVector3 vecNormal;
		CrossProduct(vecNormal, vecNormal0, vecNormal1);
		NormalizeVector(vecNormal, vecNormal);

		// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
		glColor3f(fRed, fGreen, fBlue);
		glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
		glVertex3f(pt0.x, pt0.y, pt0.z);
		glVertex3f(pt1.x, pt1.y, pt1.z);
		glVertex3f(pt2.x, pt2.y, pt2.z);
	}

}


void GenerationCylinder(GLVector3 ptCenter, float fRadius, float fHeight)
{
	int nHdetail = hDetail;

	GLVector3 vecRot[hDetail];
	for (int k = 0; k < nHdetail; k++)
	{
		vecRot[k].x = sin(RADIAN((float)k * 10));
		vecRot[k].y = 0.f;
		vecRot[k].z = cos(RADIAN((float)k * 10));
	}

	GLVector3 vecNormal(0.f, 1.f, 0.f);


	// 원기둥의 vertex
	GLVector3 ptCylinder[2][hDetail];

	for (int h = 0; h < nHdetail; h++)
	{
		// Z 축방향의 VecDir 방향으로 회전할 Center (구의 Center 아님)
		GLVector3 ptBottomPos;
		ptBottomPos.x = vecRot[h].x * fRadius;
		ptBottomPos.y = vecRot[h].y * fRadius;
		ptBottomPos.z = vecRot[h].z * fRadius;

		ptCylinder[0][h] = ptBottomPos;

		GLVector3 ptTopPos;
		ptTopPos.x = ptBottomPos.x + vecNormal.x * fHeight;
		ptTopPos.y = ptBottomPos.y + vecNormal.y * fHeight;
		ptTopPos.z = ptBottomPos.z + vecNormal.z * fHeight;

		ptCylinder[1][h] = ptTopPos;
	}

	srand((unsigned)time(NULL));

	for (int h = 0; h < nHdetail - 1; h++)
	{
		int nRed = rand() % 255;
		int nGreen = rand() % 255;
		int nBlue = rand() % 255;

		float fRed = float(nRed) / 255.f;
		float fGreen = float(nGreen) / 255.f;
		float fBlue = float(nBlue) / 255.f;

		// 반시계방향으로 폴리곤 생성 (1개)
		{
			GLVector3 pt0 = ptCylinder[0][h];
			GLVector3 pt1 = ptCylinder[0][h + 1];
			GLVector3 pt2 = ptCylinder[1][h];

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			NormalizeVector(vecNormal1, vec1);

			GLVector3 vecNormal;
			CrossProduct(vecNormal, vecNormal0, vecNormal1);

			pt0.x += ptCenter.x;
			pt0.y += ptCenter.y;
			pt0.z += ptCenter.z;

			pt1.x += ptCenter.x;
			pt1.y += ptCenter.y;
			pt1.z += ptCenter.z;

			pt2.x += ptCenter.x;
			pt2.y += ptCenter.y;
			pt2.z += ptCenter.z;

			// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
			glColor3f(fRed, fGreen, fBlue);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}


		// 반시계방향으로 폴리곤 생성 (2개)
		{
			GLVector3 pt0 = ptCylinder[1][h];
			GLVector3 pt1 = ptCylinder[1][h + 1];
			GLVector3 pt2 = ptCylinder[0][h + 1];

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			NormalizeVector(vecNormal1, vec1);

			GLVector3 vecNormal;
			CrossProduct(vecNormal, vecNormal0, vecNormal1);

			// 역방향이라 Normal Vector 뒤집음
			vecNormal.x *= -1.f;
			vecNormal.y *= -1.f;
			vecNormal.z *= -1.f;

			pt0.x += ptCenter.x;
			pt0.y += ptCenter.y;
			pt0.z += ptCenter.z;

			pt1.x += ptCenter.x;
			pt1.y += ptCenter.y;
			pt1.z += ptCenter.z;

			pt2.x += ptCenter.x;
			pt2.y += ptCenter.y;
			pt2.z += ptCenter.z;

			glColor3f(fRed, fGreen, fBlue);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}

		// 천장 Draw
		{
			GLVector3 pt0 = ptCylinder[0][h];
			GLVector3 pt1 = ptCylinder[0][h + 1];
			GLVector3 pt2 = g_ptZero;


			pt0.y += fHeight;
			pt1.y += fHeight;
			pt2.y += fHeight;

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			NormalizeVector(vecNormal1, vec1);

			GLVector3 vecNormal;
			CrossProduct(vecNormal, vecNormal0, vecNormal1);

			pt0.x += ptCenter.x;
			pt0.y += ptCenter.y;
			pt0.z += ptCenter.z;

			pt1.x += ptCenter.x;
			pt1.y += ptCenter.y;
			pt1.z += ptCenter.z;

			pt2.x += ptCenter.x;
			pt2.y += ptCenter.y;
			pt2.z += ptCenter.z;

			// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
			glColor3f(fRed, fGreen, fBlue);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}

		// 바닥 Draw
		{
			GLVector3 pt1 = ptCylinder[0][h];
			GLVector3 pt0 = ptCylinder[0][h + 1];
			GLVector3 pt2 = g_ptZero;

			GLVector3 vec0;
			vec0.x = pt1.x - pt0.x;
			vec0.y = pt1.y - pt0.y;
			vec0.z = pt1.z - pt0.z;

			GLVector3 vecNormal0;
			NormalizeVector(vecNormal0, vec0);


			GLVector3 vec1;
			vec1.x = pt2.x - pt0.x;
			vec1.y = pt2.y - pt0.y;
			vec1.z = pt2.z - pt0.z;

			GLVector3 vecNormal1;
			NormalizeVector(vecNormal1, vec1);

			GLVector3 vecNormal;
			CrossProduct(vecNormal, vecNormal0, vecNormal1);

			pt0.x += ptCenter.x;
			pt0.y += ptCenter.y;
			pt0.z += ptCenter.z;

			pt1.x += ptCenter.x;
			pt1.y += ptCenter.y;
			pt1.z += ptCenter.z;

			pt2.x += ptCenter.x;
			pt2.y += ptCenter.y;
			pt2.z += ptCenter.z;

			// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
			glColor3f(fRed, fGreen, fBlue);
			glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
			glVertex3f(pt0.x, pt0.y, pt0.z);
			glVertex3f(pt1.x, pt1.y, pt1.z);
			glVertex3f(pt2.x, pt2.y, pt2.z);
		}

	}

}


// 구체 생성
void GenerationSphere()
{
	// 회전(Mesh 생성 축 Vector)

	int nHdetail = hDetail;
	int nVdetail = vDetail;

	// 가로로 회전되는 방향 벡트
	// 세로로 회전되는 방향 벡트
	GLVector3 vecDir[hDetail];
	GLVector3 vecRight[hDetail];
	for (int k = 0; k < nHdetail; k++)
	{
		vecDir[k].x = sin(RADIAN((float)k * 10));
		vecDir[k].y = cos(RADIAN((float)k * 10));
		vecDir[k].z = 0;

		vecRight[k].x = 0.f;
		vecRight[k].y = sin(RADIAN((float)k * 10));
		vecRight[k].z = cos(RADIAN((float)k * 10));
	}

	float fRadius = 0.25f;

	// 구 중심점
	GLVector3 ptCenter;

	// 구의 vertex
	GLVector3 ptSphere[vDetail][hDetail];
	for (int v = 0; v < nVdetail; v++)
	{
		for (int h = 0; h < nHdetail; h++)
		{
			// Z 축방향의 VecDir 방향으로 회전할 Center (구의 Center 아님)
			GLVector3 ptCurrentPos;
			ptCurrentPos.x = ptCenter.x + vecRight[v].x * fRadius;
			ptCurrentPos.y = ptCenter.y + vecRight[v].y * fRadius;
			ptCurrentPos.z = ptCenter.z + vecRight[v].z * fRadius;

			// y 축값을 0으로 맞춰서 Current Center 계산
			// 그리고 그이후 두점간의 반지름을 구해서 Vecdir 만큼 이동해준다.
			GLVector3 ptCurrentCenter;
			ptCurrentCenter.x = ptCenter.x + vecRight[v].x * fRadius;
			ptCurrentCenter.y = 0.f;
			ptCurrentCenter.z = ptCenter.z + vecRight[v].z * fRadius;

			// 두점간의 거리계산
			float fCurRadius = CalcDistance3D(ptCurrentPos, ptCurrentCenter);

			// 최종 포인트 생성
			GLVector3 ptVertex;
			ptVertex.x = ptCurrentCenter.x + vecDir[h].x * fCurRadius;
			ptVertex.y = ptCurrentCenter.y + vecDir[h].y * fCurRadius;
			ptVertex.z = ptCurrentCenter.z + vecDir[h].z * fCurRadius;

			// 구체 Vertex 에 입력
			ptSphere[v][h] = ptVertex;
		}
	}

	srand((unsigned)time(NULL));

	for (int v = 0; v < nVdetail - 1; v++)
	{
		for (int h = 0; h < nHdetail - 1; h++)
		{
			int nRed = rand() % 255;
			int nGreen = rand() % 255;
			int nBlue = rand() % 255;

			float fRed = float(nRed) / 255.f;
			float fGreen = float(nGreen) / 255.f;
			float fBlue = float(nBlue) / 255.f;

			// 반시계방향으로 폴리곤 생성 (1개)
			{
				GLVector3 pt0 = ptSphere[v][h];
				GLVector3 pt1 = ptSphere[v][h + 1];
				GLVector3 pt2 = ptSphere[v + 1][h];

				GLVector3 vec0;
				vec0.x = pt1.x - pt0.x;
				vec0.y = pt1.y - pt0.y;
				vec0.z = pt1.z - pt0.z;

				GLVector3 vecNormal0;
				NormalizeVector(vecNormal0, vec0);


				GLVector3 vec1;
				vec1.x = pt2.x - pt0.x;
				vec1.y = pt2.y - pt0.y;
				vec1.z = pt2.z - pt0.z;

				GLVector3 vecNormal1;
				NormalizeVector(vecNormal1, vec1);

				GLVector3 vecNormal;
				CrossProduct(vecNormal, vecNormal0, vecNormal1);


				// 색상, Vertex 3개, Normal 1개로 메쉬(폴리곤)정보를 완성하여 gogo
				glColor3f(fRed, fGreen, fBlue);
				glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
				glVertex3f(pt0.x, pt0.y, pt0.z);
				glVertex3f(pt1.x, pt1.y, pt1.z);
				glVertex3f(pt2.x, pt2.y, pt2.z);
			}


			// 반시계방향으로 폴리곤 생성 (2개)
			{
				GLVector3 pt0 = ptSphere[v + 1][h];
				GLVector3 pt1 = ptSphere[v + 1][h + 1];
				GLVector3 pt2 = ptSphere[v][h + 1];

				GLVector3 vec0;
				vec0.x = pt1.x - pt0.x;
				vec0.y = pt1.y - pt0.y;
				vec0.z = pt1.z - pt0.z;

				GLVector3 vecNormal0;
				NormalizeVector(vecNormal0, vec0);


				GLVector3 vec1;
				vec1.x = pt2.x - pt0.x;
				vec1.y = pt2.y - pt0.y;
				vec1.z = pt2.z - pt0.z;

				GLVector3 vecNormal1;
				NormalizeVector(vecNormal1, vec1);

				GLVector3 vecNormal;
				CrossProduct(vecNormal, vecNormal0, vecNormal1);

				// 역방향이라 Normal Vector 뒤집음
				vecNormal.x *= -1.f;
				vecNormal.y *= -1.f;
				vecNormal.z *= -1.f;

				glColor3f(fRed, fGreen, fBlue);
				glNormal3f(vecNormal.x, vecNormal.y, vecNormal.z);
				glVertex3f(pt0.x, pt0.y, pt0.z);
				glVertex3f(pt1.x, pt1.y, pt1.z);
				glVertex3f(pt2.x, pt2.y, pt2.z);
			}

		}
	}
}

float fRot = 0.05f;

// 구체 및 큐브 생성 및 그리기
void Draw3DObject()
{
	// 컬러 뎁스 재초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	// gl Matrix 시작 합니다. (반대로 종료합니다. glPopMatrix) 
	// push 와 pop 은 쌍을 맞춰야합니다.
	glPushMatrix();


	// gl Depth Lighting On (Shadering)
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	// 0 번째 빛 근원 On
	glEnable(GL_LIGHT0);

	// 상단 글로벌 변수 선언 주석 참조 (Shading 관련 변수 입니다.)
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, afAmbientGreen);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseGreen);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, afAmbientBlue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, afSpecularWhite);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 25.0);
	// Shading 관련 변수 사용 끝


	// 자동 회전 및 카메라 포지션 정의
	glRotatef(30.f, 1.0, 1.0, 0.0);

	glRotatef(g_fAngleX, 0.0, 1.0, 0.0);
	glRotatef(g_fAngleY, 1.0, 0.0, 0.0);

	glScalef(g_fWheelScale, g_fWheelScale, g_fWheelScale);

	// 레고 바닥 객체 그리기
	DrawLegoBlockGround();

	// 레고 배치 모델 객체 그리기 (상단)
	DrawLegoSelectModel();

	// 레고 메인 모델 객체 그리기
	DrawLegoMainModel();


	glPopMatrix();

	glutSwapBuffers();
}


void DrawLegoMainModel()
{
	if (g_bLight)
		glEnable(GL_LIGHTING); //use vertex colors
	else
		glDisable(GL_LIGHTING); //use light material color

	int nSize = int(g_vecMainBlock.size());

	for (int n = 0; n < nSize; n++)
	{
		sLegoBlockInfo item = g_vecMainBlock[n];

		// shading 처리된 색상 변경
		GLfloat afDiffuseColor[] = { item.color[0], item.color[1], item.color[2], 1.f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

		glPushMatrix();

		float fAngle = float(item.nRotation) * 90.f;
		glRotatef(fAngle, 0.f, 1.f, 0.f);

		// 이 push 와 pop 이 화면 그려주는 구간
		glPushMatrix();

		glBegin(GL_TRIANGLES);

		if(E_BLOCK_TYPE0 == item.type)
			GenerationLegoModel1(item, g_fBlockWidth, g_fBlockHeight);
		else if (E_BLOCK_TYPE1 == item.type)
			GenerationLegoModel2(item, g_fBlockWidth, g_fBlockHeight);
		else if (E_BLOCK_TYPE2 == item.type)
			GenerationLegoModel3(item, g_fBlockWidth, g_fBlockHeight);
		else if (E_BLOCK_TYPE3 == item.type)
			GenerationLegoModel4(item, g_fBlockWidth, g_fBlockHeight);
		else if (E_BLOCK_TYPE4 == item.type)
			GenerationLegoModel5(item, g_fBlockWidth, g_fBlockHeight);
		else if (E_BLOCK_TYPE5 == item.type)
			GenerationLegoModel6(item, g_fBlockWidth, g_fBlockHeight);

		glEnd();

		glPopMatrix();

		glPopMatrix();

	}
}

void DrawLegoSelectModel()
{
	sLegoBlockInfo item = g_vecModelBlock[g_nCurrentModelIndex];

	// 이 push 와 pop 이 화면 그려주는 구간
	glPushMatrix();

	glBegin(GL_TRIANGLES);

	if (E_BLOCK_TYPE0 == item.type)
		GenerationLegoModel1(item, g_fBlockWidth, g_fBlockHeight);
	else if (E_BLOCK_TYPE1 == item.type)
		GenerationLegoModel2(item, g_fBlockWidth, g_fBlockHeight);
	else if (E_BLOCK_TYPE2 == item.type)
		GenerationLegoModel3(item, g_fBlockWidth, g_fBlockHeight);
	else if (E_BLOCK_TYPE3 == item.type)
		GenerationLegoModel4(item, g_fBlockWidth, g_fBlockHeight);
	else if (E_BLOCK_TYPE4 == item.type)
		GenerationLegoModel5(item, g_fBlockWidth, g_fBlockHeight);
	else if (E_BLOCK_TYPE5 == item.type)
		GenerationLegoModel6(item, g_fBlockWidth, g_fBlockHeight);

	glEnd();

	glPopMatrix();

}

void DrawLegoBlockGround()
{
	// shading 처리된 색상 변경
	static const GLfloat afDiffuseColor[] = { 0.f, 1.f, 0.f, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, afDiffuseColor);

	// 이 push 와 pop 이 화면 그려주는 구간
	glPushMatrix();

	for (int z = 0; z < g_nBlockCount; z++)
	{
		for (int x = 0; x < g_nBlockCount; x++)
		{
			GLVector3 pos;

			pos.x = g_boundingBox[0].x + float(x) * g_fBlockWidth;
			pos.y = g_boundingBox[1].y;
			pos.z = g_boundingBox[0].z + float(z) * g_fBlockWidth;

			glBegin(GL_TRIANGLES);

			GenerationCube(pos, g_fBlockWidth, g_fBlockHeight);

			pos.x += g_fBlockWidth * 0.5f;
			pos.z += g_fBlockWidth * 0.5f;

			pos.y += g_fBlockHeight;
			GenerationCylinder(pos, g_fCylinderRadius, g_fCylinderHeight);

			glEnd();
		}
	}

	glPopMatrix();
}

void ViewResize(GLsizei iWidth, GLsizei iHeight)
{
	glViewport(0, 0, iWidth, iHeight);

	// GL Projection 한다는건 2D로 투영하겠다는것
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 해상도 비율
	GLfloat fAspect = (GLfloat)iWidth / (GLfloat)iHeight;

	// 3차원 World Matrix 선언 (1.414~ 숫자는 해상도에 대한 비율 * 0.5 (절반)
	GLfloat fHalfWorldSize = (1.4142135623730950488016887242097 * 0.5f);
	glOrtho(-fHalfWorldSize * fAspect, fHalfWorldSize*fAspect, -fHalfWorldSize,
		fHalfWorldSize, -10 * fHalfWorldSize, 10 * fHalfWorldSize);

	// 2D로 해서 비율 및 3D world Matrix 선언 이후 (다시 3D 로 복귀)
	glMatrixMode(GL_MODELVIEW);
}

void IdleEvent()
{
	glutPostRedisplay();
}

void MouseMotionEvent(int iX, int iY)
{
	if (g_bLbuttonDown)
	{
		if (g_nMousePrePosX < iX)
		{
			g_fAngleX += 1.5f;
			g_nMousePrePosX = iX;
		}

		if (g_nMousePrePosX > iX)
		{
			g_fAngleX -= 1.5f;
			g_nMousePrePosX = iX;
		}

		if (g_nMousePrePosY < iY)
		{
			g_fAngleY += 1.5f;
			g_nMousePrePosY = iY;
		}

		if (g_nMousePrePosY > iY)
		{
			g_fAngleY -= 1.5f;
			g_nMousePrePosY = iY;
		}

	}

	if (g_bMbuttonDown)
	{
		if (g_nMousePrePosY < iY)
		{
			g_fWheelScale -= 0.03f;
			g_nMousePrePosY = iY;
		}

		if (g_nMousePrePosY > iY)
		{
			g_fWheelScale += 0.03f;
			g_nMousePrePosY = iY;
		}
	}
}

void MouseClickedEvent(int button, int state, int iX, int iY)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		g_nMousePrePosX = iX;
		g_nMousePrePosY = iY;

		g_bLbuttonDown = TRUE;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		g_bLbuttonDown = FALSE;


	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		g_nMousePrePosY = iY;

		g_bMbuttonDown = TRUE;
	}

	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
		g_bMbuttonDown = FALSE;

}

BOOL ExportCSVFile()
{
	DeleteFile(L"lego.csv");
	FILE * fp = fopen(".\\lego.csv", "w");

	if (!fp)
	{
		::MessageBox(NULL, L"Failed Export File!!", L"Warning", 0);
		return FALSE;
	}

	int nSize = int(g_vecMainBlock.size());

	for (int n = 0; n < nSize; n++)
	{
		// 변환한 문자열을 저장할 배열
		char token0[64] = { 0, };
		char token1[64] = { 0, };
		char token2[64] = { 0, };
		char token3[64] = { 0, };

		sLegoBlockInfo item = g_vecMainBlock[n];

		sprintf(token0, "%d,", int(item.type));
		fputs(token0, fp);

		sprintf(token1, "%f,%f,%f,", item.center.x, item.center.y, item.center.z);
		fputs(token1, fp);

		sprintf(token2, "%f,%f,%f,", item.color[0], item.color[1], item.color[2]);
		fputs(token2, fp);

		sprintf(token3, "%d\n", item.nRotation);
		fputs(token3, fp);
	}


	fclose(fp);

	return TRUE;
}


BOOL ImportCSVFile()
{
	FILE * fp = fopen(".\\lego.csv", "r");

	if (!fp)
	{
		::MessageBox(NULL, L"Failed Import File!!", L"Warning", 0);
		return FALSE;
	}



	CTokenizer tok;
	tok.ClearGroupMakerKey();
	tok.ClearDelimiterKey();
	tok.ClearSeparatorKey();
	tok.AddSeparatorKey(',');


	while (!feof(fp))
	{
		int		len = 0;
		char*	token = NULL;
		char	aline[512] = { 0, };
		memset(aline, 0, 512);
		fgets(aline, 512, fp);
		tok.SetLine(aline, 512);

		sLegoBlockInfo item;

		// type
		token = tok.GetNext(&len);

		if (NULL == token)
			break;

		item.type = eBlockType(atoi(token));

		// center
		token = tok.GetNext(&len);
		item.center.x = atof(token);

		token = tok.GetNext(&len);
		item.center.y = atof(token);

		token = tok.GetNext(&len);
		item.center.z = atof(token);

		// color
		token = tok.GetNext(&len);
		item.color[0] = atof(token);

		token = tok.GetNext(&len);
		item.color[1] = atof(token);

		token = tok.GetNext(&len);
		item.color[2] = atof(token);

		// angle
		token = tok.GetNext(&len);
		item.nRotation = atoi(token);

		g_vecMainBlock.push_back(item);
	}

	fclose(fp);

	return TRUE;
}


void KeyboardEvent(unsigned char cKey, int iX, int iY)
{
	switch (cKey)
	{
	case 'w':
	{
		// W 버튼을 누르면 라인, 면 모드가 스위칭됨
		// GL_FILL 면모드, GL_LINE 선모드
		if (g_ePolygonMode == GL_LINE)
			g_ePolygonMode = GL_FILL;
		else
			g_ePolygonMode = GL_LINE;

		glPolygonMode(GL_FRONT_AND_BACK, g_ePolygonMode);
	} break;
	case 'l':
	{
		// W 버튼을 누르면 라인, 면 모드가 스위칭됨
		// GL_FILL 면모드, GL_LINE 선모드


		if (g_bLight)
			glEnable(GL_LIGHTING); //use vertex colors
		else
			glDisable(GL_LIGHTING); //use light material color

		g_bLight = !g_bLight;
	}
	break;
	case 'r':
	{
		int idx = int(g_vecMainBlock.size() - 1);

		g_vecMainBlock[idx].nRotation++;

		if (g_vecMainBlock[idx].nRotation > 3)
			g_vecMainBlock[idx].nRotation = 0;

	}
	break;

	case '-':
	{
		// 현재 상태가 insert mode 일경우 모델을 이동시킬 수 있다.
		if (g_currentInsertMode == E_MODEL_INSERT)
		{
			int idx = int(g_vecMainBlock.size() - 1);

			g_vecMainBlock[idx].center.y -= g_fBlockHeight;

			if (g_vecMainBlock[idx].center.y < g_boundingBox[1].y + g_fBlockHeight)
				g_vecMainBlock[idx].center.y = g_boundingBox[1].y + g_fBlockHeight;
		}

	}
	break;
	case '+':
	{
		// 현재 상태가 insert mode 일경우 모델을 이동시킬 수 있다.
		if (g_currentInsertMode == E_MODEL_INSERT)
		{
			int idx = int(g_vecMainBlock.size() - 1);

			g_vecMainBlock[idx].center.y += g_fBlockHeight;
		}

	}
	break;
	case 'i':
	{
		BOOL bRet = ImportCSVFile();

		if (bRet)
			::MessageBox(NULL, L"Successed Import File", L"Info", 0);

	}
	break;
	case 'e':
	{
		BOOL bRet = ExportCSVFile();

		if(bRet)
			::MessageBox(NULL, L"Successed Export File", L"Info", 0);

	}
	break;
	case '4':
	{
	}
	break;
	case '5':
	{
	}
	break;
	case '6':
	{
	}
	break;

	case '1':
	{
		g_nCurrentModelIndex++;

		if (g_nCurrentModelIndex > g_nModelCount - 1)
			g_nCurrentModelIndex = g_nModelCount - 1;
	}
	break;
	case '2':
	{
		g_nCurrentModelIndex--;

		if (g_nCurrentModelIndex < 0)
			g_nCurrentModelIndex = 0;
	}
	break;
	case 'c':
	{
		g_vecModelBlock[g_nCurrentModelIndex].color[0] = 0.5f;
		g_vecModelBlock[g_nCurrentModelIndex].color[1] = 0.5f;
		g_vecModelBlock[g_nCurrentModelIndex].color[2] = 0.5f;
	}
	break;
	case 'd':
	{
		g_vecMainBlock.clear();
	}
	break;
	case SpaceBar:
	{
		// 현재 상태가 select 면 insert 모드 되면서 블럭의 위치를 조절할 수있다.
		if (g_currentInsertMode == E_MODEL_SELECT)
		{
			g_currentInsertMode = E_MODEL_INSERT;


			sLegoBlockInfo item = g_vecModelBlock[g_nCurrentModelIndex];
			item.center.x += g_fBlockWidth * 0.5f;
			item.center.z += g_fBlockWidth * 0.5f;

			item.center.y = g_boundingBox[1].y + g_fBlockHeight * 10.f;

			g_vecMainBlock.push_back(item);
		}
		else if (g_currentInsertMode == E_MODEL_INSERT)
		{
			g_currentInsertMode = E_MODEL_SELECT;

			g_nCurrentModelIndex = 0;
		}
	}
	break;
	}
}



void SpecialKeyboardEvent(int iKey, int iX, int iY)
{
	switch (iKey)
	{
	case GLUT_KEY_LEFT:
	{
		// 현재 상태가 insert mode 일경우 모델을 이동시킬 수 있다.
		if (g_currentInsertMode == E_MODEL_INSERT)
		{
			int idx = int(g_vecMainBlock.size() - 1);

			int nRotIndex = g_vecMainBlock[idx].nRotation;
			if(nRotIndex == 0)
				g_vecMainBlock[idx].center.x -= g_fBlockWidth;
			else if(nRotIndex == 1)
				g_vecMainBlock[idx].center.z -= g_fBlockWidth;
			else if (nRotIndex == 2)
				g_vecMainBlock[idx].center.x += g_fBlockWidth;
			else
				g_vecMainBlock[idx].center.z += g_fBlockWidth;

		}
	}
	break;
	case GLUT_KEY_RIGHT:
	{
		if (g_currentInsertMode == E_MODEL_INSERT)
		{
			int idx = int(g_vecMainBlock.size() - 1);

			int nRotIndex = g_vecMainBlock[idx].nRotation;
			if (nRotIndex == 0)
				g_vecMainBlock[idx].center.x += g_fBlockWidth;
			else if (nRotIndex == 1)
				g_vecMainBlock[idx].center.z += g_fBlockWidth;
			else if (nRotIndex == 2)
				g_vecMainBlock[idx].center.x -= g_fBlockWidth;
			else
				g_vecMainBlock[idx].center.z -= g_fBlockWidth;
		}
	}
	break;
	case GLUT_KEY_UP:
	{
		if (g_currentInsertMode == E_MODEL_INSERT)
		{
			int idx = int(g_vecMainBlock.size() - 1);

			int nRotIndex = g_vecMainBlock[idx].nRotation;
			if (nRotIndex == 0)
				g_vecMainBlock[idx].center.z -= g_fBlockWidth;
			else if (nRotIndex == 1)
				g_vecMainBlock[idx].center.x += g_fBlockWidth;
			else if (nRotIndex == 2)
				g_vecMainBlock[idx].center.z += g_fBlockWidth;
			else
				g_vecMainBlock[idx].center.x -= g_fBlockWidth;


		}
	}
	break;
	case GLUT_KEY_DOWN:
	{
		if (g_currentInsertMode == E_MODEL_INSERT)
		{
			int idx = int(g_vecMainBlock.size() - 1);

			int nRotIndex = g_vecMainBlock[idx].nRotation;
			if (nRotIndex == 0)
				g_vecMainBlock[idx].center.z += g_fBlockWidth;
			else if (nRotIndex == 1)
				g_vecMainBlock[idx].center.x -= g_fBlockWidth;
			else if (nRotIndex == 2)
				g_vecMainBlock[idx].center.z -= g_fBlockWidth;
			else
				g_vecMainBlock[idx].center.x += g_fBlockWidth;

		}
	}
	break;
	}
}
