#include "Actor.h"
#include "StudentWorld.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <algorithm>
#include "GameController.h"
#include "GraphObject.h"
#include <thread>

#include <string>
#include <vector>
#include <queue>

using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

void Iceman::doSomething()
{
	{
		int ch;
		if (ourWorld->getKey(ch) == true)
			switch (ch)
			{
			case KEY_PRESS_RIGHT: // move right
				if (getDirection() == right)
				{
					if (checkBoulder(getX() + 1, getY())) break; // boulder in the way
					if (getX() == 60)
					{
						moveTo(getX(), getY()); break;
					}
					moveTo(getX() + 1, getY());
					ourWorld->playSound(SOUND_DIG); break;
				}
				else setDirection(right); break;

			case KEY_PRESS_LEFT: // move left
				if (getDirection() == left)
				{
					if (checkBoulder(getX() - 1, getY())) break;
					if (getX() == 0)
					{
						moveTo(getX(), getY()); break;
					}
					moveTo(getX() - 1, getY()); 
					ourWorld->playSound(SOUND_DIG);break;
				}
				else setDirection(left); break;

			case KEY_PRESS_UP: // move up
				if (getDirection() == up)
				{
					if (checkBoulder(getX(), getY() + 1)) break;
					if (getY() == 60)
					{
						moveTo(getX(), getY()); break;
					}
					moveTo(getX(), getY() + 1);
					ourWorld->playSound(SOUND_DIG); break;
				}
				else setDirection(up); break;

			case KEY_PRESS_DOWN: // move down
				if (getDirection() == down)
				{
					if (checkBoulder(getX(), getY() - 1))break;
					if (getY() == 0)
					{
						moveTo(getX(), getY()); break;
					}
					moveTo(getX(), getY() - 1);
					ourWorld->playSound(SOUND_DIG); break;
				}
				else setDirection(down); break;
			case 'Z': // sonar illuminates objects within 12 radius
			case'z':
				if (sonar > 0)
				{
					for (auto it : ourWorld->getObjects())
					{
						if (ourWorld->distance(getX(), getY(), it->getX(), it->getY()) <= 12)
							it->setVisible(true);
					}
					sonar--; ourWorld->playSound(SOUND_SONAR); break;
				} break;
			case KEY_PRESS_TAB:
				if (gold > 0)
				{
					(ourWorld->getAObjects()).push_back(new Gold(getX(), getY(), ourWorld));
					ourWorld->addItem();
					gold--; break;
				} break;
			case KEY_PRESS_SPACE: // squirt
				if (water > 0)
				{
					water--; // used water
					ourWorld->playSound(SOUND_PLAYER_SQUIRT);
					// finds direction to face water right way and spawn if no ice is in the way
					switch (getDirection())
					{
					case right:
						if ((ourWorld->checkSquirt(getX() + 4, getY())) && ((getX() + 4) < 64))
						{
							(ourWorld->getAObjects()).push_back(new Squirt(getX() + 4, getY(), (Direction)getDirection()));
							ourWorld->addItem();
						} break;
					case left:
						if ((ourWorld->checkSquirt(getX() - 4, getY())) && ((getX() - 4) > 0))
						{
							(ourWorld->getAObjects()).push_back(new Squirt(getX() - 4, getY(), (Direction)getDirection()));
							ourWorld->addItem();
						} break;
					case up:
						if ((ourWorld->checkSquirt(getX(), getY() + 4)) && ((getY() + 4) < 64))
						{
							(ourWorld->getAObjects()).push_back(new Squirt(getX(), getY() + 4, (Direction)getDirection()));
							ourWorld->addItem();
						} break;
					case down:
						if ((ourWorld->checkSquirt(getX(), getY() - 4)) && ((getY() - 4) > 0))
						{
							(ourWorld->getAObjects()).push_back(new Squirt(getX(), getY() - 4, (Direction)getDirection()));
							ourWorld->addItem();
						} break;
					}
					ourWorld->setSquirtWorld(); break;
				}break;
			case KEY_PRESS_ESCAPE:
				hp = 0;
				break;
			}
	}
	if (checkFalling()) // boulder hits iceman
	{
		hp = 0;
		ourWorld->playSound(SOUND_PLAYER_ANNOYED);
	}
}

void Boulder::doSomething()
{
	// makes boulder fall at end of 30 ticks
	if (state == "waiting")
	{
		if (hp != 0)
			hp--;
		else
		{
			state = "falling";
			ourWorld->playSound(SOUND_FALLING_ROCK);
		}
	}

	if (state == "falling")
	{
		moveTo(getX(), getY() - 1);
		if (getY() < 0)
			state = "dead";
	}
}

bool Actor::checkFalling()
{
	// checks for falling boulder
	for (auto it : ourWorld->getObjects())
	{
		if ((it->getIID() == BOULDER) && (it->getState() == "falling"))
			return checkBoulder(getX(), getY());
	}
}

void Iceman::setPointsPick() { ourWorld->setPoints(10); }

void Iceman::setPointsBribe() { ourWorld->setPoints(25); }

void Iceman::setPointsBarrel() { ourWorld->setPoints(1000); }

void Iceman::setPointsSonar() { ourWorld->setPoints(75); }

void Iceman::setPointsWaterpool() { ourWorld->setPoints(100); }

void Iceman::setPointsBoulder() { ourWorld->setPoints(500); }

bool Actor::checkBoulder(int x, int y)
{
	// for loop looking for boulders in vector
	for (auto it : ourWorld->getObjects())
	{
		// looking at the same objects
		if ((x == it->getX()) && (y == it->getY()) && (getIID() == it->getIID())) continue;
		// found boulder
		if (it->getIID() == BOULDER)
		{
			// checks if coords match
			int bx = it->getX(), by = it->getY();

			if (checkItemsLoop(x, y, bx, by)) return true;
		}
	}
	return false;
}

bool Actor::checkItems(int x, int y)
{
	for (auto it : ourWorld->getObjects())
	{
		// looking at the same objects
		if ((x == it->getX()) && (y == it->getY() && (getIID() == it->getIID()))) continue;
		// checks if coords match
		int bx = it->getX(), by = it->getY();

		if(checkItemsLoop(x, y, bx, by)) return true;
	}

	return false;
}

bool Actor::checkItemsLoop(int x, int y, int bx, int by)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (((y + j) == by) || ((by + j) == y))
				if (((x + i) == bx) || ((bx + i) == x)) return true;

			if (((x + i) == bx) || ((bx + i) == x))
				if (((y + j) == by) || ((by + j) == y)) return true;
		}
	return false;
}

void Actor::viableSpawn(int& x, int& y)
{
	// finding random coords for items
	do {
		do {
			x = rand() % 61;
		} while (!((x < 27) || (x > 34)));

		y = rand() % 56 + 1;
	} while (checkItems(x, y));
}

void Gold::doSomething()
{
	if (m_discovered == false)
	{
		// undiscovered and hasn't been close
		if (hp == 2)
		{
			if (ourWorld->distance(getX(), getY(), ourWorld->getIceman()->getX(),
				ourWorld->getIceman()->getY()) <= 4.00)
			{
				setVisible(true);
				hp--;
			}
		}

		if (hp == 1)
		{
			if (ourWorld->distance(getX(), getY(), ourWorld->getIceman()->getX(),
				ourWorld->getIceman()->getY()) <= 3.00)
			{
				setVisible(true);
				ourWorld->playSound(SOUND_GOT_GOODIE);
				ourWorld->getIceman()->setPointsPick();
				ourWorld->getIceman()->setGoldNum();
				hp--;
				state = "dead";
			}
		}
	}
	// checking for protestor to pickup
	if (m_discovered == true)
	{
		for (auto it : ourWorld->getObjects())
		{
			// checks for protestors in vicinity
			if ((it->getIID() == PROTESTER) || (it->getIID() == HARD_CORE_PROTESTER))
			{
				// protestor picks up gold
				if (ourWorld->distance(getX(), getY(), it->getX(), it->getY()) <= 3.00)
				{
					ourWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
					hp--;
					state = "dead";
					ourWorld->getIceman()->setPointsBribe();
					if (getIID() == PROTESTER) it->setState("leave");
					else it->setState("gold");
				}
			}
		}
		if (hp < -100)
		{
			state = "dead";
		}
		hp--;
	}
}

void Barrel::doSomething()
{
	if (m_discovered == false)
	{
		// undiscovered and hasn't been close
		if (ourWorld->distance(getX(), getY(), ourWorld->getIceman()->getX(),
			ourWorld->getIceman()->getY()) <= 4.00)
		{
			setVisible(true);
			if (ourWorld->distance(getX(), getY(), ourWorld->getIceman()->getX(),
				ourWorld->getIceman()->getY()) <= 3.00)
			{
				ourWorld->playSound(SOUND_FOUND_OIL);
				ourWorld->getIceman()->setPointsBarrel();
				ourWorld->getIceman()->setBarrelNum();

				hp--;
				state = "dead";
			}
		}
	}

	// checking for iceman to pick up
	if (m_discovered == true)
	{
		for (auto it : ourWorld->getObjects())
		{
			// check for iceman in vicinity
			if (it->getIID() == PLAYER)
			{
				//iceman pick up the oil
				if (ourWorld->distance(getX(), getY(), it->getX(),
					it->getY()) <= 3.00)
				{
					ourWorld->playSound(SOUND_FOUND_OIL);
					hp--;
					state = "dead";
				}
			}
		}
		hp--;
	}

}
void Squirt::doSomething()
{
	// moves water in the right direction until we hit ice
	if (hp > 0)
	{
		hp--;
		ourWorld->damageProtester();
		switch (getDirection())
		{
		case right:
			if (ourWorld->checkSquirt(getX() + 1, getY())) // checks that we dont hit ice
			{
				moveTo(getX() + 1, getY()); break;
			}
			else state = "dead";
		case left:
			if (ourWorld->checkSquirt(getX() - 1, getY())) // checks that we dont hit ice
			{
				moveTo(getX() - 1, getY()); break;
			}
			else state = "dead";
		case up:
			if (ourWorld->checkSquirt(getX(), getY() + 1)) // checks that we dont hit ice
			{
				moveTo(getX(), getY() + 1); break;
			}
			else state = "dead";
		case down:
			if (ourWorld->checkSquirt(getX(), getY() - 1))  // checks that we dont hit ice
			{
				moveTo(getX(), getY() - 1); break;
			}
			else state = "dead";
		}
	}
	else state = "dead";
}

void WaterPool::doSomething()
{
	if (hp < 0) { state = "dead"; }

	hp--;

	if ((ourWorld->distance(getX(), getY(), ourWorld->getIceman()->getX(),
		ourWorld->getIceman()->getY()) <= 3.00))
	{
		state = "dead";
		ourWorld->playSound(SOUND_GOT_GOODIE);
		setPoints(getIID());
	}
}

void WaterPool::setPoints(int id)
{
	switch (id)
	{
	// water pool has water increase and points
	case WATER_POOL:
		ourWorld->getIceman()->renewWater();
		ourWorld->getIceman()->setPointsWaterpool(); break;
	// sonar has sonar increase and points
	case SONAR:
		ourWorld->getIceman()->setSonarNum();
		ourWorld->getIceman()->setPointsSonar();
	}
}

void Protester::doSomething()
{
	if (checkFalling())
	{
		hp = 0;
		ourWorld->getIceman()->setPointsBoulder();
	}
	
	if (state == "gold")
	{
		rest = std::max(50, ((int)(100 - (ourWorld->getLevel()) * 10)));
		state = "search";
	}

	if (rest <= 0)
	{
		if (state == "leave")
		{
			getToEnd(getX(), getY());
			if ((getX() == 30) && (getY() == 60))
				state = "dead";
			return;
		}
		if (hp <= 0)
		{
			state = "leave";
			ourWorld->playSound(SOUND_PROTESTER_GIVE_UP);
		}
		if (state == "search")
		{
			//if (getIID() == HARD_CORE_PROTESTER)
			//{
			//	int x = getX(), y = getY();
			//	//future<int> fut = async(stepsFromIceman, &x, &y);
			//	//thread th(&Protester::stepsFromIceman, ref(x), ref(y), std::move(prms));
			//	if (stepsFromIceman(x, y) < (16 + (int)(ourWorld->getLevel()) * 2))
			//	{
			//		if (x > getX()) moveInDir(right);
			//		if (x < getX()) moveInDir(left);
			//		if (y > getY()) moveInDir(up);
			//		if (y < getY()) moveInDir(down);
			//	}
			//}
			// coming in contact of iceman
			if (facingIceman())
			{
				ourWorld->playSound(SOUND_PROTESTER_YELL);
				ourWorld->getIceman()->annoy();
				state = "annoying";
				yelled = 15;
			}
			else
			{
				bool skip = false; // tells if we can skip a bit of code
				// moving towards iceman if in line of sight
				if ((ourWorld->distance(getX(), getY(), ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY())) >= 4)
				{
					if (checkIceman(right)) { moveInDir(right); skip = true; }
					else if (checkIceman(left)) { moveInDir(left); skip = true; } 
					else if (checkIceman(up)) { moveInDir(up); skip = true; }
					else if (checkIceman(down)) { moveInDir(down); skip = true; }
				}
				if (!skip)
				{
					if ((numMove > 0) && (iceInTheWay(getDirection()))) // no ice in the way and we can move
						moveInDir(getDirection());
					else
					{
						numMove = randGen(); Direction dir; randGen(dir);
						while (!iceInTheWay(dir)) randGen(dir); // runs until we get a direction where ice is not in the way
						moveInDir(dir);
					}
				}
			}
			rest = std::max(0, (int)((3 - ourWorld->getLevel()) / 4)); return;
		}

		if (state == "annoying")
		{
			if (yelled == 0) state = "search";
			yelled--;
		}

		if (state == "stunned")
		{
			yelled--;
			if (yelled < 0) state = "search";
		}
		rest--;
	}
}

bool Protester::facingIceman()
{
	switch (getDirection())
	{
	case up:
		if (checkItemsLoop(getX(), getY() + 1, ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY())) 
			return true;
		break;
	case down:
		if (checkItemsLoop(getX(), getY() - 1, ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY())) 
			return true;
		break;
	case right:
		if (checkItemsLoop(getX() + 1, getY(), ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY()))
			return true;
		break;
	case left:
		if (checkItemsLoop(getX() - 1, getY(), ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY()))
			return true;
		break;
	}
	return false;
}

// checks for ice in direction, and iceman in direction
bool Protester::checkIceman(Direction dir)
{
	switch (dir)
	{
	case up:
		for (int i = 0; i < 64; i++)
		{
			if (getY() + i + 4 >= 60) return false; // out of scope
			if (!ourWorld->iceInDir(getX(), getY() + i + 4, "top")) return false; // checks if ice is in the way
			if (checkItemsLoop(getX(), getY() + 4 + i, ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY()))
			{
				return true; //checks if we run into iceman
				break;
			}
		}
	case down:
		for (int i = 0; i < 64; i++)
		{
			if (getY() - i < 0) return false; // out of scope
			if (!ourWorld->iceInDir(getX(), getY() - i, "top")) return false; // checks if ice is in the way
			if (checkItemsLoop(getX(), getY() - i, ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY()))
			{
				return true; //checks if we run into iceman
				break;
			} 
		}
	case right:
		for (int i = 0; i < 64; i++)
		{
			if (getX() + i + 4 == 60) return false; // out of scope
			if (!ourWorld->iceInDir(getX() + i + 4, getY(), "side")) return false; // checks if ice is in the way
			if (checkItemsLoop(getX() + 4 + i, getY(), ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY()))
			{
				return true; //checks if we run into iceman
				break;
			}
		}
	case left:
		for (int i = 0; i < 64; i++)
		{
			if (getX() - i < 0) return false; // out of scope
			if (!ourWorld->iceInDir(getX() - i, getY(), "side")) return false; // checks if ice is in the way
			if (checkItemsLoop(getX() - i, getY(), ourWorld->getIceman()->getX(), ourWorld->getIceman()->getY()))
			{
				return true; //checks if we run into iceman
				break;
			}
		}
	}
	return false;
}

int Protester::randGen()
{
	return rand() % 52 + 8;
}

void Protester::randGen(Direction& dir)
{
	int x = rand() % 4;
	switch (x)
	{
	case 0: dir = up; break;
	case 1: dir = down; break;
	case 2: dir = left; break;
	case 3: dir = right; break;
	}

}

void Protester::moveInDir(Direction dir)
{
	switch (dir)
	{
	case right:
		setDirection(right);
		moveTo(getX() + 1, getY()); break;
	case left:
		setDirection(left);
		moveTo(getX() - 1, getY()); break;
	case up:
		setDirection(up);
		moveTo(getX(), getY() + 1); break;
	case down:
		setDirection(down);
		moveTo(getX(), getY() - 1); break;
	}
}

bool Protester::iceInTheWay(Direction dir)
{
	switch (dir)
	{
	case up:
		return ourWorld->iceInDir(getX(), getY() + 4, "top");
	case down:
		return ourWorld->iceInDir(getX(), getY() - 1, "top");
	case left:
		return ourWorld->iceInDir(getX() - 1, getY(), "side");
	case right:
		return ourWorld->iceInDir(getX() + 4, getY(), "side");
	}
}

void Protester::hitWWater()
{
	ourWorld->playSound(SOUND_PROTESTER_ANNOYED);
	rest = std::max(50, 100 - ((int)ourWorld->getLevel() * 10));
	hp--;
	state = "stunned";
	yelled = std::max(50, (int)(100 - (ourWorld->getLevel() * 10)));
	if (hp == 0) ourWorld->getIceman()->setPointsWaterpool();
}

void Protester::getToEnd(int ox, int oy)
{
	bool search[64][64], store;
	int x = ox, y = oy;
	// creates a copy of ice but with bool
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
		{
			if (ourWorld->iceAt(i, j)) search[i][j] = true;
			else search[i][j] = false;
		}
	queue<pair<int, int>> q;
	search[x][y] = true;
	//q.push({ x, y });
	while ((x != 30) || (y != 60))
	{
		if (x - 1 >= 0)
		{
			store = ourWorld->iceInDir(x - 1, y, "side");
			if ((store) && (!search[x - 1][y]))
			{
				x--;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop();
			}
		}
		if (x + 4 < 64)
		{
			store = ourWorld->iceInDir(x + 4, y, "side");
			if ((store) && (!search[x + 4][y]))
			{
				x++;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop();
			}
		}
		if (y - 1 <= 0)
		{
			store = ourWorld->iceInDir(x, y - 1, "top");
			if ((store) && (!search[x][y - 1]))
			{
				y--;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop();
			}
		}
		if (y + 4 < 64)
		{
			store = ourWorld->iceInDir(x, y + 4, "top");
			if ((store) && (!search[x][y + 4]))
			{
				y++;
				q.push({ x , y });
				search[x][y] = true;
				continue;
			}
			else
			{
				q.pop();
			}
		}
	}
  	ox = q.front().first;
	oy = q.front().second;
	moveTo(ox, oy);
}

int Protester::stepsFromIceman(int& x, int& y)
{
	return ourWorld->distanceFromIceman(x, y);
}
