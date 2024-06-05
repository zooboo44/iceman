#include "StudentWorld.h"
#include <string>
#include "Actor.h"
#include "GameConstants.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{
    //srand(time(NULL));
    m_bouldersPerLevel = std::min(int(getLevel() / 2 + 2), 9);
    m_goldPerLevel = std::max(int(5 - getLevel() / 2), 2);
    m_barrelsPerLevel = std::min(int(2 + getLevel()), 21);
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 64; y++)
        {
            if (((x == 30 || x == 31 || x == 32 || x == 33) && y > 3) || y >= 60)
                iceArr[x][y] = nullptr;
            else
                iceArr[x][y] = new Ice(x, y, this);
        }
    }
    for (int i = 0; i < m_bouldersPerLevel; i++)
    {
        int x = -1;
        int y = -1;
        while(y < 3){
            getRandNum(x, y);
        }
        Boulder* boulder = new Boulder(x, y, this);
        boulders.push_back(boulder);
    }

	for (int i = 0; i < m_barrelsPerLevel; i++)
	{
		int x;
		int y;
		getRandNum(x, y);
		OilBarrel* barrel = new OilBarrel(x, y, this);
		pickupables.push_back(barrel);
	}

	for (int i = 0; i < m_goldPerLevel; i++)
	{
		int x;
		int y;
		getRandNum(x, y);
		GoldNugget* gold = new GoldNugget(x, y, this);
		pickupables.push_back(gold);
	}

    
    probabilityOfHardcore = min(90, int(getLevel() * 10 + 30));
    addProtester(probabilityOfHardcore);
    protesterCount = 1;
    newProtesterTickOriginal = max(25, int(200 - getLevel()));
    newProtesterTickCountdown = newProtesterTickOriginal;
    maxProtesterCount = min(15, int(2 + getLevel() * 1.5));

	player = new IceMan(30, 60, this);

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::setDisplayText()
{
    int level = getLevel();
    int lives = getLives();
    int health = (player->getHealth() * 100) / 5;
    int squirts = player->getSquirts();
    int gold = player->getGold();
    int barrelsLeft = getBarelsLeft();
    int sonar = player->getSonar();
    std::stringstream score;
    score << setw(6) << setfill('0') << getScore();
    string scoreValue = score.str();

    // string to make it pretty
    string s = "Lvl: " + to_string(level) + "  Lives: " + to_string(lives) + "  Hlth: " + to_string(health) + "%" + "  Wtr: " + to_string(squirts) + "  Gld: " + to_string(gold) + "  Oil Left: " + to_string(barrelsLeft) + "  Sonar: " + to_string(sonar) + "  Scr: " + scoreValue;

    setGameStatText(s);

}

int StudentWorld::move()
{
	// Update stats
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> chaceOfItemSpawn(1, (int((getLevel() * 25) + 300)));
    std::uniform_int_distribution<int> chanceOfOther(1, 5);
	setDisplayText();
	player->doSomething();
    if (chaceOfItemSpawn(generator) == 1){
        if(chanceOfOther(generator) == 1 && ifSonarExists() == false){
            SonarKit* sonar = new SonarKit(0, 60, this);
            pickupables.push_back(sonar);
        }
        else{
            std::uniform_int_distribution<int> forX(0, 60);
            std::uniform_int_distribution<int> forY(0, 56);
            int w_x;
            int w_y;
            bool tempB = true;
            while(tempB){
                w_x = forX(generator);
                w_y = forY(generator);
                if (!iceChunkExists(w_x, w_y) && !(boulderExists(w_x, w_y)) && !(playerExists(w_x, w_y))){
                    tempB = false;
                }
            }
            WaterPool* water = new WaterPool(w_x, w_y, this);
            pickupables.push_back(water);
        }
    }
	for (auto a : squirtedGun) {
		if (a != nullptr) {
			a->doSomething();
		}
	}
    for (auto a : pickupables) {
        if (a != nullptr) {
            a->doSomething();
        }
    }
    for (auto it = pickupables.begin(); it != pickupables.end();)
    {
        if (!(*it)->isAlive()){
            ActivatingObject* temp = (*it);
            it = pickupables.erase(it);
            delete temp;
        }
      else
        ++it;
    }
    
    
	for (auto it = squirtedGun.begin(); it != squirtedGun.end();)
	{
		if ((*it)->getIsDead()) {
			Squirt* temp = (*it);
			it = squirtedGun.erase(it);
			delete temp;
		}
		else
			++it;
	}
    for (auto a : boulders){
        a->doSomething();
    }
    for (auto it = boulders.begin(); it != boulders.end();)
    {
        if (!(*it)->isAlive()){
            Boulder* temp = (*it);
            it = boulders.erase(it);
            delete temp;
        }
      else
        ++it;
    }
    
    if (m_barrelsPerLevel == 0){
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    if ((newProtesterTickCountdown == 0) && (protesterCount < maxProtesterCount)) {
        addProtester(probabilityOfHardcore);
        newProtesterTickCountdown = newProtesterTickOriginal;
    }
    
    for (auto a : protesters) {
        a->doSomething();
    }
    for (auto it = protesters.begin(); it != protesters.end();)
    {
        if ((*it)->getDead()) {
            Protester* temp = (*it);
            it = protesters.erase(it);
            delete temp;
        }
        else
            ++it;
    }
    if (player->getHealth() <= 0) {
        decLives();
        playSound(SOUND_PLAYER_GIVE_UP);
        return GWSTATUS_PLAYER_DIED;
    }
    newProtesterTickCountdown--;
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			Ice* temp = iceArr[x][y];
			delete temp;
		}
	}
    delete player;
    while(!boulders.empty()) {
        delete boulders.back();
        boulders.pop_back();
    }
    while(!pickupables.empty()) {
        delete pickupables.back();
        pickupables.pop_back();
    }
    while(!protesters.empty()) {
        delete protesters.back();
        protesters.pop_back();
    }
	for (auto a : squirtedGun) {
		delete a;
	}
}

IceMan* StudentWorld::getPlayer()
{
	return player;
}

bool StudentWorld::clearIce(int clearX, int clearY)
{
	bool tempBool = false;
	for (int startX = clearX; startX < clearX + 4; startX++)
	{
		for (int startY = clearY; startY < clearY + 4; startY++)
		{
			if ((clearX < 0 || clearY < 0) || clearX > 63 || clearY > 63)
				break;
			Ice* tempIce = iceArr[startX][startY];
			if (tempIce != nullptr)
			{
				tempBool = true;
				iceArr[startX][startY] = nullptr;
				delete tempIce;
			}
		}
	}
	return tempBool;
}

bool StudentWorld::getIfIceExists(int x, int y) {
	if (iceArr[x][y] != nullptr)
		return true;
	return false;
}

bool StudentWorld::iceChunkExists(int x, int y){
    for (int startX = x; startX < x + 4; startX++)
    {
        for (int startY = y; startY < y + 4; startY++)
        {
            if (iceArr[startX][startY] != nullptr)
                return true;
        }
    }
    return false;
}

bool StudentWorld::boulderExists(int x, int y){
    for (int startX = x; startX < x + 4; startX++)
    {
        for (int startY = y; startY < y + 4; startY++)
        {
            for (auto a : boulders){
                if (a->getX() == x && a->getY() == y)
                    return true;
            }
        }
    }
    return false;
}

bool StudentWorld::playerExists(int x, int y){
    for (int startX = x; startX < x + 4; startX++)
    {
        for (int startY = y; startY < y + 4; startY++)
        {
            if (player->getX() == x && player->getY() == y)
                return true;
        }
    }
    return false;
}

bool StudentWorld::isNear(int x, int y)
{
    double distance;
    for(auto a : pickupables){
        if(distance = sqrt(pow(a->getX() - x, 2) + pow(a->getY() - y, 2)) <= 6){
            return true;
        }
    }
    for (auto a : boulders) {
        if (distance = sqrt(pow(a->getX() - x, 2) + pow(a->getY() - y, 2)) <= 6) {
            return true;
        }
    }
    return false;
}

bool StudentWorld::isNearWhileFalling(int startX, int startY){
    bool distance;
    for (auto a : boulders) {
        if (a->isFalling() && (distance = sqrt(pow(a->getX() - startX, 2) + pow(a->getY() - startY, 2)) <= 3)) {
            return true;
        }
    }
    return false;
}

void StudentWorld::revealHidden(int startX, int startY){
    double distance;
    for(auto a : pickupables){
        if(distance = sqrt(pow(a->getX() - startX, 2) + pow(a->getY() - startY, 2)) <= 4){
            a->setVisible(true);
        }
    }
}

void StudentWorld::revealHiddenSonar(int startX, int startY){
    double distance;
    for(auto a : pickupables){
        if(distance = sqrt(pow(a->getX() - startX, 2) + pow(a->getY() - startY, 2)) <= 14){
            a->setVisible(true);
        }
    }
}

string StudentWorld::pickUpNear(int x, int y){
    double distance;
    for(auto it = pickupables.begin(); it != pickupables.end();){
        if(distance = sqrt(pow((*it)->getX() - x, 2) + pow((*it)->getY() - y, 2)) <= 3){
            if ((*it)->getPickupablePlayer() == true){
                std::string tempStr = (*it)->getType();
                ActivatingObject* tempPtr = (*it);
                it = pickupables.erase(it);
                delete tempPtr;
                return tempStr;
            }
        }
        ++it;
    }
    return "";
}

bool StudentWorld::pickUpNearProt(int x, int y){
    double distance;
    for(auto it = pickupables.begin(); it != pickupables.end();){
        if(distance = sqrt(pow((*it)->getX() - x, 2) + pow((*it)->getY() - y, 2)) <= 3){
            if ((*it)->getPickupableProtestor() == true){
                ActivatingObject* tempPtr = (*it);
                it = pickupables.erase(it);
                delete tempPtr;
                return true;
            }
        }
        ++it;
    }
    return false;
}

bool StudentWorld::isNearBoulder(int startX, int startY, GraphObject::Direction dir)
{
    if (dir == GraphObject::right){
        for(auto a : boulders){
            for (int x = a->getX(); x < a->getX() + 4; x++)
            {
                for (int y = a->getY(); y < a->getY() + 4; y++)
                {
                    if(startX + 4 == x && (startY == y || startY + 3 == y))
                        return true;
                }
            }
        }
    }
    else if (dir == GraphObject::left){
        for(auto a : boulders){
            for (int x = a->getX(); x < a->getX() + 4; x++)
            {
                for (int y = a->getY(); y < a->getY() + 4; y++)
                {
                    if(startX - 1 == x && (startY == y || startY + 3 == y))
                        return true;
                }
            }
        }
    }
    else if (dir == GraphObject::up){
        for(auto a : boulders){
            for (int x = a->getX(); x < a->getX() + 4; x++)
            {
                for (int y = a->getY(); y < a->getY() + 4; y++)
                {
                    if(startY + 4 == y && (startX == x || startX + 3 == x))
                        return true;
                }
            }
        }
    }
    else if (dir == GraphObject::down){
        for(auto a : boulders){
            for (int x = a->getX(); x < a->getX() + 4; x++)
            {
                for (int y = a->getY(); y < a->getY() + 4; y++)
                {
                    if(startY + - 1 == y && (startX == x || startX + 3 == x))
                        return true;
                }
            }
        }
    }
    return false;
}

void StudentWorld::getRandNum(int& x, int& y) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> randX(0, 60);
    std::uniform_int_distribution<int> randY(0, 56);
    x = randX(generator);
    y = randY(generator);
	while (((x >= 27 && x <= 33) && y >= 4) || isNear(x, y)) {
        x = randX(rd);
        y = randY(rd);
	}
}

void StudentWorld::addSquirt(int x, int y, GraphObject::Direction dir) {
	if (dir == GraphObject::up) {
		Squirt* squirt = new Squirt(x, y+1, dir, this);
		squirtedGun.push_back(squirt);
		return;
	}
	if (dir == GraphObject::down) {
		Squirt* squirt = new Squirt(x, y-1, dir, this);
		squirtedGun.push_back(squirt);
		return;
	}
	if (dir == GraphObject::right) {
		Squirt* squirt = new Squirt(x+1, y, dir, this);
		squirtedGun.push_back(squirt);
		return;
	}
	if (dir == GraphObject::left) {
		Squirt* squirt = new Squirt(x-1, y, dir, this);
		squirtedGun.push_back(squirt);
		return;
	}
}

void StudentWorld::decreaseBarrels(){
    m_barrelsPerLevel--;
}

void StudentWorld::decreaseGold(){
    m_goldPerLevel--;
}

void StudentWorld::decSquirt() {

}

void StudentWorld::addProtester(int probabilityOfHardcore) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> randProt(1, 100);
    if (randProt(generator) <= probabilityOfHardcore) {
        HardcoreProtester* protester = new HardcoreProtester(getLevel(), this);
        protesters.push_back(protester);
        protesterCount++;
    }
    else {
        RegularProtester* protester = new RegularProtester(getLevel(), this);
        protesters.push_back(protester);
        protesterCount++;
    }
}

bool StudentWorld::isNearIceman(int x, int y) {
    double distance;

    if (distance = sqrt(pow(x - getPlayer()->getX(), 2) + pow(y - getPlayer()->getY(), 2)) <= 4)
        return true;
    else
        return false;
}

void StudentWorld::damageIceman() {
    getPlayer()->getsAnnoyed();
}

void StudentWorld::nearProtester(int x, int y) {
    double distance;
    
    for (auto a : protesters) {
        if (distance = sqrt(pow(x - a->getX(), 2) + pow(y - a->getY(), 2)) <= 3) {
            annoyProtester(a);
        }
    }
}

void StudentWorld::annoyProtester(Protester* pro) {
    pro->getAnnoyed();
}

bool StudentWorld::ifSonarExists(){
    for (auto a : pickupables){
        if (a->getType() == "sonar"){
            return true;
        }
    }
    return false;
}

void StudentWorld::createProtestorGold(int x, int y){
    GoldNugget* gold = new GoldNugget(x, y, this);
    gold->setVisible(true);
    gold->setPickupablePlayer(false);
    gold->setPickupableProtestor(true);
    gold->setTickUntilDeath(500);
    pickupables.push_back(gold);
}


bool StudentWorld::iceAbove(int x, int y){
    for(int i = 0; i < 4; i++){
        if(iceArr[x + i][y + 4] != nullptr){
            return true;
        }
    }
    return false;
}

bool StudentWorld::iceBelow(int x, int y){
    for(int i = 0; i < 4; i++){
        if(iceArr[x + i][y - 1] != nullptr){
            return true;
        }
    }
    return false;
}

bool StudentWorld::iceRight(int x, int y){
    for(int i = 0; i < 4; i++){
        if(iceArr[x + 4][y + i] != nullptr){
            return true;
        }
    }
    return false;
}

bool StudentWorld::iceLeft(int x, int y){
    for(int i = 0; i < 4; i++){
        if(iceArr[x - 1][y + i] != nullptr){
            return true;
        }
    }
    return false;
}
