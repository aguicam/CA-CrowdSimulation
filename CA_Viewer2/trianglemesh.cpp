#include <iostream>
#include "trianglemesh.h"


using namespace std;


struct CornerEdge
{
	int vertexA, vertexB, corner;

	bool operator<(const CornerEdge &cEdge) { return (vertexA < cEdge.vertexA) || ((vertexA == cEdge.vertexA) && (vertexB < cEdge.vertexB)); }
	bool operator==(const CornerEdge &cEdge) { return (vertexA == cEdge.vertexA) && (vertexB == cEdge.vertexB); }
};


int next(int corner)
{
	return 3 * (corner / 3) + (corner + 1) % 3;
}

int previous(int corner)
{
	return 3 * (corner / 3) + (corner + 2) % 3;
}


TriangleMesh::TriangleMesh() : vboVertices(QOpenGLBuffer::VertexBuffer),
										 vboNormals(QOpenGLBuffer::VertexBuffer),
										 eboTriangles(QOpenGLBuffer::IndexBuffer)
{
}


void TriangleMesh::addVertex(const QVector3D &position)
{
	vertices.push_back(position);
}

void TriangleMesh::addTriangle(int v0, int v1, int v2)
{
	triangles.push_back(v0);
	triangles.push_back(v1);
	triangles.push_back(v2);
}

void TriangleMesh::buildCube()
{
	GLfloat vertices[] = {-1, -1, -1,
								  1, -1, -1,
								  1,  1, -1,
								 -1,  1, -1,
								 -1, -1,  1,
								  1, -1,  1,
								  1,  1,  1,
								 -1,  1,  1
								};

	GLuint faces[] = {3, 1, 0, 3, 2, 1,
							5, 6, 7, 4, 5, 7,
							7, 3, 0, 0, 4, 7,
							1, 2, 6, 6, 5, 1,
							0, 1, 4, 5, 4, 1,
							2, 3, 7, 7, 6, 2
						  };

	int i;

	for(i=0; i<8; i+=1)
		addVertex(0.5f * QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
	for(i=0; i<12; i++)
		addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}


void TriangleMesh::buildQuad(){

    GLfloat vertices[] = {-0.5f, 0.0f, 0.5f,
                          0.5f, 0.0f, 0.5f,
                          0.5f, 0.0f, -0.5f,
                          -0.5f, 0.0f, -0.5f
                                };

    GLuint faces[] = {0,1,2,0,2,3};

    int i;

    for(i=0; i<4; i+=1)
        addVertex(QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));//0.5f
    for(i=0; i<2; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

void TriangleMesh::buildRoom(){

    GLfloat vertices[] = {-10.0f, 0.0f, -10.0f,
                                  10.0f, 0.0f, -10.0f,
                                  10.0f, 10.0f/3, -10.0f,
                                 -10.0f, 10.0f/3, -10.0f,
                                 -10.0f, 0.0f,  10.0f,
                                  10.0f, 0.0f,  10.0f,
                                  10.0f, 10.0f/3,  10.0f,
                                 -10.0f, 10.0f/3,  10.0f

                                };

    GLuint faces[] = {0,1,3,1,2,3,
                            7, 6, 5, 7, 5, 4,
                            0, 3, 7, 7, 4, 0,
                            6, 2, 1, 1, 5, 6,
                          };

    int i;

    for(i=0; i<8; i+=1)
        addVertex(QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));//0.5f
    for(i=0; i<8; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}
void TriangleMesh::buildWalls(){

    GLfloat vertices[] = {1.25f, 0.0f, 3.25f, //0
                          1.25f, 0.0f, 10.0f, //1
                          1.25f, 1.5f, 10.0f, //2
                          1.25f, 1.5f, 3.25f, //3
                          1.75f, 0.0f, 3.25f, //4
                          1.75f, 1.5f, 3.25f, //5
                          1.75f, 0.0f, 10.0f, //6
                          1.75f, 1.5f, 10.0f, //7
                          3.25f, 0.0f, -10.0f, //8
                          3.25f, 0.0f, -7.25f, //9
                          3.25f, 1.5f, -7.25f, //10
                          3.25f, 1.5f, -10.0f, //11
                          3.75f, 0.0f, -7.25f, //12
                          3.75f, 1.5f, -7.25f, //13
                          3.75f, 0.0f, -10.0f, //14
                          3.75f, 1.5f, -10.0f, //15
                          -5.75f, 0.0f, 6.75f, //16
                          -5.75f, 1.5f, 6.75f, //17
                          -5.25f, 0.0f, 6.75f, //18
                          -5.25f, 1.5f, 6.75f, //19
                          -5.25f, 0.0f, 3.75f, //20
                          -5.25f, 1.5f, 3.75f, //21
                          -1.25f, 0.0f, 3.75f, //22
                          -1.25f, 1.5f, 3.75f, //23
                          -1.25f, 0.0f, -0.25f, //24
                          -1.25f, 1.5f, -0.25f, //25
                          7.25f, 0.0f, -0.25f, //26
                          7.25f, 1.5f, -0.25f, //27
                          7.25f, 0.0f, 3.25f, //28
                          7.25f, 1.5f, 3.25f, //29
                          4.25f, 0.0f, 3.25f, //30
                          4.25f, 1.5f, 3.25f, //31
                          4.25f, 0.0f, 3.75f, //32
                          4.25f, 1.5f, 3.75f, //33
                          7.75f, 0.0f, 3.75f, //34
                          7.75f, 1.5f, 3.75f, //35
                          7.75f, 0.0f, -0.75f, //36
                          7.75f, 1.5f, -0.75f, //37
                          3.75f, 0.0f, -0.75f, //38////
                          3.75f, 1.5f, -0.75f, //39
                          3.75f, 0.0f, -3.75f, //40
                          3.75f, 1.5f, -3.75f, //41
                          3.25f, 0.0f, -3.75f, //42
                          3.25f, 1.5f, -3.75f, //43
                          3.25f, 0.0f, -0.75f, //44
                          3.25f, 1.5f, -0.75f, //45
                          -1.25f, 0.0f, -0.75f, //46
                          -1.25f, 1.5f, -0.75f, //47
                          -1.25f, 0.0f, -4.75f, //48
                          -1.25f, 1.5f, -4.75f, //49
                          -3.25f, 0.0f, -4.75f, //50
                          -3.25f, 1.5f, -4.75f, //51
                          -3.25f, 0.0f, -6.75f, //52
                          -3.25f, 1.5f, -6.75f, //53
                          -3.75f, 0.0f, -6.75f, //54
                          -3.75f, 1.5f, -6.75f, //55
                          -3.75f, 0.0f, -4.75f, //56
                          -3.75f, 1.5f, -4.75f, //57
                          -6.75f, 0.0f, -4.75f, //58
                          -6.75f, 1.5f, -4.75f, //59
                          -6.75f, 0.0f, -4.25f, //60
                          -6.75f, 1.5f, -4.25f, //61
                          -1.75f, 0.0f, -4.25f, //62
                          -1.75f, 1.5f, -4.25f, //63
                          -1.75f, 0.0f, 3.25f, //64
                          -1.75f, 1.5f, 3.25f, //65
                          -5.75f, 0.0f, 3.25f, //66
                          -5.75f, 1.5f, 3.25f, //67

                                };

    GLuint faces[] = {0, 1, 2, 0, 2, 3,
                      4, 0, 3, 4, 3, 5,
                      6, 4, 5, 6, 5, 7,
                      5, 3, 2, 5, 2, 7,
                      8, 9, 10, 8, 10, 11,
                      9, 12, 13, 9, 13, 10,
                      12, 14, 15, 12, 15, 13,
                      10, 13, 15, 10, 15, 11,
                      16, 18, 19, 16, 19, 17,
                      18, 20, 21, 18, 21, 19,
                      20, 22, 23, 20, 23, 21,
                      22, 24, 25, 22, 25, 23, //12
                      24, 26, 27, 24, 27, 25,
                      26, 28, 29, 26, 29, 27,
                      28, 30, 31, 28, 31, 29,
                      30, 32, 33, 30, 33, 31,
                      32, 34, 35, 32, 35, 33,
                      34, 36, 37, 34, 37, 35,
                      36, 38, 39, 36, 39, 37,
                      38, 40, 41, 38, 41, 39,
                      40, 42, 43, 40, 43, 41,
                      42, 44, 45, 42, 45, 43, //22
                      44, 46, 47, 44, 47, 45,
                      46, 48, 49, 46, 49, 47,
                      48, 50, 51, 48, 51, 49,
                      50, 52, 53, 50, 53, 51,
                      52, 54, 55, 52, 55, 53,
                      54, 56, 57, 54, 57, 55,
                      56, 58, 59, 56, 59, 57,
                      58, 60, 61, 58, 61, 59,
                      60, 62, 63, 60, 63, 61,
                      62, 64, 65, 62, 65, 63,
                      64, 66, 67, 64, 67, 65,
                      66, 16, 17, 66, 17, 67, //34
                      17, 19, 21, 17, 21, 67,
                      21, 23, 65, 21, 65, 67,
                      23, 49, 63, 23, 63, 65,
                      61, 63, 49, 61, 49, 59,
                      57, 51, 53, 57, 53, 55,
                      25, 27, 37, 25, 37, 47,
                      27, 29, 35, 27, 35, 37,
                      33, 35, 29, 33, 29, 31,
                      45, 39, 41, 45, 41, 43  //43
                          };

    int i;

    for(i=0; i<68; i+=1)
        addVertex(QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));//0.5f
    for(i=0; i<86; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}
//void TriangleMesh::buildWalls(){

//    GLfloat vertices[] = {1.0f, 0.0f, 3.0f, //0
//                          1.0f, 0.0f, 10.0f, //1
//                          1.0f, 1.5f, 10.0f, //2
//                          1.0f, 1.5f, 3.0f, //3
//                          2.0f, 0.0f, 3.0f, //4
//                          2.0f, 1.5f, 3.0f, //5
//                          2.0f, 0.0f, 10.0f, //6
//                          2.0f, 1.5f, 10.0f, //7
//                          3.0f, 0.0f, -10.0f, //8
//                          3.0f, 0.0f, -7.0f, //9
//                          3.0f, 1.5f, -7.0f, //10
//                          3.0f, 1.5f, -10.0f, //11
//                          4.0f, 0.0f, -7.0f, //12
//                          4.0f, 1.5f, -7.0f, //13
//                          4.0f, 0.0f, -10.0f, //14
//                          4.0f, 1.5f, -10.0f, //15
//                          -6.0f, 0.0f, 7.0f, //16
//                          -6.0f, 1.5f, 7.0f, //17
//                          -5.0f, 0.0f, 7.0f, //18
//                          -5.0f, 1.5f, 7.0f, //19
//                          -5.0f, 0.0f, 4.0f, //20
//                          -5.0f, 1.5f, 4.0f, //21
//                          -1.0f, 0.0f, 4.0f, //22
//                          -1.0f, 1.5f, 4.0f, //23
//                          -1.0f, 0.0f, 0.0f, //24
//                          -1.0f, 1.5f, 0.0f, //25
//                          7.0f, 0.0f, 0.0f, //26
//                          7.0f, 1.5f, 0.0f, //27
//                          7.0f, 0.0f, 3.0f, //28
//                          7.0f, 1.5f, 3.0f, //29
//                          4.0f, 0.0f, 3.0f, //30
//                          4.0f, 1.5f, 3.0f, //31
//                          4.0f, 0.0f, 4.0f, //32
//                          4.0f, 1.5f, 4.0f, //33
//                          8.0f, 0.0f, 4.0f, //34
//                          8.0f, 1.5f, 4.0f, //35
//                          8.0f, 0.0f, -1.0f, //36
//                          8.0f, 1.5f, -1.0f, //37
//                          4.0f, 0.0f, -1.0f, //38
//                          4.0f, 1.5f, -1.0f, //39
//                          4.0f, 0.0f, -4.0f, //40
//                          4.0f, 1.5f, -4.0f, //41
//                          3.0f, 0.0f, -4.0f, //42
//                          3.0f, 1.5f, -4.0f, //43
//                          3.0f, 0.0f, -1.0f, //44
//                          3.0f, 1.5f, -1.0f, //45
//                          -1.0f, 0.0f, -1.0f, //46
//                          -1.0f, 1.5f, -1.0f, //47
//                          -1.0f, 0.0f, -5.0f, //48
//                          -1.0f, 1.5f, -5.0f, //49
//                          -3.0f, 0.0f, -5.0f, //50
//                          -3.0f, 1.5f, -5.0f, //51
//                          -3.0f, 0.0f, -7.0f, //52
//                          -3.0f, 1.5f, -7.0f, //53
//                          -4.0f, 0.0f, -7.0f, //54
//                          -4.0f, 1.5f, -7.0f, //55
//                          -4.0f, 0.0f, -5.0f, //56
//                          -4.0f, 1.5f, -5.0f, //57
//                          -7.0f, 0.0f, -5.0f, //58
//                          -7.0f, 1.5f, -5.0f, //59
//                          -7.0f, 0.0f, -4.0f, //60
//                          -7.0f, 1.5f, -4.0f, //61
//                          -2.0f, 0.0f, -4.0f, //62
//                          -2.0f, 1.5f, -4.0f, //63
//                          -2.0f, 0.0f, 3.0f, //64
//                          -2.0f, 1.5f, 3.0f, //65
//                          -6.0f, 0.0f, 3.0f, //66
//                          -6.0f, 1.5f, 3.0f, //67

//                                };

//    GLuint faces[] = {0, 1, 2, 0, 2, 3,
//                      4, 0, 3, 4, 3, 5,
//                      6, 4, 5, 6, 5, 7,
//                      5, 3, 2, 5, 2, 7,
//                      8, 9, 10, 8, 10, 11,
//                      9, 12, 13, 9, 13, 10,
//                      12, 14, 15, 12, 15, 13,
//                      10, 13, 15, 10, 15, 11,
//                      16, 18, 19, 16, 19, 17,
//                      18, 20, 21, 18, 21, 19,
//                      20, 22, 23, 20, 23, 21,
//                      22, 24, 25, 22, 25, 23, //12
//                      24, 26, 27, 24, 27, 25,
//                      26, 28, 29, 26, 29, 27,
//                      28, 30, 31, 28, 31, 29,
//                      30, 32, 33, 30, 33, 31,
//                      32, 34, 35, 32, 35, 33,
//                      34, 36, 37, 34, 37, 35,
//                      36, 38, 39, 36, 39, 37,
//                      38, 40, 41, 38, 41, 39,
//                      40, 42, 43, 40, 43, 41,
//                      42, 44, 45, 42, 45, 43, //22
//                      44, 46, 47, 44, 47, 45,
//                      46, 48, 49, 46, 49, 47,
//                      48, 50, 51, 48, 51, 49,
//                      50, 52, 53, 50, 53, 51,
//                      52, 54, 55, 52, 55, 53,
//                      54, 56, 57, 54, 57, 55,
//                      56, 58, 59, 56, 59, 57,
//                      58, 60, 61, 58, 61, 59,
//                      60, 62, 63, 60, 63, 61,
//                      62, 64, 65, 62, 65, 63,
//                      64, 66, 67, 64, 67, 65,
//                      66, 16, 17, 66, 17, 67, //34
//                      17, 19, 21, 17, 21, 67,
//                      21, 23, 65, 21, 65, 67,
//                      23, 49, 63, 23, 63, 65,
//                      61, 63, 49, 61, 49, 59,
//                      57, 51, 53, 57, 53, 55,
//                      25, 27, 37, 25, 37, 47,
//                      27, 29, 35, 27, 35, 37,
//                      33, 35, 29, 33, 29, 31,
//                      45, 39, 41, 45, 41, 43  //43
//                          };

//    int i;

//    for(i=0; i<68; i+=1)
//        addVertex(QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));//0.5f
//    for(i=0; i<86; i++)
//        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
//}



void TriangleMesh::buildFloor(){

    std::vector<GLfloat> vert;

    for(int i=0;i<21;i++){
        for(int j=0;j<21;j++){
            vert.push_back(-10.0f+i);
            vert.push_back(0.0);
            vert.push_back(10.0f-j);
        }
    }



    std::vector<GLuint> faces;

    for(int i=0;i<20;i++){
        for(int j=0;j<20;j++){
            faces.push_back(j+21*i);
            faces.push_back(j+21*(i+1));
            faces.push_back(j+21*(i+1)+1);
            faces.push_back(j+21*i);
            faces.push_back(j+21*(i+1)+1);
            faces.push_back(j+21*i+1);
        }
    }

    int i;

    for(i=0; i<441; i+=1)
        addVertex(QVector3D(vert[3*i], vert[3*i+1], vert[3*i+2]));
    for(i=0; i<800; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

void TriangleMesh::buildTriangles(){

    GLfloat vertices[] = {
//        -6.0f, 0.0f, -12.0f,
//                          -20.0f, 0.0f, 4.0f,
//                          -12.0f,  12.0f, -12.0f,
                          -3.0f, 0.0f, -6.0f,
                          -10.0f, 0.0f, 2.0f,
                          -12.0f,  12.0f, -12.0f,
                          -5.0f, 8.0f, 1.0f,
                          -10.0f, 10.0f, 4.0f,
                          -3.0f,  2.0f, 8.0f

                                };

    GLuint faces[] = {0,2,1 , 3,4,5
                          };

    int i;

    for(i=0; i<6; i+=1)
        addVertex(0.5f * QVector3D(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
    for(i=0; i<2; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}


//bool TriangleMesh::init(QOpenGLShaderProgram *program)
bool TriangleMesh::init(QGLShaderProgram *program)
{
	vector<QVector3D> replicatedVertices, normals;
	vector<unsigned int> perFaceTriangles;

	buildReplicatedVertices(replicatedVertices, normals, perFaceTriangles);

	program->bind();

	vao.destroy();
	vao.create();
	if(vao.isCreated())
		vao.bind();
	else
		return false;

	vboVertices.destroy();
	vboVertices.create();
	if(vboVertices.isCreated())
		vboVertices.bind();
	else
		return false;
	vboVertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(0);
	program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

	vboNormals.destroy();
	vboNormals.create();
	if(vboNormals.isCreated())
		vboNormals.bind();
	else
		return false;
	vboNormals.setUsagePattern(QOpenGLBuffer::StaticDraw);
	program->enableAttributeArray(1);
	program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

	eboTriangles.destroy();
	eboTriangles.create();
	if(eboTriangles.isCreated())
		eboTriangles.bind();
	else
		return false;
	eboTriangles.setUsagePattern(QOpenGLBuffer::StaticDraw);

    fillVBOs(replicatedVertices, normals, perFaceTriangles);

	vao.release();
	program->release();

	return true;
}

void TriangleMesh::destroy()
{
	vao.destroy();
	vboVertices.destroy();
	vboNormals.destroy();
	eboTriangles.destroy();

	vertices.clear();
	triangles.clear();
}

void TriangleMesh::render(QOpenGLFunctions &gl)
{

	vao.bind();
	eboTriangles.bind();
    gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);
   // gl.glDrawArrays(GL_TRIANGLES,0,3*triangles.size());
	vao.release();
}

void TriangleMesh::buildReplicatedVertices(vector<QVector3D> &replicatedVertices, vector<QVector3D> &normals, vector<unsigned int> &perFaceTriangles)
{
	normals.resize(triangles.size());

	for(unsigned int i=0; i<triangles.size(); i+=3)
	{
		replicatedVertices.push_back(vertices[triangles[i]]);
		replicatedVertices.push_back(vertices[triangles[i+1]]);
		replicatedVertices.push_back(vertices[triangles[i+2]]);

		QVector3D N = QVector3D::crossProduct(vertices[triangles[i+1]] - vertices[triangles[i]], vertices[triangles[i+2]] - vertices[triangles[i]]);
		N.normalize();
		normals[i] = N;
		normals[i+1] = N;
		normals[i+2] = N;

		perFaceTriangles.push_back(perFaceTriangles.size());
		perFaceTriangles.push_back(perFaceTriangles.size());
		perFaceTriangles.push_back(perFaceTriangles.size());
	}

}

void TriangleMesh::fillVBOs(vector<QVector3D> &replicatedVertices, vector<QVector3D> &normals, vector<unsigned int> &perFaceTriangles)
{
	vboVertices.bind();
	vboVertices.allocate(&replicatedVertices[0], 3 * sizeof(float) * replicatedVertices.size());
	vboVertices.release();

	vboNormals.bind();
	vboNormals.allocate(&normals[0], 3 * sizeof(float) * normals.size());
	vboNormals.release();

	eboTriangles.bind();
	eboTriangles.allocate(&perFaceTriangles[0], sizeof(int) * perFaceTriangles.size());
	eboTriangles.release();
}

void TriangleMesh::updateVertices(){

    vector<QVector3D> replicatedVertices, normals;
        vector<unsigned int> perFaceTriangles;
        normals.resize(triangles.size());

       // vertices = newVertices;

        for(unsigned int i=0; i<triangles.size(); i+=3)
        {
            replicatedVertices.push_back(vertices[triangles[i]]);
            replicatedVertices.push_back(vertices[triangles[i+1]]);
            replicatedVertices.push_back(vertices[triangles[i+2]]);

            QVector3D N = QVector3D::crossProduct(vertices[triangles[i+1]] - vertices[triangles[i]], vertices[triangles[i+2]] - vertices[triangles[i]]);
            N.normalize();
            normals[i] = N;
            normals[i+1] = N;
            normals[i+2] = N;

            perFaceTriangles.push_back(perFaceTriangles.size());
            perFaceTriangles.push_back(perFaceTriangles.size());
            perFaceTriangles.push_back(perFaceTriangles.size());
        }

        vboVertices.bind();
        vboVertices.write(0, &replicatedVertices[0], 3 * sizeof(float) * replicatedVertices.size());
        vboVertices.release();

        vboNormals.bind();
        vboNormals.write(0, &normals[0], 3 * sizeof(float) * normals.size());
        vboNormals.release();

        eboTriangles.bind();
        eboTriangles.write(0, &perFaceTriangles[0], sizeof(int) * perFaceTriangles.size());
        eboTriangles.release();
}


void TriangleMesh::setVertices(vector<QVector3D> vert){
    vertices=vert;
}
void TriangleMesh::setTriangles(vector<int> tri){
    triangles =tri;
}

