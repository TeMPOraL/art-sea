#include<cstdio>
#include<vector>
#include<ogre.h>
#ifndef _artSea_Simulation_
#define _artSea_Simulation_
      
//====================================================
// Fish
//====================================================
class Fish
{
public:
	Fish()
	{
		this->hunger=0;
		this->position= Ogre::Vector3(0,0,0);
	}
	Fish(Ogre::Vector3 position,int hunger=0)
	{
		this->hunger=hunger;
		this->position=position;
	}
	void upateFriend(Fish fish);
	void updateEnemy(Fish fish);
	Ogre::Vector3 getPosition()
	{
		return position;
	}

private:
	Ogre::Vector3 position;
	Ogre::Vector3 nextPosition;
	int hunger;
};

//====================================================
// Flock
//====================================================
class Flock
{
public:
	Flock()
	{
		visibility=5; 
		flockSize=1;
	};
	//positions of fish in the flock setting randomly
	Flock(int howMany, int visibility)
	{
		this->flockSize=howMany;
		this->visibility=visibility;
	}
	
	void createAllFish();
	void updateAllFish();
	int getFlockVisibility()
	{
		return visibility;
	}
	int getFlockSize()
	{
		return flockSize;
	}
	Ogre::Real getSquaredDistance(Fish a, Fish b)
	{
		return a.getPosition().squaredDistance(b.getPosition());		
	}
	//TODO: check pointer. This returs cons reference to a vector, which has
	//pointers to fish. We cannot change pointer to this vector, but what about
	//fish? Check & make sure it's corrent. Everything const! Const references
	const std::vector<Fish*>& getAllFish()
	{
		return fishInTheFlock;
	}
	~Flock(){};

private:
	void createFish(Ogre::Vector3 position);
	//TODO optimalization problem; the fact that one fish can see another
	//doesn't mean the second one can see the first one. For right now - ok; later - to change
	bool canSeeEachOther(Fish a, Fish b); // this function is to optimalize algorithms. Can be used only 
										  // for fish from the same folk==fish having the same visibility
										  //fish which can see everything around

private:
	std::vector<Fish*> fishInTheFlock;	//fish which belong to this flock
	//TODO  right know fish can see everywhere around; change it!
	int visibility;				//how far each fish can see; the same for all fish from the flock;
								//fish can see everywhere arund in the sphere. visibility=radius of this sphere
	int flockSize;
};

//====================================================
// SimulationWorld (singleton)
//====================================================
class SimulationWorld
{
public:

	~SimulationWorld()
	{
		assert(singleton!=0);
		singletonFlag=false;
	}
	static SimulationWorld* getSimulationWorld()
	{
		if(singletonFlag==false)
		{
			singleton= new SimulationWorld();
		}
		return singleton;
	}
	void setHowManyFlocks(int howManyFlocks)
	{
		this->howManyFlocks=howManyFlocks;
	}
	int getHowManyFlocks()
	{
		return howManyFlocks;
	}
	void createFlocks(int howMany);
	std::vector<Fish*> getAllFish();
private:
	SimulationWorld()
	{
		if(singleton==0)
		{
			singletonFlag=true;
		}
	}

private:
	static SimulationWorld *singleton;
	static bool singletonFlag;
	std::vector<Flock*>flocks;
	int howManyFlocks;
};

#endif 