#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
#include <random>
class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, double size, unsigned int depth, StudentWorld* stuWorld);
	virtual ~Actor();
	virtual void doSomething() = 0;
	StudentWorld* getWorld();

private:
	StudentWorld* world;
};

class Agent : public Actor {
public:
	Agent(int imageID, int startX, int startY, double size, unsigned int depth, StudentWorld* stuWorld);
};

class Ice : public Actor {
public:
	Ice(int startX, int startY, StudentWorld* world);
	~Ice();
	void doSomething();
};

class IceMan : public Agent {
public:
	IceMan(int startX, int startY, StudentWorld* world);
	~IceMan();
	void doSomething();
	int getHealth();
	int getSquirts();
	int getGold();
	int getSonar();
	void decSquirt();
	void getsAnnoyed();
private:
    int health = 5;
    int squirts = 5;
    int gold = 0;
    int sonar = 1;
    int m_lives;
};

class Protester : public Agent {
public:
	Protester(int IID_PLAYER, StudentWorld* world);
	virtual void setDead() = 0;
	virtual bool getDead() = 0;
	virtual void getAnnoyed() = 0;
};

class RegularProtester : public Protester {
public:
	RegularProtester(int currentLevel, StudentWorld* world);
	virtual void doSomething();
	virtual bool isNearIceman();
	virtual bool isFacing();
	virtual bool lineOfSight(int& dir);
	virtual bool iceExistsLineOfSightX(int proX, int icemanX, int y, int& dirToMove);
	virtual bool iceExistsLineOfSightY(int proX, int icemanX, int y, int& dirToMove);
	virtual void setDead();
	virtual bool getDead();
	virtual void getAnnoyed();
    void setWait(int set);
    int getWait();
    void decWait();
private:
    int waitTilNext = 0;
	bool leaveField = false;
	int health = 5;
	int ticksToWaitBetweenMoves; //number of ticks to wait before protester is allowed to move
	int waitCount; //counter for the number of ticks to wait before protester is allowed to move
	bool rest;
	int nonRestingTickShoutWait = 15; //number of non resting ticks a protester is allowed to shout
	int nonRestingTickShoutWaitCount; //counter for the number of non resting ticks a protester is allowed to shout
	bool dead = false;
};

class HardcoreProtester : public Protester {
public:
	HardcoreProtester(int currentLevel, StudentWorld* world);
	virtual void doSomething();
	virtual bool isNearIceman();
	virtual bool isFacing();
	virtual bool lineOfSight(int& dir);
	virtual bool iceExistsLineOfSightX(int proX, int icemanX, int y, int& dirToMove);
	virtual bool iceExistsLineOfSightY(int proX, int icemanX, int y, int& dirToMove);
	virtual void setDead();
	virtual bool getDead();
	virtual void getAnnoyed();
    void setWait(int set);
    int getWait();
    void decWait();
private:
    int waitTilNext = 0;
	bool leaveField = false;
	int health = 5;
	int ticksToWaitBetweenMoves; //number of ticks to wait before protester is allowed to move
	int waitCount; //counter for the number of ticks to wait before protester is allowed to move
	bool rest;
	int nonRestingTickShoutWait = 15; //number of non resting ticks a protester is allowed to shout
	int nonRestingTickShoutWaitCount; //counter for the number of non resting ticks a protester is allowed to shout
	bool dead = false;
};

class Boulder : public Actor {
private:
	bool m_alive = true;
	bool m_falling = false;
	int m_fallWait = 60;
	bool m_soundHasPlayed = false;
public:
	Boulder(int startX, int startY, StudentWorld* stuWorld);
	virtual void doSomething();
	void setDead();
	bool isAlive();
	bool isFalling();
	bool iceExistUnder();
};

class Squirt : public Actor {
public:
	Squirt(int startX, int startY, Direction dir, StudentWorld* stuWorld);
	virtual void doSomething();
	bool getIsDead() const;
	void setDead(bool set);
private:
	int travelDistance = 4;
	bool isClear(GraphObject::Direction dir);
	void moveSquirt();
	bool m_isDead = false;
};


class ActivatingObject : public Actor {
private:
    bool isPickupPlayer = true;
    bool isPickupProtestor = false;
    bool m_alive = true;
    std::string m_whatType;
    int m_tickUntilDeath;
    
public:
    ActivatingObject(int imageID, int startX, int startY, double size, unsigned int depth, StudentWorld* stuWorld, std::string type);
    virtual ~ActivatingObject(){};
    std::string getType();
    void setDead();
    bool isAlive();
    void setTickUntilDeath(int time);
    int getTickUntilDeath();
    void decreaseTickDeath();
    void setPickupablePlayer(bool set);
    void setPickupableProtestor(bool set);
    bool getPickupablePlayer();
    bool getPickupableProtestor();
    
};

class OilBarrel : public ActivatingObject {
public:
	OilBarrel(int startX, int startY, StudentWorld* stuWorld);
	virtual void doSomething();
};

class GoldNugget : public ActivatingObject {
public:
	GoldNugget(int startX, int startY, StudentWorld* stuWorld);
	virtual void doSomething();
};

class SonarKit : public ActivatingObject {
public:
    SonarKit(int startX, int startY, StudentWorld* stuWorld);
    virtual void doSomething();
};

class WaterPool : public ActivatingObject {
public:
    WaterPool(int startX, int startY, StudentWorld* stuWorld);
    virtual void doSomething();
};
#endif // ACTOR_H_
