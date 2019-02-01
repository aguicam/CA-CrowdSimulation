#include "glwidget.h"
#include <iostream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <plyreader.h>
#include <random>

#define rand01() ((float)std::rand()/RAND_MAX)


using namespace std;




const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 50.0f;


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), bPolygonFill(true), angleX(45.0f), angleY(0.0f), distance(15.0f)
{
    program = NULL;
}

GLWidget::~GLWidget()
{
    if(program) delete program;
}


void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    program = new QGLShaderProgram();
    program->addShaderFromSourceFile(QGLShader::Vertex, ":/shaders/simpleshader.vert");
    program->addShaderFromSourceFile(QGLShader::Fragment, ":/shaders/simpleshader.frag");

    program->link();
    if(!program->isLinked())
    {
        cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
        QApplication::quit();
    }
    program->bind();

    mesh.buildCube();
    room.buildRoom();
    trianglesMesh.buildTriangles();
    walls.buildWalls();
    quad.buildQuad();

    if(!mesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!room.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!trianglesMesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!walls.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    if(!quad.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    BBmax =QVector3D(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    BBmin =QVector3D(FLT_MAX,FLT_MAX,FLT_MAX);
    initParticleSystem();
    //compute radius for collisions
    colRadi = max(BBmax.x()-BBmin.x(),BBmax.z()-BBmin.z())/120.0;
    std::cout<<"Radi colision: "<<colRadi<<std::endl;
    initPlanes();
    //initWalls();
    std::cout<<"MAX "<<BBmax.x()<<" "<<BBmax.y()<<" "<<BBmax.z()<<" MIN "<<BBmin.x()<<" "<<BBmin.y()<<" "<<BBmin.z()<<std::endl;
    std::cout<<"Pos imprimo "<<models.size()<<std::endl;
    std::cout<<"Pos imprimo "<<calModels.size()<<std::endl;

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    loadSphere(QString("sphere.ply"));



}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
    setProjection((float)w/h);
    setModelview();
}

void GLWidget::paintGL()
{

    renderParticleSystem();

    update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    // Rotation
    if(event->buttons() & Qt::LeftButton)
    {
        angleX += rotationFactor * (event->y() - lastMousePos.y());
        angleX = max(-maxRotationCamera, min(angleX, maxRotationCamera));
        angleY += rotationFactor * (event->x() - lastMousePos.x());
    }
    // Zoom
    if(event->buttons() & Qt::RightButton)
    {
        distance += 0.01f * (event->y() - lastMousePos.y());
        distance = max(minDistanceCamera, min(distance, maxDistanceCamera));
    }

    lastMousePos = event->pos();

    makeCurrent();
    setModelview();
    doneCurrent();
    update();
}

void GLWidget::setProjection(float aspect)
{
    QMatrix4x4 projectionMatrix;

    projectionMatrix.perspective(60, aspect, 0.01, 100.0);
    program->bind();
    program->setUniformValue("projection", projectionMatrix);
    program->release();
}

void GLWidget::setModelview()
{
    QMatrix4x4 modelviewMatrix;

    modelviewMatrix.translate(0, 0, -distance);
    modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
    program->bind();
    program->setUniformValue("modelview", modelviewMatrix);
    program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    program->release();
}

QMatrix4x4 GLWidget::getModelview()
{
    QMatrix4x4 modelviewMatrix;

    modelviewMatrix.translate(0, 0, -distance);
    modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
    return modelviewMatrix;

}

void GLWidget::setPolygonMode(bool bFill)
{
    bPolygonFill = bFill;

    makeCurrent();
    if(bFill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    doneCurrent();
    update();
}

void GLWidget::loadMesh(const QString &filename)
{
    PLYReader reader;

    mesh.destroy();
    reader.readMesh(filename, mesh);
    makeCurrent();
    if(!mesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    doneCurrent();
    update();
}
void GLWidget::loadSphere(const QString &filename)
{
    PLYReader reader;

    mesh.destroy();
    reader.readMesh(filename, mesh);
    makeCurrent();
    if(!mesh.init(program))
    {
        cout << "Could not create vbo" << endl;
        QApplication::quit();
    }
    doneCurrent();
}


void GLWidget::resetScene(){
    particles.clear();
}

//RENDERING FUNCTIONS
void GLWidget::renderParticleSystem(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();
    program->setUniformValue("bLighting", bPolygonFill);
    program->setUniformValue("color", QVector4D(1, 0.0, 0.0, 1.0));

    QMatrix4x4 model = getModelview();
    program->setUniformValue("color", QVector4D(0.8, 0.8, 0.8, 1.0));
    //QMatrix4x4 model = getModelview();
    model = getModelview();
    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    room.render(*this);

    for(uint l=0;l<grid.size();l++){
        model = getModelview();
        model.translate(grid[l].center.x,grid[l].center.y,grid[l].center.z);
        program->setUniformValue("modelview", model);
        program->setUniformValue("normalMatrix", model.normalMatrix());
        if(showPath){
            switch (grid[l].color) {
            case 0:
                program->setUniformValue("color", QVector4D(0.3, 0.3, 0.3, 1.0));
                break;
            case 1:
                program->setUniformValue("color", QVector4D(0.9, 0.0, 0.0, 1.0));
                break;
            case 2:
                program->setUniformValue("color", QVector4D(0.0, 0.9, 0.0, 1.0));
                break;
            case 3:
                program->setUniformValue("color", QVector4D(0.9, 0.9, 0.0, 1.0));
                break;
            case 4:
                program->setUniformValue("color", QVector4D(0.0, 0.0, 0.9, 1.0));
                break;
            default:
                program->setUniformValue("color", QVector4D(0.2, 0.2, 0.2, 1.0));
                break;
            }
        }
        quad.render(*this);
    }


    model = getModelview();
    program->setUniformValue("modelview", model);
    program->setUniformValue("normalMatrix", model.normalMatrix());
    program->setUniformValue("color", QVector4D(0.0, 0.0, 0.0, 1.0));
    walls.render(*this);




    //RENDER PARTICLES

    for(int k= 0;k<(int)particles.size();k++){


        model = getModelview();


        glm::vec3 prevPos=particles[k].getCurrentPosition();

        if(!stop){
        particles[k].updateParticle(dt, Particle::UpdateMethod::EulerSemi);
        }
        //Path Finding

        int w= particles[k].getFirstWaypoint();
        if(w==-1){
            particles[k].setStart(particles[k].getGoal());
            int goal = rand()%grid.size();
            while(!grid[goal].free){goal = rand()%grid.size();}
            particles[k].setGoal(goal);
            if(inAstar){computeAStar(k);}
            else{computeARAStar(k);}
            w= particles[k].getFirstWaypoint();
            if(showPath&&currentShowing==k){setShowPathAgent(k);}
        }
        if(particleInCell(k, w)){
            particles[k].popWaypoint();
            w= particles[k].getFirstWaypoint();
            if(w==-1){
                particles[k].setStart(particles[k].getGoal());
                int goal = rand()%grid.size();
                while(!grid[goal].free){goal = rand()%grid.size();}
                particles[k].setGoal(goal);
                if(inAstar){computeAStar(k);}
                else{computeARAStar(k);}
                w= particles[k].getFirstWaypoint();
                if(showPath&&currentShowing==k){setShowPathAgent(k);}
            }
        }

        glm::vec3 desiredVelocityN=glm::normalize(grid[w].center-prevPos);
        glm::vec3 prevVel=glm::normalize(particles[k].getVelocity());
        desiredVelocityN =glm::normalize(desiredVelocityN +2.0f*prevVel);
        particles[k].setVelocity(desiredVelocityN*particles[k].getSpeed());

        collisionPlane(k,prevPos);
        collisionWalls(k,prevPos,w);
        collisionAgents(k);


        glm::vec3 currentParticlePos = particles[k].getCurrentPosition();
        glm::vec3 currentVel=glm::normalize(particles[k].getVelocity());
        //currentVel=glm::normalize(desiredVelocityN + currentVel)*particles[k].getSpeed();
        //particles[k].setVelocity(currentVel);

        float angle =glm::acos(glm::dot(currentVel,glm::vec3(0.0,0.0,1.0)))*180/M_PI*glm::sign(currentVel.x);

        colorModel =  QVector4D(1.0,0.0, 0.0, 1.0);
        switch (particles[k].getModelNum()) {
        case 0:
            colorModel=QVector4D(0.5,1.0, 0.0, 1.0);
            break;
        case 1:
            colorModel=QVector4D(1.0,0.0, 1.0, 1.0);
            break;
        case 2:
            colorModel=QVector4D(0.0,1.0, 1.0, 1.0);
            break;
        case 3:
            colorModel=QVector4D(0.0,1.0, 0.0, 1.0);
            break;
        default:
            break;
        }

        model.translate(currentParticlePos.x,currentParticlePos.y,currentParticlePos.z);
        model.rotate(-90.0,1.0,0.0,0.0);
        model.rotate(angle,0.0,0.0,1.0);
        model.scale(1/60.0f);
        program->bind();
        program->setUniformValue("modelview", model);
        program->setUniformValue("normalMatrix", model.normalMatrix());
        program->setUniformValue("color", colorModel);
        for(unsigned i=0;i<models[k].size();i++){
           models[k][i]->render(*this);
        }
        if(!stop){
        calModels[k].m_calModel->update(0.01*particles[k].getSpeed());//  A VARIAR CON LA VELOCIDAD
        reMesh(k);
        }
    }

    setModelview();

    program->release();
}
//INITIALIZING FUNCTIONS
void GLWidget::initParticleSystem(){
    particles.clear();
    triangles.clear();
    spheres.clear();
    planes.clear();


    //Init Grid
    Cell c;
    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            c.id = j+20*i;
            c.center= glm::vec3(-9.5f+i,0.0f,9.5f-j);
            c.free=true;
            grid.push_back(c);
        }
    }
    for(int i=0;i<wallCells.size();i++){
        grid[wallCells[i]].free=false;
    }
    for(int i=1;i<19;i++){
        for(int j=1;j<19;j++){
            if(grid[20*i+j].free){
            std::vector<int> n;
            if(grid[20*i+j-21].free){n.push_back(20*i+j-21);}
            if(grid[20*i+j-20].free){n.push_back(20*i+j-20);}
            if(grid[20*i+j-19].free){n.push_back(20*i+j-19);}
            if(grid[20*i+j-1].free){n.push_back(20*i+j-1);}
            if(grid[20*i+j+1].free){n.push_back(20*i+j+1);}
            if(grid[20*i+j+19].free){n.push_back(20*i+j-19);}
            if(grid[20*i+j+-20].free){n.push_back(20*i+j+20);}
            if(grid[20*i+j+21].free){n.push_back(20*i+j+21);}
            grid[20*i+j].neighborCells=n;
        }
        }
    }
    for(int i=1;i<19;i++){
        if(grid[i].free){
            std::vector<int> n;
            if(grid[i-1].free){n.push_back(i-1);}
            if(grid[i+1].free){n.push_back(i+1);}
            if(grid[i+19].free){n.push_back(i+19);}
            if(grid[i+20].free){n.push_back(i+20);}
            if(grid[i+21].free){n.push_back(i+21);}
            grid[i].neighborCells=n;
        }
    }
    for(int i=1;i<19;i++){
        if(grid[20*i].free){
            std::vector<int> n;
            if(grid[20*i-20].free){n.push_back(20*i-20);}
            if(grid[20*i-19].free){n.push_back(20*i-19);}
            if(grid[20*i+1].free){n.push_back(20*i+1);}
            if(grid[20*i+20].free){n.push_back(20*i+20);}
            if(grid[20*i+21].free){n.push_back(20*i+21);}
            grid[20*i].neighborCells=n;
        }
    }
    for(int i=1;i<19;i++){
        if(grid[20*i+19].free){
            std::vector<int> n;
            if(grid[20*i-2].free){n.push_back(20*i-2);}
            if(grid[20*i-1].free){n.push_back(20*i-1);}
            if(grid[20*i+18].free){n.push_back(20*i+18);}
            if(grid[20*(i+1)+18].free){n.push_back(20*(i+1)+18);}
            if(grid[20*(i+1)+21].free){n.push_back(20*(i+1)+19);}
            grid[20*i+19].neighborCells=n;
        }
    }
    for(int i=380;i<400;i++){
        if(grid[i].free){
            std::vector<int> n;
            if(grid[i-1].free){n.push_back(i-1);}
            if(grid[i+1].free){n.push_back(i+1);}
            if(grid[i-19].free){n.push_back(i-19);}
            if(grid[i-20].free){n.push_back(i-20);}
            if(grid[i-21].free){n.push_back(i-21);}
            grid[i].neighborCells=n;
        }
    }
    grid[0].neighborCells=std::vector<int>{1,20,21};
    grid[19].neighborCells=std::vector<int>{18,38,39};
    grid[380].neighborCells=std::vector<int>{360,361,381};
    grid[399].neighborCells=std::vector<int>{378,379,398};



    //INITIALIZATION OF PARTICLES

//    initPPos=glm::vec3((rand01()-0.5), 0.0f, (rand01()-0.5));
    int id = rand()%grid.size();
    while(!grid[id].free){id =rand()%grid.size();}
    initPPos= grid[id].center;
    Particle ps(initPPos.x,initPPos.y,initPPos.z);
    p=ps;
    p.setStart(id);
    int goal = rand()%grid.size();
    while(!grid[goal].free){goal =rand()%grid.size();}
    p.setGoal(goal);
    p.setBouncing(0.5f);
    p.setVelocity(10*(rand01()-0.5), 0, 10*(rand01()-0.5));
    p.setModelNum(0);
    p.setSpeed(5*rand01()+2);
    glm::vec3 newVel= glm::normalize(p.getVelocity())*p.getSpeed();
    p.setVelocity(newVel.x,newVel.y,newVel.z);
    //p.addForce(0, -9.8f, 0);
    if(!SemiEuler){
        p.setPreviousPosition(initPPos-dt*p.getVelocity());
    }
    particles.push_back(p);
    if(inAstar){computeAStar(0);}
    else{computeARAStar(0);}
    //

    for(int i=1;i<numParticles;i++){
        //initPPos=glm::vec3((rand01()-0.5)*10, 0.0f, (rand01()-0.5)*10);
        id = rand()%grid.size();
        while(!grid[id].free){id = rand()%grid.size();}
        p.setStart(id);
        initPPos= grid[id].center;
        goal = rand()%grid.size();
        while(!grid[goal].free){goal =rand()%grid.size();}
        p.setGoal(goal);
        p.setPosition(initPPos);
        p.setVelocity(10*(rand01()-0.5), 0, 10*(rand01()-0.5));
        p.setModelNum(i%4);
        p.setSpeed(5*rand01()+2);
        newVel= glm::normalize(p.getVelocity())*p.getSpeed();
        p.setVelocity(newVel.x,newVel.y,newVel.z);
        particles.push_back(p);
        if(inAstar){computeAStar(i);}
        else{computeARAStar(i);}
    }




    //CAL3D Model init

    calModel.onInit("data/paladin.cfg");
    calModels.push_back(calModel);
    initMesh(true,calModels[0]);

    for(int i=1;i<numParticles;i++){
        Model calModel2;
        calModel2.onInit("data/paladin.cfg");
        calModel2.m_calModel->update(i*0.1);
        calModels.push_back(calModel2);
        initMesh(false,calModels[i]);
    }




}
void GLWidget::initPlanes(){
    //We define the planes

    // define a floor plane for collision
    glm::vec3 punt(0.0f);
    glm::vec3 normal(0, 1, 0);
    Plane pl(punt, normal);
    //planes.push_back(pl);
    //FrontPlane (z+)
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,10.0f-colRadi),glm::vec3(0,0,-1));
    planes.push_back(pl);
    //Back plane (z-)
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-10.0f+colRadi),glm::vec3(0,0,1));
    planes.push_back(pl);
    //Right plane (x+)
    pl.setPosAndNorm(glm::vec3(10.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    planes.push_back(pl);
    //Left plane (x-)
    pl.setPosAndNorm(glm::vec3(-10.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    planes.push_back(pl);
}
void GLWidget::initWalls(){
    //Z Walls
    Plane pl(glm::vec3(1.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    pl.isZrange=true;
    pl.wallMaxZ = 10.0f;
    pl.wallMinZ = 3.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(2.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(8.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    pl.wallMaxZ = 4.0f;
    pl.wallMinZ = -1.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(7.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    pl.wallMaxZ = 3.0f;
    pl.wallMinZ = 0.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(4.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    pl.wallMaxZ = 4.0f;
    pl.wallMinZ = 3.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(4.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    pl.wallMaxZ = -1.0f;
    pl.wallMinZ = -4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(3.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(4.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    pl.wallMaxZ = -7.0f;
    pl.wallMinZ = -10.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(3.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-1.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    pl.wallMaxZ = -1.0f;
    pl.wallMinZ = -5.0f;
    wallsP.push_back(pl);
    pl.wallMaxZ = 4.0f;
    pl.wallMinZ = 0.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-2.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    pl.wallMaxZ = 3.0f;
    pl.wallMinZ = -4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-3.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    pl.wallMaxZ = -5.0f;
    pl.wallMinZ = -7.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-4.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-5.0f+colRadi,0.0f,0.0f),glm::vec3(1,0,0));
    pl.wallMaxZ = 7.0f;
    pl.wallMinZ = 4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-6.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    pl.wallMinZ = 3.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(-7.0f-colRadi,0.0f,0.0f),glm::vec3(-1,0,0));
    pl.wallMaxZ = -4.0f;
    pl.wallMinZ = -5.0f;
    wallsP.push_back(pl);
    // X Walls
    pl.isZrange=false;
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-7.0f-colRadi),glm::vec3(0,0,-1));
    pl.wallMaxX = -3.0f;
    pl.wallMinX = -4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-7.0f+colRadi),glm::vec3(0,0,1));
    pl.wallMaxX = 4.0f;
    pl.wallMinX = 3.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-5.0f-colRadi),glm::vec3(0,0,-1));
    pl.wallMaxX = -4.0f;
    pl.wallMinX = -7.0f;
    wallsP.push_back(pl);
    pl.wallMaxX = -1.0f;
    pl.wallMinX = -3.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-4.0f-colRadi),glm::vec3(0,0,-1));
    pl.wallMaxX = 4.0f;
    pl.wallMinX = 3.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-4.0f+colRadi),glm::vec3(0,0,1));
    pl.wallMaxX = -2.0f;
    pl.wallMinX = -7.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,-1.0f-colRadi),glm::vec3(0,0,-1));
    pl.wallMaxX = 3.0f;
    pl.wallMinX = -1.0f;
    wallsP.push_back(pl);
    pl.wallMaxX = 8.0f;
    pl.wallMinX = 4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,0.0f+colRadi),glm::vec3(0,0,1));
    pl.wallMaxX = 7.0f;
    pl.wallMinX = -1.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,3.0f-colRadi),glm::vec3(0,0,-1));
    pl.wallMaxX = -2.0f;
    pl.wallMinX = -6.0f;
    wallsP.push_back(pl);
    pl.wallMaxX = 2.0f;
    pl.wallMinX = 1.0f;
    wallsP.push_back(pl);
    pl.wallMaxX = 7.0f;
    pl.wallMinX = 4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,4.0f+colRadi),glm::vec3(0,0,1));
    pl.wallMaxX = -1.0f;
    pl.wallMinX = -5.0f;
    wallsP.push_back(pl);
    pl.wallMaxX = 8.0f;
    pl.wallMinX = 4.0f;
    wallsP.push_back(pl);
    pl.setPosAndNorm(glm::vec3(0.0f,0.0f,7.0f-colRadi),glm::vec3(0,0,-1));
    pl.wallMaxX = -5.0f;
    pl.wallMinX = -6.0f;
    wallsP.push_back(pl);


}

void GLWidget::collisionWalls(int k, glm::vec3 prevPos, int w){
    int cell = -1;
    for(uint i=0;i<wallCells.size();i++){
        if(particleInCell(k, wallCells[i])){
            cell=wallCells[i];
        }
    }
    if(cell!=-1){
        glm::vec3 dir = particles[k].getCurrentPosition()-grid[cell].center;
        Plane ss(particles[k].getCurrentPosition(),-glm::normalize(dir));
        glm::vec3 interPos = particles[k].getCurrentPosition();
        glm::vec3 pointPos = ss.intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
        glm::vec3 pointVel = ss.intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
        pointVel = glm::normalize(pointVel+2.0f*dir)*particles[k].getSpeed();
        particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
        particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
        particles[k].setStart(particles[k].getGoal());

        particles[k].setStart(w);
        if(inAstar){computeAStar(k);}
        else{computeARAStar(k);}
    }
}

void GLWidget::collisionPlane(int k, glm::vec3 prevPos){
    for(int i=0;i<(int)planes.size();i++){
        float angle =glm::acos(glm::dot(particles[k].getVelocity(),planes[i].normal))*180/M_PI;

        if(planes[i].distPoint2Plane(particles[k].getCurrentPosition())*planes[i].distPoint2Plane(prevPos)< 0&&angle>90){

            glm::vec3 interPos = particles[k].getCurrentPosition();
            glm::vec3 pointPos = planes[i].intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
            glm::vec3 pointVel = planes[i].intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
            pointVel = glm::normalize(pointVel)*particles[k].getSpeed();
            particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
            particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
        }
    }
}
void GLWidget::collisionTriangle(int k, glm::vec3 prevPos){
    for(int i=0;i<(int)triangles.size();i++){
        if (triangles[i].distPoint2Plane(particles[k].getCurrentPosition())*triangles[i].distPoint2Plane(prevPos)< 0) {
            // std::cout<<" Current position: "<<particles[k].getCurrentPosition().x <<" "<<particles[k].getCurrentPosition().y <<" "<<particles[k].getCurrentPosition().z <<std::endl;
            glm::vec3 interPos = particles[k].getCurrentPosition();
            glm::vec3 pointPos = triangles[i].intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
            glm::vec3 pointVel = triangles[i].intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
            if(triangles[i].isInsideArea(triangles[i].intersecSegmentP(particles[k].getPreviousPosition(),particles[k].getCurrentPosition()))){
                particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
                particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
            }
        }
    }
}
void GLWidget::collisionSphere(int k, glm::vec3 prevPos){
    for(int i=0;i<(int)spheres.size();i++){

        if(spheres[i].isInside(particles[k].getCurrentPosition())){
            Plane tangPlane(spheres[i].intersectingPointSphere(prevPos,particles[k].getCurrentPosition()),glm::normalize(spheres[i].intersectingPointSphere(prevPos,particles[k].getCurrentPosition())-spheres[i].center));
            glm::vec3 interPos = particles[k].getCurrentPosition();
            glm::vec3 pointPos = tangPlane.intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
            glm::vec3 pointVel = tangPlane.intersecVelocity(interPos,particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
            particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
            particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
        }
    }
}

//void GLWidget::updateMesh(){



//}
void GLWidget::initMesh(bool init, Model model){
    std::vector<TriangleMesh*> agent;
    CalRenderer *pCalRenderer;
    pCalRenderer = model.m_calModel->getRenderer();
    int meshCount;
    meshCount = pCalRenderer->getMeshCount();
    TriangleMesh *subAgent;
    for(int meshId = 0; meshId < meshCount; meshId++)
    {
        // get the number of submeshes
        int submeshCount;
        submeshCount = pCalRenderer->getSubmeshCount(meshId);

        // render all submeshes of the mesh
        int submeshId;
        for(submeshId = 0; submeshId < submeshCount; submeshId++)
        {
            // select mesh and submesh for further data access
            if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
            {

                // get the transformed vertices of the submesh
                static float meshVertices[30000][3];
                int vertexCount;
                vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

                //          // get the transformed normals of the submesh
                //          static float meshNormals[30000][3];
                //          pCalRenderer->getNormals(&meshNormals[0][0]);

                //          // get the texture coordinates of the submesh
                //          static float meshTextureCoordinates[30000][2];
                //          int textureCoordinateCount;
                //          textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);

                // get the faces of the submesh
                static CalIndex meshFaces[50000][3];
                int faceCount;
                faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);


                subAgent= new TriangleMesh();

                for(int i=0;i<vertexCount;i++){
                    subAgent->addVertex(QVector3D(meshVertices[i][0],meshVertices[i][1],meshVertices[i][2]));
                    if(init){
                    if(meshVertices[i][0]<BBmin[0]){BBmin[0]=meshVertices[i][0];}
                    else if(meshVertices[i][1]<BBmin[1]){BBmin[1]=meshVertices[i][1];}
                    else if(meshVertices[i][2]<BBmin[2]){BBmin[2]=meshVertices[i][2];}
                    else if(meshVertices[i][2]>BBmax[2]){BBmax[2]=meshVertices[i][2];}
                    else if(meshVertices[i][1]>BBmax[1]){BBmax[1]=meshVertices[i][1];}
                    else if(meshVertices[i][0]>BBmax[0]){BBmax[0]=meshVertices[i][0];}
                    }
                }
                for(int i=0;i<faceCount;i++){
                    subAgent->addTriangle(meshFaces[i][0],meshFaces[i][1],meshFaces[i][2]);
                }
                if(!subAgent->init(program))
                {
                    cout << "Could not create vbo" << endl;
                    QApplication::quit();
                }
                agent.push_back(subAgent);
            }
        }

    }
    models.push_back(agent);
}

void GLWidget::reMesh(int ind){

//    for (TriangleMesh *agent_ : agent)
//    {
//        agent_->destroy();
//        delete agent_;
//    }

    for (TriangleMesh *agent_ : models[ind])
    {
        agent_->destroy();
        delete agent_;
    }

    models[ind].clear();
    std::vector<TriangleMesh*> agent;
    CalRenderer *pCalRenderer;
    pCalRenderer = calModels[ind].m_calModel->getRenderer();
    int meshCount;
    meshCount = pCalRenderer->getMeshCount();
    TriangleMesh *subAgent;
    for(int meshId = 0; meshId < meshCount; meshId++)
    {
        // get the number of submeshes
        int submeshCount;
        submeshCount = pCalRenderer->getSubmeshCount(meshId);

        // render all submeshes of the mesh
        int submeshId;
        for(submeshId = 0; submeshId < submeshCount; submeshId++)
        {
            // select mesh and submesh for further data access
            if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
            {

                // get the transformed vertices of the submesh
                static float meshVertices[30000][3];
                int vertexCount;
                vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

                //          // get the transformed normals of the submesh
                //          static float meshNormals[30000][3];
                //          pCalRenderer->getNormals(&meshNormals[0][0]);

                //          // get the texture coordinates of the submesh
                //          static float meshTextureCoordinates[30000][2];
                //          int textureCoordinateCount;
                //          textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);

                // get the faces of the submesh
                static CalIndex meshFaces[50000][3];
                int faceCount;
                faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);


                subAgent= new TriangleMesh();

                for(int i=0;i<vertexCount;i++){
                    subAgent->addVertex(QVector3D(meshVertices[i][0],meshVertices[i][1],meshVertices[i][2]));
                }
                for(int i=0;i<faceCount;i++){
                    subAgent->addTriangle(meshFaces[i][0],meshFaces[i][1],meshFaces[i][2]);
                }
                if(!subAgent->init(program))
                {
                    cout << "Could not create vbo" << endl;
                    QApplication::quit();
                }
                agent.push_back(subAgent);
            }
        }

    }
    models[ind]=agent;
}
void GLWidget::changeStop(){
    stop =!stop;
}

void GLWidget::collisionAgents(int k){

    for(unsigned int i=0;i<particles.size();i++){
        if(k!=i){
            glm::vec3 dir=particles[i].getCurrentPosition()-particles[k].getCurrentPosition();
            glm::vec3 dirP =glm::normalize(glm::vec3(particles[i].getVelocity().x,particles[i].getVelocity().y,-particles[i].getVelocity().z))*particles[k].getSpeed();
            float dist = glm::length(dir);
            if(dist<2*colRadi){
                glm::vec3 origVel=particles[k].getVelocity();
                glm::vec3 dirN=glm::normalize(dir)*particles[k].getSpeed();
                particles[k].setVelocity(dirN.x,dirN.y,dirN.z);

                Plane wall(particles[k].getCurrentPosition(),-glm::normalize(dir));
                glm::vec3 pointPos = wall.intersecPos(particles[k].getCurrentPosition(),particles[k].getBouncing());
                glm::vec3 pointVel = wall.intersecVelocity(particles[k].getCurrentPosition(),particles[k].getVelocity(),particles[k].getBouncing(),!SemiEuler,dt);
                pointVel = glm::normalize(pointVel)*particles[k].getSpeed();

                pointVel=glm::normalize(origVel+pointVel+2.0f*dirP)*particles[k].getSpeed();

                particles[k].setPosition(pointPos.x, pointPos.y, pointPos.z);
                particles[k].setVelocity(pointVel.x,pointVel.y, pointVel.z);
            }

        }
    }

}


void GLWidget::computeAStar(int k){
    for(uint i =0;i<grid.size();i++){
        grid[i].inClosed=false;
        grid[i].inOpen=false;
    }
    int goal = particles[k].getGoal();
    int start = particles[k].getStart();
    std::cout<<"start: "<<start<<std::endl;
    std::cout<<"goal: "<<goal<<std::endl;

    std::deque<Cell> open;
    std::deque<Cell> closed;

    open.push_back(grid[start]);
    grid[start].inOpen=true;

    while(open[0].id!=goal){

        Cell current= open[0];

        open.pop_front();
        grid[current.id].inOpen=false;

        closed.push_back(current);
        grid[current.id].inClosed=true;

        std::vector<int> curr_neighs = grid[current.id].neighborCells;
        for(unsigned int i=0;i<curr_neighs.size();i++){
            float cost= current.g+movementCost(current.id,curr_neighs[i]);

            if(grid[curr_neighs[i]].inOpen&&cost<grid[curr_neighs[i]].g){
                grid[curr_neighs[i]].inOpen=false;

            }
            if(grid[curr_neighs[i]].inClosed&&cost<grid[curr_neighs[i]].g){
                grid[curr_neighs[i]].inClosed=false;
            }

            if(!grid[curr_neighs[i]].inClosed&&!grid[curr_neighs[i]].inOpen){
                grid[curr_neighs[i]].g=cost;
                grid[curr_neighs[i]].h=heristics(curr_neighs[i],goal);
                grid[curr_neighs[i]].fn=cost+grid[curr_neighs[i]].h;
                grid[curr_neighs[i]].parent = current.id;
                open.push_back(grid[curr_neighs[i]]);
                grid[curr_neighs[i]].inOpen=true;
                sort(open.begin(),open.end(),[](const Cell& a, const Cell& b) {
                    return a.fn < b.fn;
                });

            }

        }

    }

    std::cout<<"A* computed"<<std::endl;
    std::deque<int> path;
    Cell currentCell = open[0];
    path.push_back(goal);
    while(start!=currentCell.id){
        int parentN = currentCell.parent;
        path.push_back(parentN);
        currentCell=grid[parentN];
    }
    std::reverse(path.begin(),path.end());
    particles[k].setWaypoints(path);
    std::deque<int> cc;
    for(uint i=0;i<closed.size();i++){
        cc.push_back(closed[i].id);
    }
    for(uint i=0;i<open.size();i++){
        cc.push_back(open[i].id);
    }
    particles[k].setAreaExplored(cc);



}
void GLWidget::computeARAStar(int k){
    e=2.5;
    for(uint i =0;i<grid.size();i++){
        grid[i].inClosed=false;
        grid[i].inOpen=false;
        grid[i].h=heristics(i,particles[k].getGoal());
    }
    int goal = particles[k].getGoal();
    int start = particles[k].getStart();
    std::cout<<"start: "<<start<<std::endl;
    std::cout<<"goal: "<<goal<<std::endl;

    openA.clear();
    closedA.clear();
    inconsA.clear();
    grid[goal].g=FLT_MAX/2;
    grid[start].g=0;

    grid[start].fn = FValue(start);
    openA.push_back(grid[start]);
    grid[start].inOpen=true;
    ImprovePath(goal);

    float minF=FLT_MAX;
    for(uint i=0;i<openA.size();i++){
        if(minF> grid[openA[i].id].h+grid[openA[i].id].g){ minF=grid[openA[i].id].h+grid[openA[i].id].g;}
    }
    for(uint i=0;i<inconsA.size();i++){
        if(minF> grid[inconsA[i].id].h+grid[inconsA[i].id].g){ minF=grid[inconsA[i].id].h+grid[inconsA[i].id].g;}
    }
    ep = min(e,grid[goal].g/minF);
    //Publish ?
    while(ep>1.0){
        e-=0.5;

        for(uint i=0;i<inconsA.size();i++){
            openA.push_back(inconsA[i]);
            grid[inconsA[i].id].inIncons =false;
            grid[inconsA[i].id].inOpen=true;
        }
        inconsA.clear();
        sort(openA.begin(),openA.end(),[](const Cell& a, const Cell& b) {
            return a.fn < b.fn;
        });
        closedA.clear();
        for(uint i =0;i<grid.size();i++){
            grid[i].inClosed=false;
        }
        ImprovePath(goal);
        minF=FLT_MAX;
        for(uint i=0;i<openA.size();i++){
            if(minF> grid[openA[i].id].h+grid[openA[i].id].g){ minF=grid[openA[i].id].h+grid[openA[i].id].g;}
        }
        for(uint i=0;i<inconsA.size();i++){
            if(minF> grid[inconsA[i].id].h+grid[inconsA[i].id].g){ minF=grid[inconsA[i].id].h+grid[inconsA[i].id].g;}
        }
        ep = min(e,grid[goal].g/minF);
        //Publish?

    }

    std::cout<<"ARA* computed"<<std::endl;
    std::deque<int> path;
    Cell currentCell = grid[goal];

    path.push_back(goal);
    while(start!=currentCell.id){
        int parentN = currentCell.parent;
        path.push_back(parentN);
        currentCell=grid[parentN];
    }
    std::reverse(path.begin(),path.end());
    particles[k].setWaypoints(path);
    std::deque<int> cc;
    for(uint i=0;i<closedA.size();i++){
        cc.push_back(closedA[i].id);
    }
    for(uint i=0;i<openA.size();i++){
        cc.push_back(openA[i].id);
    }
    for(uint i=0;i<inconsA.size();i++){
        cc.push_back(inconsA[i].id);
    }
    particles[k].setAreaExplored(cc);

}
void GLWidget::ImprovePath(int goal){


    while (FValue(goal)>grid[openA[0].id].fn) {
        Cell current= openA[0];
        openA.pop_front();
        grid[current.id].inOpen=false;

        closedA.push_back(current);
        grid[current.id].inClosed=true;

        std::vector<int> curr_neighs = grid[current.id].neighborCells;
        for(uint i=0;i<curr_neighs.size();i++){
            if(!grid[curr_neighs[i]].inClosed&&!grid[curr_neighs[i]].inOpen&!grid[curr_neighs[i]].inIncons){
                grid[curr_neighs[i]].g=FLT_MAX;
            }
            if(grid[curr_neighs[i]].g>grid[current.id].g+movementCost(current.id,curr_neighs[i])){
                grid[curr_neighs[i]].g=grid[current.id].g+movementCost(current.id,curr_neighs[i]);
                grid[curr_neighs[i]].parent=current.id;
                if(!grid[curr_neighs[i]].inClosed){
                    grid[curr_neighs[i]].fn=FValue(curr_neighs[i]);
                    grid[curr_neighs[i]].inOpen=true;
                    openA.push_back(grid[curr_neighs[i]]);
                }else{
                    grid[curr_neighs[i]].fn=FValue(curr_neighs[i]);
                    grid[curr_neighs[i]].inIncons=true;
                    inconsA.push_back(grid[curr_neighs[i]]);
                }

            }
        }
        sort(openA.begin(),openA.end(),[](const Cell& a, const Cell& b) {
            return a.fn < b.fn;
        });
    }

}

float GLWidget::FValue(int s){
    return grid[s].g+e*grid[s].h;
}

float GLWidget::movementCost(int cur, int ne){
    if(cur%20==ne%20 || (cur-cur%20)/20==(ne-ne%20)/20){ return 10.0f;}
    else{return 14.0f;}

}
float GLWidget::heristics(int ne,int goal){
    return abs(goal%20-ne%20)+abs((goal-goal%20)/20-(ne-ne%20)/20);

}
bool GLWidget::particleInCell(int k, int c){
    bool x = particles[k].getCurrentPosition().x<grid[c].center.x+0.5 &&particles[k].getCurrentPosition().x>grid[c].center.x-0.5;
    bool z = particles[k].getCurrentPosition().z<grid[c].center.z+0.5 &&particles[k].getCurrentPosition().z>grid[c].center.z-0.5;
    return x&&z;
}

void GLWidget::setShowPathAgent(int num){

    for(uint i =0;i<grid.size();i++){
        grid[i].color=0;
        if(!grid[i].free){grid[i].color=5;}
    }
    if(num==-1){
        showPath=false;
    }
    else{
        showPath=true;
        if(num>=numParticles){num=numParticles-1;}
        std::deque<int> c = particles[num].getAreaExplored();
        for(uint i=0;i<c.size();i++){
            grid[c[i]].color=4;
        }
         c = particles[num].getPath();
        for(uint i=0;i<c.size();i++){
            grid[c[i]].color=3;
        }
        grid[particles[num].getGoal()].color=2;
        grid[particles[num].getStart()].color=1;

    }
    for(uint i =0;i<grid.size();i++){
        if(!grid[i].free){grid[i].color=5;}
    }
    currentShowing=num;
}
void GLWidget::setAstar(){
    inAstar=!inAstar;
    std::cout<<" a* "<<inAstar<<std::endl;
}
