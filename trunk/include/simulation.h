#include<cstdio>
#include<vector>
#include<ogre.h>
#include"debug.h"
#ifndef _artSea_Simulation_
#define _artSea_Simulation_
   
static const Ogre::Real INFINITE_DISTANCE = 500000;
static const int DEFAULT_VISIBILITY=70;
static const int CLOSE_FRIENDS_DISTANCE=30;
static const int DEFAULT_FLOCK_SIZE=50;
static const int RANDOM_VECTOR_LENGTH=500;
static const float DEFAULT_RESOLUTION_FACTOR=1;
static const float DEFAULT_DIRECTION_FACTOR=1; 
static const float DEFAULT_CENTER_FACTOR=0.3;
static const float DEFAULT_FRICTION_FACTOR=20;
static const int ESCAPING_CONSTANT=2;
static const int SPEED_FACTOR=5;
static int counter=0;

//TODO przekazujemy wskazniki do rybek nie const uwaac!!!!
//static const float k=0.3;
//====================================================
// Fish
//====================================================
class Fish
{
public:
	Fish(Ogre::Vector3 position):
		_myPosition(position),
		m(1),
		velocity(0,0,0)
		{
		}

	void setPosition(Ogre::Vector3 position)
	{
		_myPosition=position;
	}

	Ogre::Vector3 getPosition()
	{
		return _myPosition;
	}

	void updateFriendsPosition(Ogre::Vector3 position)
	{
		_friendsPosition+=position;
	}

	void updateMyPosition(float centerFactor,float frictionFactor,Ogre::Real deltaT)
	{
		if(howManyVisibleFriends>0) //see some friends
		{
			_friendsPosition/=howManyVisibleFriends;
			_myForce=centerFactor*(_friendsPosition-_myPosition);
		}
		else //doesn't see anybody
		{
			_myForce=Ogre::Vector3(0,0,0);
		}
		friction=frictionFactor*velocity;
		_myForce.normalise();
		_myForce-=friction;
		_myForce*=10;
		_myPosition+=velocity*deltaT;
		velocity+=(_myForce/m)*deltaT;
		/**_myForce=centerFactor*(_friendsPosition-_myPosition); //centerForce = makes me going to the center
		
		//friction=frictionFactor*velocity;
		//_myForce-=friction;
		_myForce.normalise();
		_myForce*=5;
		Ogre::Vector3 oldPos=_myPosition;
		_myPosition+=velocity*deltaT;
		velocity+=(_myForce/m)*deltaT;*/
	}
	void init()
	{
		_friendsPosition=Ogre::Vector3(0,0,0);
		howManyVisibleFriends=0;
	}
	void incrementVisibleFriends()
	{
		++howManyVisibleFriends;
	}

protected:
	Ogre::Vector3 _myPosition;
	Ogre::Vector3 _friendsPosition;
	Ogre::Vector3 _myForce;
	Ogre::Vector3 velocity;
	Ogre::Vector3 friction;
	int howManyVisibleFriends;
	int m;

};

//====================================================
// Flock
//====================================================
class Flock
{
public:
	Flock(unsigned int size,float centerFactor,float resolutionFactor,float directionFactor,float friction, int flockVisibility,int terytoryCenter,int terytorySize);
	
	unsigned int getSize()
	{
		return _size;
	}
	
	std::vector<Fish*>& getAllFish()
	{
		std::vector<Fish*>& fishPointer=fish;
		return fishPointer;
	}
	void updateAllFish(Ogre::Real deltaT);
	
protected:
	void placeAllFish();
protected:
	unsigned int _size;
	float _centerFactor;
	float _resolutionFactor;
	float _directionFactor;
	int _flockVisibility;
	int _startingTerytoryCenter;
	int _startignTerytorySize;
	float _frictionFactor;
	std::vector<Fish*>fish;

};

//====================================================
// SimulationWorld (singleton)
//====================================================
/**class SimulationWorld
{
public:

	~SimulationWorld()
	{
		assert(singleton!=0);
		singletonFlag=false;
	}

	static SimulationWorld* getSimulationWorld(int howManyFlocks, std::vector<int>& sizes,
		std::vector<float>&directions,std::vector<float>&resolutions,std::vector<float>&centers,
		std::vector<float>&frictions, std::vector<float>&cameraFactors)
	{
		if(singletonFlag==false)
		{
			singleton= new SimulationWorld(howManyFlocks,sizes,directions,resolutions,centers,
			frictions,cameraFactors);
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

	void createFlocks(int howMany,std::vector<int>&sizes,std::vector<float>&directions,
		std::vector<float>&resolutions,std::vector<float>&centers,std::vector<float>&frictions,std::vector<float>&cameraFactors);
	
	std::vector<Ogre::Vector3> & getAllFishPositions()
	{
		return allFishPositions;
	}

	std::vector<int> & getAllFishFlocks()
	{
		return allFishFlocks;
	}

	void setAllFishPositionsAndFlocks();

	void updateAllFish(Ogre::Real deltaT,std::vector<float>&direction,std::vector<float>&resolution,
		std::vector<float>&center,std::vector<float>&frictions,std::vector<float>& cameraFactors,Ogre::Camera*camera);

	void addInteraction(int predator, int prey) // predator's id and prey's id
	{
		flocks[predator]->addPrey(flocks[prey]);
		flocks[prey]->addPredator(flocks[predator]);
	}


private:
	SimulationWorld(int howManyFlocks, std::vector<int> & sizes,
		std::vector<float>&directions,std::vector<float>&resolutions,
		std::vector<float>&centers,std::vector<float>&frictions,std::vector<float>&cameraFactors)
	{
		if(singleton==0)
		{
			setHowManyFlocks(howManyFlocks);
			createFlocks(getHowManyFlocks(),sizes,directions,resolutions,centers,frictions,cameraFactors);
			setAllFishPositionsAndFlocks();
			singletonFlag=true;
		}
	}

private:
	static SimulationWorld *singleton;
	static bool singletonFlag;
	std::vector<Flock*>flocks;
	int howManyFlocks;
	std::vector<Ogre::Vector3>allFishPositions; //allFishmyPositions[i] - myPositions of fish 'number' i; needed in ogre module
	std::vector<int> allFishFlocks; //allFishFlocks[i] - flock id of fish 'number' i; needed in ogre module
};*/

#endif 