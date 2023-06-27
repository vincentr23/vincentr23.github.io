#ifndef ACTOR_H_
#define ACTOR_H_

#include <vector>
#include "GraphObject.h"
#include "GameWorld.h"
#include <algorithm>
#include <future>
#include <thread>

// IID constants
const int PLAYER = 0;
const int PROTESTER = 1;
const int HARD_CORE_PROTESTER = 2;
const int WATER_SPURT = 3;
const int BOULDER = 4;
const int BARREL = 5;
const int ICE = 6;
const int GOLD = 7;
const int SONAR = 8;
const int WATER_POOL = 9;

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0) :
		GraphObject(imageID, startX, startY, dir, size, depth)
	{
		id = imageID;
		ourWorld = nullptr;
		std::string state = "";
		hp = 0;
	};

	int getIID() { return id; }
	virtual void doSomething() {}
	bool checkItems(int, int); // checks for items at coords
	void viableSpawn(int&, int&); // checks if spawn location is good to use
	virtual void setState(std::string a_state) { state = a_state; }
	virtual std::string getState() { return state; }
	bool checkBoulder(int, int); // checks for boulder at coords
	bool checkItemsLoop(int, int, int, int);
	int getHp() { return hp; }
	bool checkFalling(); // checks for boulders falling on iceman / protester
	void setWorld(StudentWorld& world) // sets the StudentWorld
	{
		ourWorld = &world;
	}
	virtual void hitWWater() {}
	StudentWorld* getWorld() { return ourWorld; }
	virtual ~Actor() {};
private:
	int id;
protected:
	int hp;
	StudentWorld* ourWorld;
	std::string state;
};

class Ice : public Actor
{
public:
	Ice(int startX, int startY) : Actor(IID_ICE, startX, startY, right, 0.25, 3) {
		setVisible(true);
	};
	virtual ~Ice() {};
};
class Iceman : public Actor
{
public:
	Iceman() : Actor(IID_PLAYER, 30, 60, right, 1, 0)
	{
		hp = 10; // initalization
		water = 5;
		sonar = 1;
		gold = 0;
		points = 0;
		barrel = 0;
		setVisible(true);
	};


	//gets input from user and moves accordingly
	virtual void doSomething();

	void setGoldNum() {
		gold++;
	}
	int getGoldNum() {
		return gold;
	}
	void renewWater() {
		water += 5;
	}

	int getWater() {
		return water;
	}

	// different point values
	void setPointsPick();
	void setPointsBribe();
	void setPointsBarrel();
	void setPointsSonar();
	void setPointsWaterpool();
	void setPointsBoulder();

	void setBarrelNum() {
		barrel++;
	}
	int getBarrelNum() {
		return barrel;
	}
	void setSonarNum() {
		sonar++;
	}
	int getSonar() {
		return sonar;
	}
	void annoy() {
		hp -= 2;
		if (hp <= 0) { state = "dead"; }
	}

	virtual ~Iceman() {} // destructor 

private:
	int water;
	int sonar;
	int gold;
	int points;
	int barrel;
};

class Boulder : public Actor
{
public:
	Boulder(int x, int y) : Actor(IID_BOULDER, x, y, down, 1, 1)
	{
		// initializing
		setVisible(true);
		state = "stable";
		hp = 30;
	}

	virtual ~Boulder() {}

	// checks state of boulder and acts accordingly
	virtual void doSomething();

};

class Gold :public Actor {
public:
	Gold(int startX, int startY) : Actor(IID_GOLD, startX, startY, right, 1.0, 2) {
		setVisible(false);
		ourWorld = nullptr;
		hp = 2;
		m_discovered = false;
	}
	// this constructor is for gold dropped by iceman
	Gold(int startX, int startY, StudentWorld* world) :
		Actor(IID_GOLD, startX, startY, right, 1.0, 2)
	{
		setVisible(true);
		ourWorld = world;
		hp = 0;
		m_discovered = true;
	}
	virtual~Gold() {

	};
	StudentWorld* getWorld() {
		return ourWorld;
	}

	virtual void doSomething();

	bool isDiscovered() {
		return m_discovered;
	}
	void setDiscovered() {
		hp--;
		m_discovered = true;
	}

private:
	bool m_discovered;
};

class Protester :public Actor {
public:
	Protester(int rest_ticks) : Actor(IID_PROTESTER, 60, 60, left, 1, 0) {
		setVisible(true);
		rest = rest_ticks;
		hp = 5;
		gold = 0;
		yelled = 0;
		state = "search";
		numMove = randGen();
	};
	virtual~Protester() {};

	virtual void doSomething();
	bool facingIceman();
	bool checkIceman(Direction);
	int randGen(); // generates a random number between 8 and 60
	void randGen(Direction&); // generates a random direction
	void moveInDir(Direction); // tells protester to move in direction
	bool iceInTheWay(Direction); // checks for ice in the way
	virtual void hitWWater(); // hit protestor w water
	int stepsFromIceman(int&, int&);
	void getToEnd(int, int);
private:
	int hp;
	int gold;
	int rest;
	int yelled;
	int numMove;
	int perp;

protected:
	Protester(int rest_ticks, std::string) : Actor(IID_HARD_CORE_PROTESTER, 60, 60, left, 1, 0)
	{
		setVisible(true);
		rest = rest_ticks;
		hp = 5;
		gold = 0;
		yelled = 0;
		state = "search";
		numMove = randGen();
	}
};

class HardcoreProtester : public Protester {
public: 
	HardcoreProtester(int rest_ticks) : Protester(rest_ticks, "hardcore") {}

};

class Barrel :public Actor {
public:

	Barrel(int startX, int startY) :Actor(IID_BARREL, startX, startY,
		right, 1.0, 2) {
		setVisible(false);//NEED TO CHANGE
		hp = 1;
		m_world = nullptr;
		m_discovered = false;

	};
	virtual void setWorld(StudentWorld& world) {
		m_world = &world;
	}

	StudentWorld* getWorld() {
		return m_world;
	}
	virtual~Barrel() { }

	void setDiscovered() {
		m_discovered = true;
	}
	bool getDiscovered() {
		hp--;
		return m_discovered;
	}
	int getHp() {
		return hp;
	}
	virtual void doSomething();
private:
	StudentWorld* m_world;
	bool m_discovered;

};

class Squirt : public Actor
{
public:
	Squirt(int x, int y, Direction dir) : Actor(IID_WATER_SPURT, x, y, dir, 1, 1)
	{
		setVisible(true);
		hp = 4;
	}
	~Squirt() {

	}
	virtual void doSomething();
};

class WaterPool :public Actor {
public:
	WaterPool(int startX, int startY, int ticks) : Actor(IID_WATER_POOL, startX, startY, right, 1.0, 2)
	{
		setVisible(true);
		hp = ticks;
	}
	~WaterPool() {}

	virtual void doSomething();

	// sets correct points and adjusts iceman var's accordingly
	void setPoints(int id); 

protected:
	// a constructor to inherit for sonar
	WaterPool(int image_id, int ticks) : Actor(image_id, 0, 60, right, 1, 2)
	{
		setVisible(true);
		hp = ticks;
	}
};

// since code is mostly the same, inherit from waterpool
class Sonar :public WaterPool {
public:
	Sonar(int ticks) : WaterPool(IID_SONAR, ticks) {}
};


#endif; // ACTOR_H