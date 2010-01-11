#include<cstdio>
#include<vector>
#include<ogre.h>
#include"debug.h"
#ifndef _artSea_Simulation_
#define _artSea_Simulation_
   
static const Ogre::Real INFINITE_DISTANCE = 500000;
static const int DEFAULT_VISIBILITY=70;
static const int CLOSE_FRIENDS_DISTANCE=5;
static const int DEFAULT_FLOCK_SIZE=50;
static const int RANDOM_VECTOR_LENGTH=2000;

static int counter=0;
static const float k=0.3;
//====================================================
// Fish
//====================================================
class Fish
{
public:
/**	Fish(): 
	  hunger(0),
	  position(Ogre::Vector3(0,0,0)),
	  //force(Ogre::Vector3(Fish::getRandomVector())),
	  myNearestFriendsDirection(Ogre::Vector3::ZERO),
	  visibleFlockDirection(Ogre::Vector3::ZERO),
	  howManyVisible(0),
	  howManyCloseFriends(0) // we have ok force after initialization
	  {} */

	Fish(Ogre::Vector3 position,int hunger=0):
	 // force(Ogre::Vector3(Fish::getRandomVector()),
	  myNearestFriendsDirection(0,0,0),
	  visibleFlockDirection(0,0,0),
	  howManyVisible(0),
	  howManyCloseFriends(0),
	  velocity(0,0,0)
	{
		this->hunger=hunger; // cannot put it on initialization list because I call fish()
							 // fish() init list would cover my initialization
		this->position=position;
		this->force= Ogre::Vector3(Fish::getRandomVector());
		//ARTSEA_LOG<<"dora"<<force;
		tmp=false;
	}
	//void upateFriend(Fish fish);
	//void updateEnemy(Fish fish);
	Ogre::Vector3 getPosition()
	{
		return position;
	}
	void updateMyNearestFriendsDirection(Ogre::Vector3 direction)
	{	
		if(howManyCloseFriends>1)
		{
			myNearestFriendsDirection*=(howManyCloseFriends-1);
			this->myNearestFriendsDirection+=direction;
			myNearestFriendsDirection/=howManyCloseFriends;
		}
		else
		{
			this->myNearestFriendsDirection+=direction;
		}
	}
	Ogre::Vector3 getMyNearestFriendDirection()
	{
		return myNearestFriendsDirection;
	}
	//flock direction from this fish's point of view; based on it's visibility
	void updateFlockDirection(Ogre::Vector3 visibleFriendDirection)
	{
		if(howManyVisible>1)
		{
			visibleFlockDirection*=(howManyVisible-1);
			this->visibleFlockDirection+=visibleFriendDirection;
			visibleFlockDirection/=howManyVisible;
		}
		else
		{
			this->visibleFlockDirection+=visibleFriendDirection;
		}
	}
	Ogre::Vector3 getVibibleFlockDirection()
	{
		return visibleFlockDirection;
	}
	//myNearestFriendsDirection - vector between the fish and it's friends
	//resolution - fish doesn't want to be too close to it's friend's; goes oposite direction
	//all fish try to go the same direction = visibleFlockDirection
	void calculateForce(float flockDirectionFactor,float resolutionFactor,float flockCenterFactor, Ogre::Real deltaT)
	{
		//ARTSEA_LOG<<"f "<<flockDirectionFactor<<" "<<resolutionFactor<<" "<<flockCenterFactor;
		//ARTSEA_LOG<<"wspaniale "<<visibleFlockDirection<<" "<<myNearestFriendsDirection<<" "<< visibleFlockCenter;
		//ARTSEA_LOG<<"how many visible "<<howManyVisible;
		force=(flockDirectionFactor*visibleFlockDirection-
		resolutionFactor*myNearestFriendsDirection+flockCenterFactor*visibleFlockCenter); 
		Ogre::Vector3 friction=k*velocity; 
		force-=friction; 
		
		//fish orientation test - ogre module
		/**if(force.x>=100)
		{
			tmp=true;
		}
		if(tmp==true)
		{
			force-=Ogre::Vector3(20,0,0);
		}
		else
		{
			force+=Ogre::Vector3(20,0,0);
		}*/
	}
	Ogre::Vector3 getForce()
	{
		return force;
	}
	void updatePosition(Ogre::Real deltaT)
	{
		this->position += velocity*deltaT;
		this->velocity += (force/m)*deltaT;
	}
	void updateVisibleFlockCenter(Ogre::Vector3 meFriendDistance) //vector between this fish and the other visible fish
	{
		if(howManyVisible>1)
		{
			visibleFlockCenter*=(howManyVisible-1);
			visibleFlockCenter+=meFriendDistance;
			visibleFlockCenter/=howManyVisible;
		}
		else
		{
			visibleFlockCenter+=meFriendDistance;
		}
		
	}
	void initForces()
	{
		visibleFlockCenter=Ogre::Vector3::ZERO;
		visibleFlockDirection=Ogre::Vector3::ZERO;
		myNearestFriendsDirection=Ogre::Vector3::ZERO;
		howManyCloseFriends=0;
		howManyVisible=0;
	}
	void incrementVisibleFish()
	{
		++howManyVisible;
	}
	void incrementCloseFriends()
	{
		++howManyCloseFriends;
	}
private:
	static Ogre::Vector3 getRandomVector()
	{
		int x=rand()%(2*RANDOM_VECTOR_LENGTH)-RANDOM_VECTOR_LENGTH;
		int y=rand()%(2*RANDOM_VECTOR_LENGTH)-RANDOM_VECTOR_LENGTH;
		int z=rand()%30 - 15;
		return Ogre::Vector3(x,y,z);

	}
private:
	Ogre::Vector3 position;
	Ogre::Vector3 velocity;	//velocity of fish
	Ogre::Vector3 visibleFlockDirection;
	Ogre::Vector3 myNearestFriendsDirection;
	Ogre::Vector3 visibleFlockCenter;
	Ogre::Vector3 force; // computed; based on floclDirection and NearestFriends
	int howManyVisible;
	int howManyCloseFriends;
	int hunger;
	static const int m=1; // weight right now the same for all fish
	bool tmp;
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
	Flock(int howMany,float directionFactor,float resolutionFactor,float centerFactor,float frictionFactor,int vis=DEFAULT_VISIBILITY):
		flockSize(howMany),
		visibility(vis),
		flockDirectionFactor(directionFactor),
		resolutionFactor(resolutionFactor),
		flockCenterFactor(centerFactor),	
		friction(frictionFactor)
	{
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
	Ogre::Real getSquaredDistance(Fish *a, Fish *b)
	{ 
		return a->getPosition().squaredDistance(b->getPosition());		
	}
	//TODO: check pointer. This returs cons reference to a vector, which has
	//pointers to fish. We cannot change pointer to this vector, but what about
	//fish? Check & make sure it's corrent. Everything const! Const references
	const std::vector<Fish*>& getAllFish()
	{
		std::vector<Fish*> & ref = fishInTheFlock;
		return ref;
	}
	void updateAllFish(Ogre::Real deltaT,float direction,float resolution,float center,float friction);
	~Flock(){};

private:
	void createFish(Ogre::Vector3 position);
	//TODO optimalization problem; the fact that one fish can see another
	//doesn't mean the second one can see the first one. For right now - ok; later - to change
 // this function is to optimalize algorithms. Can be used only 
										  // for fish from the same folk==fish having the same visibility
public:	//CHANGE TO PRIVATE!!!1										  //fish which can see everything around
	bool canSeeEachOther(Fish *a, Fish *b)
	{
		//Ogre::Vector3 posA=a->getPosition();
		if(getSquaredDistance(a,b)<getFlockVisibility()*getFlockVisibility())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	std::vector<Fish*> fishInTheFlock;	//fish which belong to this flock
	//TODO  right know fish can see everywhere around; change it!
	int visibility;				//how far each fish can see; the same for all fish from the flock;
								//fish can see everywhere arund in the sphere. visibility=radius of this sphere
	int flockSize;
	float flockDirectionFactor,resolutionFactor,flockCenterFactor;
	float friction;
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
	static SimulationWorld* getSimulationWorld(int howManyFlocks, std::vector<int>& sizes,
		std::vector<float>&directions,std::vector<float>&resolutions,std::vector<float>&centers,
		std::vector<float>&frictions)
	{
		if(singletonFlag==false)
		{
			singleton= new SimulationWorld(howManyFlocks,sizes,directions,resolutions,centers,
			frictions);
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
		std::vector<float>&resolutions,std::vector<float>&centers,std::vector<float>&frictions);
	std::vector<Ogre::Vector3> & getAllFishPositions()
	{
		return allFishPositions;
	}
	std::vector<int> & getAllFishFlocks()
	{
		std::vector<int>&ref=allFishFlocks;
		return ref;
		//return allFishFlocks;
	}
	void setAllFishPositionsAndFlocks();

	void updateAllFish(Ogre::Real deltaT,std::vector<float>&direction,std::vector<float>&resolution,
		std::vector<float>&center,std::vector<float>&frictions);

private:
	SimulationWorld(int howManyFlocks, std::vector<int> & sizes,
		std::vector<float>&directions,std::vector<float>&resolutions,
		std::vector<float>&centers,std::vector<float>&frictions)
	{
		if(singleton==0)
		{
			setHowManyFlocks(howManyFlocks);
			createFlocks(getHowManyFlocks(),sizes,directions,resolutions,centers,frictions);
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
	std::vector<int> allFishFlocks; //allFishFlocks[i] - flock id of fish 'number' i; needed in ogre module
	//int directionFactor,resolutionFactor,centerFactor;
};

#endif 