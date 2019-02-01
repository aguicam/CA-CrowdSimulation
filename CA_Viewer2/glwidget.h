#ifndef GLWIDGET_H
#define GLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "trianglemesh.h"
#include <QGLShaderProgram>
#include <GeometryP.h>
#include <Particle.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <QOpenGLTimerQuery>
#include<modelC.h>
#include <queue>
#include <algorithm>


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

    struct Cell{
        int id;
        glm::vec3 center;
        bool free;
        std::vector<int> neighborCells;
        int parent=-1;
        float fn=0.0;
        float g=0.0;
        float h=0.0;
        bool inOpen=false;
        bool inClosed=false;
        bool inIncons=false;
        int color=0; //0 nothing , 1 start, 2 goal, 3 path, 4 search area,
    };

    struct Node{
        int cell;
        int parent=-1;
        float fn=0.0;
        float g=0.0;
        float h=0.0;
    };

public:
	GLWidget(QWidget *parent);
	~GLWidget();

	void loadMesh(const QString &filename);
    void loadSphere(const QString &filename);
	void setPolygonMode(bool bFill);


protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void setProjection(float aspect);
	void setModelview();
    QMatrix4x4 getModelview();
    void resetParticle(int k);

    void renderParticleSystem();
    void renderParticleSpring();
    void initParticleSystem();
    void collisionSphere(int k, glm::vec3 prevPos);
    void collisionPlane(int k, glm::vec3 prevPos);
    void collisionTriangle(int k, glm::vec3 prevPos);
    void initMesh(bool init, Model model);
    void reMesh(int ind);
    void collisionAgents(int k);
    void initPlanes();
    void initWalls();
    void collisionWalls(int k, glm::vec3 prevPos,int w);
    void computeAStar(int k);
    bool compareNodes(Node node1,Node node2);
    float heristics(int ne,int goal);
    float movementCost(int cur, int ne);
    bool particleInCell(int k, int c);
    void computeARAStar(int k);
    float FValue(int s);
    void ImprovePath(int goal);



private:
	bool bPolygonFill;
    float angleX, angleY, distance;
    QPoint lastMousePos;


   //QOpenGLShaderProgram *program;
    QGLShaderProgram *program;
	TriangleMesh mesh;

    TriangleMesh room;
    TriangleMesh trianglesMesh;
    TriangleMesh walls,quad;

    std::vector<std::vector<TriangleMesh*>> models;

    std::vector<Cell> grid;
    bool SemiEuler=true;

    //Particles
    Particle p;
    std::vector<Particle> particles;
    int numParticles = 5;
    glm::vec3 initPPos;


    float dt = 0.03f;
    //Planes
    std::vector<Plane> planes;
    std::vector<Plane> wallsP;
    float colRadi = 0.0;

    std::vector<int> wallCells{74, 83,84,85,86,94,106,114,126,134,135,136,146,154,166,167,168,169,170,171,172,173,174,
                     190,210,220,221,222,223,224,225,226,230,250,270,271,272,273,277,278,279,286,290,306,310,
                     326,330,346,347,348,349,350};


    std::deque<Cell> openA;
    std::deque<Cell> closedA;
    std::deque<Cell> inconsA;
    float e=2.5;
    float ep;
    bool inAstar=false;
    //Spheres
    std::vector<Sphere> spheres;

    //Triangles
    std::vector<TriangleP> triangles;

    bool showPath=false;



   //CAL3D
     Model calModel;
    std::vector<Model> calModels;
    QVector3D BBmin,BBmax;

    bool stop=false;

     QVector4D colorModel;


    int currentShowing;
public slots:
    void resetScene();
    void changeSolver(bool b);
    void changeFountain(bool b);
    void changeStop();
    void setShowPathAgent(int num);
    void setAstar();



};

#endif // GLWIDGET_H
