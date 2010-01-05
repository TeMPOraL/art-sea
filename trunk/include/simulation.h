#include<cstdio>
#include<vector>
#include<ogre.h>
#ifndef _artSea_Simulation_
#define _artSea_Simulation_
   
const Ogre::Real INFINITE_DISTANCE = 500000;
//====================================================
// Fish
//====================================================
class Fish
{
public:
	Fish(): 
	  hunger(0),
	  position(Ogre::Vector3(0,0,0)),
	  myClosestFriendDistance(INFINITE_DISTANCE),
	  myDirection(Ogre::Vector3::ZERO),
	  myNearestFriendDirection(Ogre::Vector3::ZERO),
	  visibleFlockDirection(Ogre::Vector3::ZERO)
	{ }
	Fish(Ogre::Vector3 position,int hunger=0)
	{
		Fish();
		this->hunger=hunger; // cannot put it on initialization list because I call fish()
							 // fish() init list would cover my initialization
		this->position=position;
	}
	void upateFriend(Fish fish);
	void updateEnemy(Fish fish);
	Ogre::Vector3 getPosition()
	{
		return position;
	}
	Ogre::Vector3 getNextPosition()
	{
		return nextPosition;
	}
	Ogre::Vector3 getDirection()
	{
		return myDirection;
	}
	void setMyNearestFriendDirection(Ogre::Vector3 direction)
	{
		myNearestFriendDirection=direction;
	}
	Ogre::Vector3 getMyNearestFriendDirection()
	{
		return myNearestFriendDirection;
	}
	//flock direction from this fish's point of view; based on it's visibility
	void updateFlockDirection(Ogre::Vector3 visibleFriendDirection)
	{
		visibleFlockDirection+=visibleFriendDirection;
	}
	Ogre::Vector3 getVibibleFlockDirection()
	{
		return visibleFlockDirection;
	}
	Ogre::Real getMyClosestFriendDistance()
	{
		return myClosestFriendDistance;
	}
	void setMyClosestFriendDistance(Ogre::Real distance)
	{
		myClosestFriendDistance=distance;
	}

	void calculateNextPosition()
	{
		Ogre::Vector3 movement;
		movement=(-1*myNearestFriendDirection)+visibleFlockDirection;
		nextPosition=position+movement;
	}


private:
	Ogre::Vector3 position;
	Ogre::Vector3 nextPosition;
	Ogre::Vector3 myDirection;
	Ogre::Vector3 visibleFlockDirection;
	Ogre::Vector3 myNearestFriendDirection;
	Ogre::Real myClosestFriendDistance;
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
	const std::vector<Fish*const >& getAllFish()
	{
		std::vector<Fish* const> & ref = fishInTheFlock;
		return ref;
	}
	void updateAllFish();
	~Flock(){};

private:
	void createFish(Ogre::Vector3 position);
	//TODO optimalization problem; the fact that one fish can see another
	//doesn't mean the second one can see the first one. For right now - ok; later - to change
	bool canSeeEachOther(Fish a, Fish b); // this function is to optimalize algorithms. Can be used only 
										  // for fish from the same folk==fish having the same visibility
										  //fish which can see everything around

private:
	std::vector<Fish* const> fishInTheFlock;	//fish which belong to this flock
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
	static SimulationWorld* getSimulationWorld(int howManyFlocks, std::vector<int>& sizes)
	{
		if(singletonFlag==false)
		{
			singleton= new SimulationWorld(howManyFlocks,sizes);
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
	void showWorld();
	void createFlocks(int howMany,std::vector<int>&sizes);
	std::vector<Ogre::Vector3> & getAllFishPositions()
	{
		std::vector<Ogre::Vector3>& temp=allFishPositions;
		return temp;
	}
	std::vector<Ogre::Vector3> & getAllFishNextPositions()
	{
		std::vector<Ogre::Vector3>& temp=allFishNextPositions;
		return temp;
	}
	std::vector<int> & getAllFishFlocks()
	{
		return allFishFlocks;
	}
	void setAllFishPositionsAndFlocks();

	void updateAllFish();

private:
	SimulationWorld(int howManyFlocks, std::vector<int> & sizes)
	{
		if(singleton==0)
		{
			setHowManyFlocks(howManyFlocks);
			createFlocks(getHowManyFlocks(),sizes);
			setAllFishPositionsAndFlocks();
			singletonFlag=true;
		}
	}

private:
	static SimulationWorld *singleton;
	static bool singletonFlag;
	std::vector<Flock*>flocks;
	int howManyFlocks;
	std::vector<Ogre::Vector3>allFishPositions; //allFishPositions[i] - positions of fish 'number' i; needed in ogre module
	std::vector<Ogre::Vector3>allFishNextPositions;
	std::vector<int> allFishFlocks; //allFishFlocks[i] - flock id of fish 'number' i; needed in ogre module
};

#endif 