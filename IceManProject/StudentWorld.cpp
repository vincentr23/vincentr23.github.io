#include "StudentWorld.h"
#include"GameController.h"
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp


// creates Iceman and sets ourWorld in Iceman

int StudentWorld::init()
{
	iceman = new Iceman();
	iceman->setWorld(*this);
	createIce();
	createThings();
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	
	iceman->doSomething();
	destroyIce(); // destroys overlapping ice

	for (int i = 0; i < lastItem; i++)
	{
		objects[i]->doSomething();
		boulderState();
		deleteDead();
		setHeadingText();
	}
	if (iceman->getHp() <= 0)
	{
		playSound(SOUND_PLAYER_GIVE_UP);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	} 
	if ((rand() % ((int)getLevel() * 25 + 100)) == 1) // chance of spawn
		createSonarOrWater();
	createProtester();
	setWorldAll();
	if (iceman->getBarrelNum() == min((int)(2 + getLevel()), 21))
		return GWSTATUS_FINISHED_LEVEL;
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	if (iceman)
	{
		delete iceman;
		iceman = nullptr;
	}
	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
			if (ice[i][j])
			{
				deleteIce(i, j);
			}
		}
	}
	for (int i = 0; i < lastItem; i++)
	{
		delete objects[i];
	}
	objects.clear();
	lastItem = 0;
	num_protesters = 0;
	proTick = 0;
}



void StudentWorld::createIce()
{
	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		for (int j = 0; j < VIEW_HEIGHT - 4; j++)
		{

			if ((i >= 30) && (i <= 33)) // creates tunnel
			{
				if ((j >= 0) && (j < 4)) // these are coords for tunnel
				{
					ice[i][j] = new Ice(i, j); // fills 2D array with ice object
				}
				continue;
			}
			ice[i][j] = new Ice(i, j); // fills 2D array with ice
		}
	}
}

void StudentWorld::destroyIce()
{
	int currX, currY; // keeps track of position
	// nested for loop for ice array
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			currX = iceman->getX() + i; // gets general location of
			currY = iceman->getY() + j; // iceman
			if (ice[currX][currY]) // if we have overlapping
			{
				// destroys ice object
				deleteIce(currX, currY);
			}
		}
	}
}

void StudentWorld::createThings()
{
	int B = std::min((int)(getLevel() / 2 + 2), 9),
		x = 0, y = 60;
	int G = std::max((int)(5 - getLevel()) / 2, 2);
	int L = min((int)(2 + getLevel()), 21);

	// creating boulders
	while (B != 0)
	{
		// using iceman to set spawn for boulder
		iceman->viableSpawn(x, y);
		objects.push_back(new Boulder(x, y));
		setWorldLast();
		B--;
	}
	while (G != 0)
	{
		iceman->viableSpawn(x, y);
		objects.push_back(new Gold(x, y));
		setWorldLast();
		G--;
	}
	//creating barrels
	while (L != 0)
	{
		iceman->viableSpawn(x, y);
		objects.push_back(new Barrel(x, y));
		setWorldLast();
		L--;
	}

	// couldn't get this to work within prev loop
	for (auto it : objects)
	{
		// deletes ice that boulder sits on
		if (it->getIID() == BOULDER)
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					deleteIce(it->getX() + i, it->getY() + j);
				}
			}
		// sets world for entire vector
		it->setWorld(*this);
	}
}

void StudentWorld::createProtester()
{
	if (num_protesters < std::min(15, 2 + (int)(getLevel() * 1.5)))
	{
		if (proTick <= 0)
		{
			int probOfHC = std::min(90, (int)(getLevel() * 10 + 30));
			int x = rand() % 100;

			if (x > probOfHC)
			{
				objects.push_back(new Protester(std::max(0, (int)((3 - getLevel()) / 4)))); // spawn reg protester
			}
			else
			{
				objects.push_back(new HardcoreProtester(std::max(0, (int)((3 - getLevel()) / 4)))); // spawn HC protester
			}
			setWorldLast();
			num_protesters++;
			proTick = std::max(25, (int)(200 - getLevel()));
			return;
		}
	}
	proTick--;
}



void StudentWorld::boulderState()
{
	int i = 0; // for keeping track of location of boulder
	for (auto it : objects)
	{
		if (it->getIID() == BOULDER) // finds boulder in vector
		{
			// checks for ice under boulder
			if ((checkBelow(it) && (it->getState() == "stable")))
			{
				// if no ice under boulder, changes state to waiting
				it->setState("waiting");
			}

			// keeps falling until we hit more ice / bottom of stage
			// then gets deleted
			if (((!checkBelow(it) && (it->getState() == "falling"))) ||
				(it->getState() == "dead"))
			{
				delete it;
				objects.erase(objects.begin() + i);
				lastItem--;
			}
		}
		i++;
	}
}

void StudentWorld::createSonarOrWater()
{
	// checks for sonar on field
	if ((rand() % 5) == 1) // 1 means we spawn sonar
	{
		for (auto it : objects)
		{
			if (it->getIID() == SONAR) return;
		}
		objects.push_back(new Sonar(max(100, (int)(300 - (10 * getLevel())))));
		setWorldLast();
	}
	else // any other number gives us waterpool
	{
		int x, y;
		bool spawn = true;
		while (spawn) // runs until we have a viable spawn for water
		{ 
			spawn = spawnWater(x, y);
		}
		objects.push_back(new WaterPool(x, y, max(100, (int)(300 - 10 * getLevel()))));
		setWorldLast();
	}
}

bool StudentWorld::spawnWater(int& x, int& y)
{
	x = rand() % 59, y = rand() % 55;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (ice[i + x][j + y])
			{
				return true;
			}
		}
	}
	if (iceman->checkBoulder(x, y)) return true;
	else return false;
}

void StudentWorld::deleteIce(int x, int y)
{
	delete ice[x][y];
	ice[x][y] = nullptr;
}

void StudentWorld::setSquirtWorld()
{
	for (auto it : objects)
	{
		if (it->getIID() == WATER_SPURT)
		{
			it->setWorld(*this);
		}
	}
}

bool StudentWorld::checkBelow(Actor* it)
{
	if ((it->checkBoulder(it->getX(), it->getY())))
	{
		return false;
	}
	if (((ice[it->getX()][it->getY() - 1] == nullptr) &&
		(ice[it->getX() + 1][it->getY() - 1] == nullptr) &&
		(ice[it->getX() + 2][it->getY() - 1] == nullptr) &&
		(ice[it->getX() + 3][it->getY() - 1] == nullptr))) // checks for items around
	{
		return true;
	}

	return false;
}

bool StudentWorld::iceInDir(int x, int y, std::string dir)
{
	if (dir == "side") // used for sides of protester
	{
		for (int i = 0; i < 4; i++)
			if (ice[x][y + i]) return false;
		return true;
	}
	if (dir == "top") // used for top and bottom of protester
	{
		for (int i = 0; i < 4; i++)
			if (ice[x + i][y]) return false;
		return true;
	}
}

void StudentWorld::setWorldLast()
{
	objects[lastItem]->setWorld(*this);
	lastItem++;
}

void StudentWorld::setWorldAll()
{
	for (auto it : objects)
	{
		if (it != nullptr) // so we dont dereference null
			if (it->getWorld() == nullptr) // does not have a world attached
			{
				it->setWorld(*this);
				lastItem++;
			}
	 }
}

bool StudentWorld::damageProtester()
{
	for (auto jt : objects)
	{
		if((jt->getIID() == PROTESTER) || (jt->getIID() == HARD_CORE_PROTESTER))
			for (auto it : objects)
			{
				if (it->getIID() == WATER_SPURT)
				{
					if (iceman->checkItemsLoop(jt->getX(), jt->getY(), it->getX(), it->getY()))
					{
						jt->hitWWater();
						return true;
					}
				}
			}
	}
	return false;
}

int StudentWorld::distanceFromIceman(int& ox, int& oy)
{
 	bool search[64][64]; int dist = 0;
	int x = ox, y = oy;
	// creates a copy of ice but with bool
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
		{
			if (ice[i][j]) search[i][j] = true;
			else search[i][j] = false;
		}
	queue<pair<int, int>> q;
	search[x][y] = true;
	while ((x != iceman->getX()) || (y != iceman->getY()))
	{
		if (x - 1 >= 0)
		{
			if ((iceInDir(x - 1, y, "side")) && (!search[x - 1][y]))
			{
				x--; dist++;
				q.push({ x , y });
				search[x][y] = true; 
				continue;
			}
			else
			{
				q.pop(); dist--;
			}
		}
		if (x + 4 < 64)
		{
			if ((iceInDir(x + 4, y, "side")) && (!search[x + 4][y]))
			{
				x++; dist++;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop(); dist--;
			}
		}
		if (y - 1 <= 0)
		{
			if ((iceInDir(x, y - 1, "top")) && (!search[x][y - 1]))
			{
				y--; dist++;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop(); dist--;
			}
		}
		if (y + 4 < 64)
		{
			if ((iceInDir(x, y + 4, "top")) && (!search[x][y + 4]))
			{
				y++; dist++;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop(); dist--;
			}
		}
	}
	ox = q.front().first;
	oy = q.front().second;
	return dist;
}

bool StudentWorld::checkSquirt(int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if ((x + i > 63) || (y + j > 63) || (x - i < 0) || (y - j < 0)) return false; // off stage
			if (ice[x + i][y + j] != nullptr)
				return false; // ice in the way
		}
	}
	return true;
}

void StudentWorld::deleteDead()
{
	int track = 0; std::string state, dead = "dead";
	for (auto it : objects)
	{
		state = it->getState();
		if (state == dead)
		{
			if ((it->getIID() == PROTESTER) ||
				(it->getIID() == HARD_CORE_PROTESTER)) num_protesters--;
			delete it;
			objects.erase(objects.begin() + track);
			lastItem--;
		}
		track++;
	}
}


double StudentWorld::distance(int x, int y, int m, int n)
{
	return sqrt((m - x) * (m - x) + (n - y) * (n - y));
}

void StudentWorld::setHeadingText()
{
	//set level
	int level = getLevel();
	string strLevel = "  Lvl: ";
	strLevel += " " + to_string(getLevel());

	//set lives
	int lives = getLives();
	string strLives = "  Lives: ";
	strLives += to_string(lives);

	//set health
	int health = iceman->getHp();
	string strHealth = "  Hlth: ";
	int healthPercentage = (health / 10.0) * 100;
	strHealth += " " + to_string(healthPercentage) + '%';

	//set water
	int waters = getIceman()->getWater();
	string strWaters = "  Wtr: ";
	strWaters += " " + to_string(waters);

	//set gold
	int gold = getIceman()->getGoldNum();
	string strGold = "  Gld: ";
	strGold += " " + to_string(gold);

	//set oil left
	int oilLeft = getOilInLevel() - getIceman()->getBarrelNum();
	string strOil = "  Oil Left: ";
	strOil += " " + to_string(oilLeft);

	//set sonar
	int sonar = getIceman()->getSonar();
	string strSonar = "  Sonar: ";
	strSonar += " " + to_string(sonar);

	//set score
	int score = getScore();
	string strScore = "Scr: ";
	int intScore = score;
	int digitCount = 0;
	while (intScore != 0)
	{
		intScore = intScore / 10;
		digitCount++;
	}
	for (int i = 0; i < 5 - digitCount; i++)
	{
		strScore += '0';
	}
	strScore += to_string(score);

	string result = strLevel + strLives + strHealth + strWaters + strGold + strOil + strSonar + strScore;

	setGameStatText(result);
}

int StudentWorld::getOilInLevel() {
	return min((int)(2 + getLevel()), 21);
}