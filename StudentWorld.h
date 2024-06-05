#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "Actor.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

    virtual int init();
	//{
	//	return GWSTATUS_CONTINUE_GAME;
	//}

    virtual int move();
	//{
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//	decLives();
	//	return GWSTATUS_PLAYER_DIED;
	//}

	virtual void cleanUp();

    
    bool clearIce(int clearX, int clearY);
    
    bool getIfIceExists(int x, int y);
    
    bool iceChunkExists(int x, int y);
    
    bool boulderExists(int x, int y);
    
    bool playerExists(int x, int y);
    
    IceMan* getPlayer();

	void setDisplayText();

	int getBarelsLeft(){
		return m_barrelsPerLevel;
	}
    int getGoldLeft(){
        return m_goldPerLevel;
    }
    bool isNearWhileFalling(int startX, int startY);
    
    void revealHidden(int startX, int startY);
    
    void revealHiddenSonar(int startX, int startY);
    
    std::string pickUpNear(int x, int y);
    
    bool pickUpNearProt(int x, int y);
    
	bool isNear(int, int);
    
    bool isNearBoulder(int startX, int startY, GraphObject::Direction dir);
    
	void getRandNum(int &x, int &y);

    void decreaseBarrels();
    
    void decreaseGold();

	void addSquirt(int x, int y, GraphObject::Direction dir);
	
    void decSquirt();

    void addProtester(int);

    bool isNearIceman(int x, int y);

    void damageIceman();

    void nearProtester(int x, int y);

    void annoyProtester(Protester*);
    
    bool ifSonarExists();
    
    void createProtestorGold(int x, int y);
    
    bool iceAbove(int x, int y);
    bool iceBelow(int x, int y);
    bool iceRight(int x, int y);
    bool iceLeft(int x, int y);
private:
    IceMan* player;
    Ice* iceArr[64][64];
    int m_barrelsPerLevel;
    int m_goldPerLevel;
    int m_bouldersPerLevel;
	std::vector<ActivatingObject*> pickupables;
    std::vector<Boulder*> boulders;
    std::vector<Protester*> protesters;
	std::vector<Squirt*> squirtedGun;
    int newProtesterTickCountdown;
    int newProtesterTickOriginal;
    int maxProtesterCount;
    int probabilityOfHardcore;
    int protesterCount;
};

#endif // STUDENTWORLD_H_
