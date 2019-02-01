#ifndef _PARTICLE_H
#define _PARTICLE_H

#include <glm\glm.hpp>
#include <QMatrix4x4>
#include <math.h>
#include <queue>

class Particle
{
public:
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
        int color=0; //0 nothing , 1 start, 2 goal, 3 path, 4 search area,
    };

	enum class UpdateMethod : std::int8_t { EulerOrig, EulerSemi, Verlet };

	Particle();
	Particle(const float& x, const float& y, const float& z);
//	Particle(glm::vec3 pos, glm::vec3 vel, float bouncing = 1.0f, bool fixed = false, int lifetime = -1, glm::vec3 force = glm::vec3(0, 0, 0));
	~Particle();
	//setters
	void setPosition(const float& x, const float& y, const float& z);
	void setPosition(glm::vec3 pos);
	void setPreviousPosition(const float& x, const float& y, const float& z);
	void setPreviousPosition(glm::vec3 pos);
	void setVelocity(const float& x, const float& y, const float& z);
	void setVelocity(glm::vec3 vel);
	void setForce(const float& x, const float& y, const float& z);
	void setForce(glm::vec3 force);
	void setBouncing(float bouncing);
	void setLifetime(float lifetime);
	void setFixed(bool fixed);
    void setModelNum(int num);
    void setSpeed(float speed);
    void setGoal(int num);
    void setStart(int st);
    void setWaypoints(std::deque<int> wayp);
    void setAreaExplored(std::deque<int> wayp);



	//getters
	glm::vec3 getCurrentPosition();
	glm::vec3 getPreviousPosition();
	glm::vec3 getForce();
	glm::vec3 getVelocity();
    float getSpeed();
    int getStart();

	float getBouncing();
	float getLifetime();
	bool isFixed();
    int getModelNum();
    int getGoal();
    std::deque<int> getsWaypoints();
    std::deque<int> getAreaExplored();
    int getFirstWaypoint();
    void popWaypoint();
    std::deque<int> getPath();

	//other
	void addForce(glm::vec3 force);
	void addForce(const float& x, const float& y, const float& z);
	void updateParticle(const float& dt, UpdateMethod method = UpdateMethod::EulerOrig);

  //  QVector4D color;
private:
	glm::vec3 m_currentPosition;
	glm::vec3 m_previousPosition;
	glm::vec3 m_force;
	glm::vec3 m_velocity;

	float m_bouncing;
	float m_lifetime;
	bool  m_fixed;
    int m_model=0;
    float m_speed=1.0;
    int m_goal=0;
    int m_start=0;
    std::deque<int> waypoints;
    std::deque<int> path;
    std::deque<int> areaExp;

};

#endif
