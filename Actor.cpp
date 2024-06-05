#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(int imageID, int startX, int startY, double size, unsigned int depth, StudentWorld* stuWorld) : GraphObject(imageID, startX, startY, GraphObject::right, size, depth)
{
	world = stuWorld;
	setVisible(true);
	// I don't think its a good thing to set default values of visible to true at least for
	// base class since there are object we don't want to show until certain points ex. oil barrels
};

Actor::~Actor() {}

StudentWorld* Actor::getWorld() {
	return world;
}

Agent::Agent(int imageID, int startX, int startY, double size, unsigned int depth, StudentWorld* stuWorld) : Actor(imageID, startX, startY, size, depth, stuWorld) {}

Ice::Ice(int startX, int startY, StudentWorld* world) : Actor(IID_ICE, startX, startY, 0.25, 3, world)
{
	//setVisible(true);
	// I called the setVisible in the StudentWorld class after I created the object but for objects
	// that show as soon as they're created it might be better to just set visible during construction
};

Ice::~Ice() {}

void Ice::doSomething() {};

IceMan::IceMan(int startX, int startY, StudentWorld* world) : Agent(IID_PLAYER, startX, startY, 1, 2, world)
{

};

IceMan::~IceMan() {}

void IceMan::doSomething() {
	int keyPress;

	//if a  is pressed key press
	//  if a specific key is pressed and the player is in bounds of play area
	//      point player model in direction of keypress
	//      move one space in direction of keypress
	//      if (clears ice in 4x4 chunk from players starting coords clears non nullptr value)
	//          play dig sound

    if (getWorld()->getKey(keyPress) == true) {
        if (keyPress == KEY_PRESS_DOWN && getY() >= 1 && !(getWorld()->isNearBoulder(getX(), getY(), GraphObject::down))) {
            setDirection(GraphObject::down);
            moveTo(getX(), getY() - 1);
            if (getWorld()->clearIce(getX(), getY()))
                getWorld()->playSound(SOUND_DIG);
        }
        if (keyPress == KEY_PRESS_UP && getY() <= 59 && !(getWorld()->isNearBoulder(getX(), getY(), GraphObject::up))) {
            setDirection(GraphObject::up);
            moveTo(getX(), getY() + 1);
            if (getWorld()->clearIce(getX(), getY()))
                getWorld()->playSound(SOUND_DIG);
        }
        if (keyPress == KEY_PRESS_RIGHT && getX() <= 59 && !(getWorld()->isNearBoulder(getX(), getY(), GraphObject::right))) {
            setDirection(GraphObject::right);
            moveTo(getX() + 1, getY());
            if (getWorld()->clearIce(getX(), getY()))
                getWorld()->playSound(SOUND_DIG);
        }
        if (keyPress == KEY_PRESS_LEFT && getX() >= 1 && !(getWorld()->isNearBoulder(getX(), getY(), GraphObject::left))) {
            setDirection(GraphObject::left);
            moveTo(getX() - 1, getY());
            if (getWorld()->clearIce(getX(), getY()))
                getWorld()->playSound(SOUND_DIG);
        }
        if (keyPress == KEY_PRESS_SPACE) {
            if (squirts > 0) {
                getWorld()->addSquirt(getX(), getY(), getDirection());
                decSquirt();
            }
        }
        if (keyPress == 'z' || keyPress == 'Z')
            if (sonar > 0){
                getWorld()->revealHiddenSonar(getX(), getY());
                getWorld()->playSound(SOUND_SONAR);
                sonar--;
            }
        if (keyPress == KEY_PRESS_ESCAPE){
            health = 0;
        }
        if(keyPress == KEY_PRESS_TAB){
            if (gold > 0){
                gold--;
                GoldNugget* gold = new GoldNugget(getX(), getY(), getWorld());
                getWorld()->createProtestorGold(getX(), getY());
            }
        }
    }
	if (getWorld()->isNearWhileFalling(getX(), getY())) {
		health = 0;
	}

	getWorld()->revealHidden(getX(), getY());

	std::string test = getWorld()->pickUpNear(getX(), getY());
	if (test == "oil") {
		getWorld()->decreaseBarrels();
		getWorld()->increaseScore(1000);
		getWorld()->playSound(SOUND_FOUND_OIL);
	}
	else if (test == "gold") {
		gold++;
		getWorld()->increaseScore(10);
		getWorld()->playSound(SOUND_GOT_GOODIE);
	}
    else if (test == "sonar"){
        sonar += 2;
        getWorld()->increaseScore(75);
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
    else if (test == "water"){
        squirts += 5;
        getWorld()->increaseScore(100);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        }
};

int IceMan::getHealth() {
	return health;
}

int IceMan::getSquirts() {
	return squirts;
}

int IceMan::getGold() {
	return gold;
}

int IceMan::getSonar() {
	return sonar;
}

void IceMan::decSquirt() {
	squirts--;
}

void IceMan::getsAnnoyed() {
	health = health - 1;
}

Protester::Protester(int IID_PLAYER, StudentWorld* world) : Agent(IID_PLAYER, 60, 60, 1, 0, world) {
	setDirection(left);
}

RegularProtester::RegularProtester(int currentLevel, StudentWorld* world) : Protester(IID_PROTESTER, world) {
	ticksToWaitBetweenMoves = max(0, int(3 - currentLevel / 4));
	waitCount = ticksToWaitBetweenMoves;
	nonRestingTickShoutWaitCount = nonRestingTickShoutWait;
	rest = true;
}
void RegularProtester::doSomething() {
    
    if(getWait() > 0){
        if(getWait() == 1){
            setDead();
            return;
        }
        decWait();
        return;
    }
    
    if (health <= 0) {
        setDead();
        return;
    }
    if (waitCount != 0) {
        //cout << "Hardcore Protester is RESTING" << endl;
        waitCount--;
        return;
    }
    
    
    if (getWorld()->pickUpNearProt(getX(), getY())){
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        setWait(75);
    }
    
	else {
        
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> randX(0, 3);
        int r = randX(generator);
        
        if(r == 0){
            if(getY() < 60){
                if(getWorld()->iceAbove(getX(), getY()) == false){
                    moveTo(getX(), getY()+1);
                }
            }
        }
        
        if(r == 1){
            if(getY() >= 0){
                if(getWorld()->iceBelow(getX(), getY()) == false){
                    moveTo(getX(), getY()-1);
                }
            }
        }
        
        if(r == 2){
            if(getX() < 60){
                if(getWorld()->iceRight(getX(), getY()) == false){
                    moveTo(getX()+1, getY());
                }
            }
        }
        
        if(r == 3){
            if(getY() >= 0){
                if(getWorld()->iceLeft(getX(), getY()) == false){
                    moveTo(getX()-1, getY());
                }
            }
        }
        
		//cout << "Regular Protester is doing something" << endl;
		//if the protester is <= 4 units away
		if (isNearIceman() && nonRestingTickShoutWaitCount <= 0) {
			cout << "\n Regular Protester is shouting" << endl;
			cout << "Player health: " << getWorld()->getPlayer()->getHealth() << endl;
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->damageIceman();
			nonRestingTickShoutWaitCount = nonRestingTickShoutWait;
			return;
		}
		//if the protester is > 4 units away AND has a line of sight
		int dirToMove = 0;
		//dirToMove Left=0, Right=1, Up=2, Down=3
		if (!isNearIceman() && lineOfSight(dirToMove)) {
			if (dirToMove == 0) {
				this->moveTo(getX() - 1, getY());
			}
			if (dirToMove == 1) {
				this->moveTo(getX() + 1, getY());
			}
			if (dirToMove == 2) {
				this->moveTo(getX(), getY() + 1);
			}
			if (dirToMove == 3) {
				this->moveTo(getX(), getY() - 1);
			}
		}
		waitCount = ticksToWaitBetweenMoves;
		nonRestingTickShoutWaitCount--;
	}
	//cout << "non resting tick shout wait count: " << nonRestingTickShoutWaitCount << endl;
}

bool RegularProtester::isFacing() {
	int icemanX = getWorld()->getPlayer()->getX();
	int icemanY = getWorld()->getPlayer()->getY();
	if ((getDirection() == left) && (icemanX <= getX()) && (icemanY == getY())) {
		return true;
	}
	if ((getDirection() == right) && (icemanX >= getX() && (icemanY == getY()))) {
		return true;
	}
	if ((getDirection() == up) && (icemanX == getX() && (icemanY >= getY()))) {
		return true;
	}
	if ((getDirection() == down) && (icemanX == getX() && (icemanY <= getY()))) {
		return true;
	}
    return false;
}

bool RegularProtester::isNearIceman() {
	if (getWorld()->isNearIceman(getX(), getY()) && isFacing()) {
		return true;
	}
    return false;
}

bool RegularProtester::lineOfSight(int& dirToMove) {
	int icemanY = getWorld()->getPlayer()->getY();
	int icemanX = getWorld()->getPlayer()->getX();
    if ((getY() == icemanY) && (iceExistsLineOfSightX(getX(), icemanY, getY(), dirToMove))) {
        if ((getX() == icemanX) && (iceExistsLineOfSightY(getY(), icemanY, getX(), dirToMove))) {
            return true;
        }
    }
    return false;
}

bool RegularProtester::iceExistsLineOfSightX(int proX, int icemanX, int y, int& dirToMove) {
	//if protester is to the right of the iceman
	if (proX > icemanX) {
		for (int i = icemanX; i < proX; i++) {
			if (getWorld()->getIfIceExists(i, y)) {
				return false;
			}
		}
		dirToMove = 0;
		return true;
	}
	//if the protester is to the left of the iceman
	if (icemanX > proX) {
		for (int i = proX; i < icemanX; i++) {
			if (getWorld()->getIfIceExists(i, y)) {
				return false;
			}
		}
		dirToMove = 1;
		return true;
	}
    return false;
}

bool RegularProtester::iceExistsLineOfSightY(int proY, int icemanY, int x, int& dirToMove) {
	if (icemanY > proY) {
		for (int i = proY; i < icemanY; i++) {
			if (getWorld()->getIfIceExists(x, i)) {
				return false;
			}
		}
		dirToMove = 2;
		return true;
	}
	if (proY > icemanY) {
		for (int i = icemanY; i < proY; i++) {
			if (getWorld()->getIfIceExists(x, i)) {
				return false;
			}
		}
		dirToMove = 3;
		return true;
	}
    return false;
}

void RegularProtester::setDead() {
	dead = true;
	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	getWorld()->increaseScore(100);
}

bool RegularProtester::getDead() {
	return dead;
}

void RegularProtester::getAnnoyed() {
	health = health - 1;
	if (health > 0) {
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	}
}

HardcoreProtester::HardcoreProtester(int currentLevel, StudentWorld* world) : Protester(IID_HARD_CORE_PROTESTER, world) {
	ticksToWaitBetweenMoves = max(0, int(3 - currentLevel / 4));
	waitCount = ticksToWaitBetweenMoves;
	nonRestingTickShoutWaitCount = nonRestingTickShoutWait;
	rest = true;
}
void HardcoreProtester::doSomething() {
    
    if(getWait() > 0){
        decWait();
        return;
    }
    
	if (health <= 0) {
		setDead();
		return;
	}
	if (waitCount != 0) {
		//cout << "Hardcore Protester is RESTING" << endl;
		waitCount--;
		return;
	}
    
    
    if (getWorld()->pickUpNearProt(getX(), getY())){
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        setWait(500);
    }
    
	else {
        
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> randX(0, 3);
        int r = randX(generator);
        
        if(r == 0){
            if(getY() < 60){
                if(getWorld()->iceAbove(getX(), getY()) == false){
                    moveTo(getX(), getY()+1);
                }
            }
        }
        
        if(r == 1){
            if(getY() >= 0){
                if(getWorld()->iceBelow(getX(), getY()) == false){
                    moveTo(getX(), getY()-1);
                }
            }
        }
        
        if(r == 2){
            if(getX() < 60){
                if(getWorld()->iceRight(getX(), getY()) == false){
                    moveTo(getX()+1, getY());
                }
            }
        }
        
        if(r == 3){
            if(getY() >= 0){
                if(getWorld()->iceLeft(getX(), getY()) == false){
                    moveTo(getX()-1, getY());
                }
            }
        }
        
		//cout << "Regular Protester is doing something" << endl;
		//if the protester is <= 4 units away
		if (isNearIceman() && nonRestingTickShoutWaitCount <= 0) {
			cout << "\n Regular Protester is shouting" << endl;
			cout << "Player health: " << getWorld()->getPlayer()->getHealth() << endl;
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->damageIceman();
			nonRestingTickShoutWaitCount = nonRestingTickShoutWait;
			return;
		}
		//if the protester is > 4 units away AND has a line of sight
		int dirToMove = 0;
		//dirToMove Left=0, Right=1, Up=2, Down=3
		if (!isNearIceman() && lineOfSight(dirToMove)) {
			if (dirToMove == 0) {
				this->moveTo(getX() - 1, getY());
			}
			if (dirToMove == 1) {
				this->moveTo(getX() + 1, getY());
			}
			if (dirToMove == 2) {
				this->moveTo(getX(), getY() + 1);
			}
			if (dirToMove == 3) {
				this->moveTo(getX(), getY() - 1);
			}
		}
		waitCount = ticksToWaitBetweenMoves;
		nonRestingTickShoutWaitCount--;
	}
}

void RegularProtester::setWait(int set){
    waitTilNext = set;
}

int RegularProtester::getWait(){
    return waitTilNext;
}

void RegularProtester::decWait(){
    waitTilNext--;
}

bool HardcoreProtester::isNearIceman() {
	if (getWorld()->isNearIceman(getX(), getY()) && isFacing()) {
		return true;
	}
    return false;
}

bool HardcoreProtester::isFacing() {
	int icemanX = getWorld()->getPlayer()->getX();
	int icemanY = getWorld()->getPlayer()->getY();
	if ((getDirection() == left) && (icemanX <= getX()) && (icemanY == getY())) {
		return true;
	}
	if ((getDirection() == right) && (icemanX >= getX() && (icemanY == getY()))) {
		return true;
	}
	if ((getDirection() == up) && (icemanX == getX() && (icemanY >= getY()))) {
		return true;
	}
	if ((getDirection() == down) && (icemanX == getX() && (icemanY <= getY()))) {
		return true;
	}
    return false;
}

bool HardcoreProtester::lineOfSight(int& dirToMove) {
	int icemanY = getWorld()->getPlayer()->getY();
	int icemanX = getWorld()->getPlayer()->getX();
	if ((getY() == icemanY) && (iceExistsLineOfSightX(getX(), icemanY, getY(), dirToMove))) {
		return true;
	}
	if ((getX() == icemanX) && (iceExistsLineOfSightY(getY(), icemanY, getX(), dirToMove))) {
		return true;
	}
    return false;
}

bool HardcoreProtester::iceExistsLineOfSightX(int proX, int icemanX, int y, int& dirToMove) {
	//if protester is to the right of the iceman
	if (proX > icemanX) {
		for (int i = icemanX; i < proX; i++) {
			if (getWorld()->getIfIceExists(i, y)) {
				return false;
			}
		}
		dirToMove = 0;
		return true;
	}
	//if the protester is to the left of the iceman
	if (icemanX > proX) {
		for (int i = proX; i < icemanX; i++) {
			if (getWorld()->getIfIceExists(i, y)) {
				return false;
			}
		}
		dirToMove = 1;
		return true;
	}
    return false;
}

bool HardcoreProtester::iceExistsLineOfSightY(int proY, int icemanY, int x, int& dirToMove) {
	if (icemanY > proY) {
		for (int i = proY; i < icemanY; i++) {
			if (getWorld()->getIfIceExists(x, i)) {
				return false;
			}
		}
		dirToMove = 2;
		return true;
	}
	if (proY > icemanY) {
		for (int i = icemanY; i < proY; i++) {
			if (getWorld()->getIfIceExists(x, i)) {
				return false;
			}
		}
		dirToMove = 3;
		return true;
	}
    return false;
}

void HardcoreProtester::setDead() {
	dead = true;
	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	getWorld()->increaseScore(250);
}

bool HardcoreProtester::getDead() {
	return dead;
}

void HardcoreProtester::getAnnoyed() {
	health = health - 1;
	if (health > 0) {
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	}
}

void HardcoreProtester::setWait(int set){
    waitTilNext = set;
}

int HardcoreProtester::getWait(){
    return waitTilNext;
}

void HardcoreProtester::decWait(){
    waitTilNext--;
}

Boulder::Boulder(int startX, int startY, StudentWorld* stuWorld) : Actor(IID_BOULDER, startX, startY, 1, 2, stuWorld) {
	stuWorld->clearIce(startX, startY);
}

void Boulder::setDead() {
	m_alive = false;
}

bool Boulder::isAlive() {
	return m_alive;
}

bool Boulder::isFalling() {
	return m_falling;
}

bool Boulder::iceExistUnder() {
	for (int i = 0; i <= 3; i++) {
		if (getWorld()->getIfIceExists(getX() + i, getY() - 1) == true) {
			return true;
		}
	}
	return false;
}
void Boulder::doSomething() {
	if (!(iceExistUnder())) {
		m_falling = true;
	}
	if (m_falling == true) {
		if (m_fallWait > 0) {
			m_fallWait--;
		}
		else {
			if (m_soundHasPlayed == false) {
				getWorld()->playSound(SOUND_FALLING_ROCK);
				m_soundHasPlayed = true;
			}
			if (iceExistUnder() || getY() == 0) {
				setDead();
			}
			else {
				moveTo(getX(), getY() - 1);
			}
		}
	}
}

Squirt::Squirt(int startX, int startY, Direction dir, StudentWorld* stuWorld) : Actor(IID_WATER_SPURT, startX, startY, 1, 1, stuWorld) {
	setVisible(true);
	setDirection(dir);
}
bool Squirt::getIsDead() const {
	return m_isDead;
}
void Squirt::setDead(bool set) {
	m_isDead = set;
}
bool Squirt::isClear(GraphObject::Direction dir) {
	if (dir == GraphObject::right) {
		for (int i = 1; i <= 2; i++) {
			if (getWorld()->getIfIceExists(getX() + i, getY()) || getWorld()->getIfIceExists(getX() + i, getY() + 1)) {
				return false;
			}
		}
	}
	if (dir == GraphObject::left) {
		for (int i = 1; i <= 2; i++) {
			if (getWorld()->getIfIceExists(getX() - i, getY()) || getWorld()->getIfIceExists(getX() - i, getY() + 1)) {
				return false;
			}
		}
	}
	if (dir == GraphObject::up) {
		for (int i = 1; i <= 2; i++) {
			if (getWorld()->getIfIceExists(getX(), getY() + i) || getWorld()->getIfIceExists(getX() + 1, getY() + i)) {
				return false;
			}
		}
	}
	if (dir == GraphObject::down) {
		for (int i = 1; i <= 2; i++) {
			if (getWorld()->getIfIceExists(getX(), getY() - i) || getWorld()->getIfIceExists(getX() + 1, getY() - i)) {
				return false;
			}
		}
	}
	return true;
}
void Squirt::moveSquirt() {
	if (!isClear(getDirection())) {
		getWorld()->playSound(SOUND_PLAYER_SQUIRT);
		setDead(true);
	}
	if (travelDistance >= 0) {
		if (getDirection() == GraphObject::up) {
			moveTo(getX(), getY() + 1);
			getWorld()->nearProtester(getX(), getY());
			travelDistance--;
			if (travelDistance == 0) {
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				setDead(true);
				return;
			}
			return;
		}
		if (getDirection() == GraphObject::down) {
			moveTo(getX(), getY() - 1);
			getWorld()->nearProtester(getX(), getY());
			travelDistance--;
			if (travelDistance == 0) {
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				setDead(true);
				return;
			}
			return;
		}
		if (getDirection() == GraphObject::right) {
			moveTo(getX() + 1, getY());
			getWorld()->nearProtester(getX(), getY());
			travelDistance--;
			if (travelDistance == 0) {
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				setDead(true);
				return;
			}
			return;
		}
		if (getDirection() == GraphObject::left) {
			moveTo(getX() - 1, getY());
			getWorld()->nearProtester(getX(), getY());
			travelDistance--;
			if (travelDistance == 0) {
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				setDead(true);
				return;
			}
			return;
		}
	}
}
void Squirt::doSomething() {
	if (getIsDead()) {
		return;
	}
	moveSquirt();
}

ActivatingObject::ActivatingObject(int imageID, int startX, int startY, double size, unsigned int depth, StudentWorld* stuWorld, std::string type) : Actor(imageID, startX, startY, size, depth, stuWorld) {
	m_whatType = type;
}

std::string ActivatingObject::getType() {
	return m_whatType;
}

void ActivatingObject::setDead() {
	m_alive = false;
}

bool ActivatingObject::isAlive() {
	return m_alive;
}

void ActivatingObject::setTickUntilDeath(int time){
    m_tickUntilDeath = time;
}

int ActivatingObject::getTickUntilDeath(){
    return m_tickUntilDeath;
}

void ActivatingObject::decreaseTickDeath(){
    m_tickUntilDeath--;
}

void ActivatingObject::setPickupablePlayer(bool set){
    isPickupPlayer = set;
}

void ActivatingObject::setPickupableProtestor(bool set){
    isPickupProtestor = set;
}

bool ActivatingObject::getPickupablePlayer(){
    return isPickupPlayer;
}

bool ActivatingObject::getPickupableProtestor(){
    return isPickupProtestor;
}

OilBarrel::OilBarrel(int startX, int startY, StudentWorld* stuWorld) : ActivatingObject(IID_BARREL, startX, startY, 1, 2, stuWorld, "oil") { setVisible(false); }
void OilBarrel::doSomething() { }

GoldNugget::GoldNugget(int startX, int startY, StudentWorld* stuWorld) : ActivatingObject(IID_GOLD, startX, startY, 1, 2, stuWorld, "gold") {
    setVisible(false);
    setTickUntilDeath(-1);
}
void GoldNugget::doSomething() {
    if (getPickupableProtestor() == true){
        if (getTickUntilDeath() != -1){
            if (getTickUntilDeath() > 0){
                decreaseTickDeath();
            }
            else{
                setDead();
            }
        }
    }
}

SonarKit::SonarKit(int startX, int startY, StudentWorld* stuWorld) : ActivatingObject(IID_SONAR, startX, startY, 1, 2, stuWorld, "sonar") {
    setTickUntilDeath(std::max(100, int(300 - (10 * getWorld()->getLevel()))));
}
void SonarKit::doSomething() {
    if(isAlive()){
        if(getTickUntilDeath() > 0){
            decreaseTickDeath();
        }
        else{
            setDead();
        }
    }
}

WaterPool::WaterPool(int startX, int startY, StudentWorld* stuWorld) : ActivatingObject(IID_WATER_POOL, startX, startY, 1, 2, stuWorld, "water") {
    setTickUntilDeath(std::max(100, int(300 - (10 * getWorld()->getLevel()))));
}
void WaterPool::doSomething() {
    if (isAlive()){
        if(getTickUntilDeath() > 0){
            decreaseTickDeath();
        }
        else{
            setDead();
        }
    }
}

