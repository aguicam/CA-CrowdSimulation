#include "Particle.h"


Particle::Particle()
{
}

Particle::Particle(const float& x, const float& y, const float& z) :
m_previousPosition(0, 0, 0), m_velocity(0, 0, 0), m_force(0, 0, 0), m_bouncing(1), m_lifetime(50), m_fixed(false)
{
	m_currentPosition.x = x;
	m_currentPosition.y = y;
	m_currentPosition.z = z;

}

/*
Particle::Particle(glm::vec3 pos, glm::vec3 vel, float bouncing, bool fixed, int lifetime, glm::vec3 force) :
m_currentPosition(pos), m_previousPosition(pos), m_force(force), m_velocity(vel), m_bouncing(bouncing), m_lifetime(lifetime), m_fixed(fixed)
{
}
*/

Particle::~Particle()
{
}

//setters
void Particle::setPosition(const float& x, const float& y, const float& z)
{
	glm::vec3 pos(x,y,z);
	m_currentPosition =  pos;
}
void Particle::setPosition(glm::vec3 pos)
{
	m_currentPosition = pos;
}

void Particle::setPreviousPosition(const float& x, const float& y, const float& z)
{
	glm::vec3 pos(x, y, z);
	m_previousPosition = pos;
}

void Particle::setPreviousPosition(glm::vec3 pos)
{
	m_previousPosition = pos;
}

void Particle::setForce(const float& x, const float& y, const float& z)
{
	glm::vec3 force(x, y, z);
	m_force = force;
}

void Particle::setForce(glm::vec3 force)
{
	m_force = force;
}

void Particle::addForce(const float& x, const float& y, const float& z)
{
	glm::vec3 force(x,y,z);
	m_force += force;
}

void Particle::addForce(glm::vec3 force)
{
	m_force += force;
}

void Particle::setVelocity(const float& x, const float& y, const float& z)
{
	glm::vec3 vel(x,y,z);
	m_velocity = vel;
}

void Particle::setVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

void Particle::setBouncing(float bouncing)
{
	m_bouncing = bouncing;
}

void Particle::setLifetime(float lifetime)
{
	m_lifetime = lifetime;
}

void Particle::setFixed(bool fixed)
{
	m_fixed = fixed;
}

void Particle::setSpeed(float speed){
    m_speed=speed;
}
void Particle::setGoal(int num){
    m_goal=num;
}
void Particle::setStart(int st){
    m_start=st;
}
void Particle::setWaypoints(std::deque<int> wayp){
    waypoints=wayp;
    path=wayp;
}
void Particle::setAreaExplored(std::deque<int> wayp){
    areaExp=wayp;
}


//getters

glm::vec3 Particle::getCurrentPosition()
{
	return m_currentPosition;
}

glm::vec3 Particle::getPreviousPosition()
{
	return m_previousPosition;
}

glm::vec3 Particle::getForce()
{
	return m_force;
}

glm::vec3 Particle::getVelocity()
{
	return m_velocity;
}

float Particle::getBouncing()
{
	return m_bouncing;
}

float Particle::getLifetime()
{
	return m_lifetime;
}

bool Particle::isFixed()
{
	return m_fixed;
}
int Particle::getModelNum(){
    return m_model;
}
float Particle::getSpeed(){
    return m_speed;
}
int Particle::getGoal(){
    return m_goal;
}
int Particle::getStart(){
    return m_start;
}

std::deque<int> Particle::getsWaypoints(){
    return waypoints;
}
int Particle::getFirstWaypoint(){
    if(waypoints.size()>0){return waypoints[0];}
    else{return -1;}
}
void Particle::popWaypoint(){
    waypoints.pop_front();
}
std::deque<int> Particle::getAreaExplored(){
    return areaExp;
}
std::deque<int> Particle::getPath(){
    return path;
}


void Particle::setModelNum(int num){
    m_model=num;
}
void Particle::updateParticle(const float& dt, UpdateMethod method)
{
	if (!m_fixed & m_lifetime > 0)
	{
		switch (method)
		{
		case UpdateMethod::EulerOrig:
		{
			m_previousPosition = m_currentPosition;
			m_currentPosition += m_velocity*dt;
			m_velocity += m_force*dt;
		}
			break;
		case UpdateMethod::EulerSemi:
		{
            m_previousPosition = m_currentPosition;
            m_velocity += m_force*dt;
            m_currentPosition += m_velocity*dt;

		}
			break;
		case UpdateMethod::Verlet:
		{
            glm::vec3 prev=m_currentPosition;
            m_currentPosition += 0.99f*(m_currentPosition-m_previousPosition)+m_force*dt*dt;
            if(std::isnan(m_currentPosition.x)){
                int i=0;
            }
            m_previousPosition =prev;
            m_velocity = (m_currentPosition-m_previousPosition)/dt;

		}
			break;
		}
	}
	return;
}
