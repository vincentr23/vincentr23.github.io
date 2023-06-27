#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp


class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
		iceman = nullptr; // initializing iceman
		// initializing ice objects in 2D array
		for (int i = 0; i < VIEW_WIDTH; i++)
		{
			for (int j = 0; j < VIEW_HEIGHT; j++)
			{
				ice[i][j] = nullptr;
			}
		}
		lastItem = 0;
		num_protesters = 0;
		proTick = 0;

	}
	// initializes all variables for game
	virtual int init();

	virtual int move();

	virtual void cleanUp();


	~StudentWorld()
	{
		cleanUp();
	};
	double distance(int x, int y, int m, int n);

	void setHeadingText();

	int getOilInLevel();

	std::vector<Actor*> getObjects() { return objects; }
	std::vector<Actor*>& getAObjects() { return objects; } // address of objects

	//void createGold();
	bool checkSquirt(int x, int y); // checks for ice in way of ice
	void setSquirtWorld(); // finds squirts in object and spasses through this world

	Iceman* getIceman() {
		if (iceman == nullptr)
			return nullptr;
		return iceman;
	}

	void setPoints(unsigned int howMuch)
	{
		increaseScore(howMuch);
	}

	void createIce(); // fills the 2D array with ice
	void destroyIce(); // finds and destroys overlapping ice
	void deleteIce(int, int); // delete ice and given coords
	bool iceAt(int x, int y) { return ice[x][y]; }
	void deleteDead(); // deletes all dead actors
	void createThings(); // creates boulders
	void createProtester(); // creates protestors
	void boulderState(); // changes state of boulder
	void createSonarOrWater(); // creates sonar if not already on field
	bool spawnWater(int&, int&); // checks coords for water
	bool checkBelow(Actor*); //checks for ice below
	bool iceInDir(int, int, std::string); // checks ice at coords
	void setWorldLast(); // sets world to last item added
	void setWorldAll(); // sets world for all of objects vector
	bool damageProtester(); // checks if water hit protester
	int distanceFromIceman(int&, int&); // search for iceman
	void addItem() { lastItem++; }

private:
	int lastItem;
	std::vector<Actor*> objects;
	Iceman* iceman;
	Ice* ice[VIEW_WIDTH][VIEW_HEIGHT];
	int num_protesters;
	int proTick;

};

#endif // STUDENTWORLD_H_
