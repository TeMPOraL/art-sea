#include<cstdio>
#include<vector>
#ifndef _artSea_Simulation_
#define _artSea_Simulation_
      
class Flock;
 
//====================================================
// Position
//====================================================
class Position
{
public:
	Position() 
	{
		x=0; 
		y=0; 
		z=0;
	}
	Position(float x, float y, float z)
	{
		this->x=x;
		this->y=y;
		this->z=z;
	}
	~Position(){};
	float getX()
	{
		return this->x;
	}
	float getY()
	{
		return this->y;
	}
	float getZ()
	{
		return this->z;
	}

private:
	float x,y,z;
};

//====================================================
// Fish
//====================================================
class Fish
{
public:
	Fish(Flock *myFlock)
	{
		this->hunger=0;
		this->position=new Position(0,0,0);
		this-> myFlock=myFlock;
	}
	Fish(int hunger,Position position, Flock * fl)
	{
		this->hunger=hunger;
		this->position=new Position(position.getX(),position.getY(),position.getZ());
		this->myFlock =fl;
	}
	void upateFriend(Fish fish);
	void updateEnemy(Fish fish);

private:
	Position  *position;
	int hunger;
	Flock * myFlock;
		
};

//====================================================
// Flock
//====================================================
class Flock
{
public:
	Flock()
	{
		visibility=0;
		myFishNumber=0;
	};
	Flock(int howMany, int visibility)
	{
		this->myFishNumber=howMany;
		this->visibility=visibility;
	}
	
	void createAllFish();
	void updateAllFish();
	~Flock(){};

private:
	void createFish();

private:
	std::vector<Fish*> myFish;	//fish which belong to this flock
	//TODO  right know fish can see everywhere around change it!
	int visibility;				//how far each fish can see
	int myFishNumber;
};
 
#endif 