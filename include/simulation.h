#include<cstdio>
#include<vector>
#include<ogre.h>
#include"debug.h"
#ifndef _artSea_Simulation_
#define _artSea_Simulation_
   
static const Ogre::Real INFINITE_DISTANCE = 500000;
static const int DEFAULT_VISIBILITY=50;
static const int CLOSE_FRIENDS_DISTANCE=10;
static const int DEFAULT_FLOCK_SIZE=50;
static const int RANDOM_VECTOR_LENGTH=2000;

static int counter=0;
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
	  howManyCloseFriends(0)
	{
		this->hunger=hunger; // cannot put it on initialization list because I call fish()
							 // fish() init list would cover my initialization
		this->position=position;
		this->force= Ogre::Vector3(Fish::getRandomVector());
		//ARTSEA_LOG<<"dora"<<force;
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
		visibleFlockDirection+=visibleFriendDirection;
	}
	Ogre::Vector3 getVibibleFlockDirection()
	{
		return visibleFlockDirection;
	}
	//myNearestFriendsDirection - vector between the fish and it's friends
	//resolution - fish doesn't want to be too close to it's friend's; goes oposite direction
	//all fish try to go the same direction = visibleFlockDirection
	void calculateForce()
	{
		force=0.1*(visibleFlockDirection-myNearestFriendsDirection+visibleFlockCenter); //0.5 wspó³czynnik; fix it; tak wiem zabijesz mnie za to
		/**if(counter==0 && force!=Ogre::Vector3::ZERO)
		{
			ARTSEA_LOG<<"my force"<<"x"<<visibleFlockCenter.x<<"y"<<visibleFlockCenter.y<<"z"<<visibleFlockCenter.z;
			counter=1;
		}*/
		if(force==Ogre::Vector3::ZERO)
		{
			force=getRandomVector();
		}
	}
	Ogre::Vector3 getForce()
	{
		return force;
	}
	void updatePosition(Ogre::Real deltaT)
	{
		this->position+=(0.5*force*deltaT*deltaT); // physics m=1kg => |F|=|a|
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
		int x=rand()%2*RANDOM_VECTOR_LENGTH-RANDOM_VECTOR_LENGTH;
		int y=rand()%2*RANDOM_VECTOR_LENGTH-RANDOM_VECTOR_LENGTH;
		int z=rand()%2*RANDOM_VECTOR_LENGTH-RANDOM_VECTOR_LENGTH;
		return Ogre::Vector3(x,y,z);

	}
private:
	Ogre::Vector3 position;
	Ogre::Vector3 visibleFlockDirection;
	Ogre::Vector3 myNearestFriendsDirection;
	Ogre::Vector3 visibleFlockCenter;
	Ogre::Vector3 force; // computed; based on floclDirection and NearestFriends
	int howManyVisible;
	int howManyCloseFriends;
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
	Flock(int howMany, int visibility=DEFAULT_VISIBILITY)
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
	void updateAllFish(Ogre::Real deltaT);
	~Flock(){};

private:
	void createFish(Ogre::Vector3 position);
	//TODO optimalization problem; the fact that one fish can see another
	//doesn't mean the second one can see the first one. For right now - ok; later - to change
 // this function is to optimalize algorithms. Can be used only 
										  // for fish from the same folk==fish having the same visibility
										  //fish which can see everything around
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
		return allFishPositions;
	}
	std::vector<int> & getAllFishFlocks()
	{
		std::vector<int>&ref=allFishFlocks;
		return ref;
		//return allFishFlocks;
	}
	void setAllFishPositionsAndFlocks();

	void updateAllFish(Ogre::Real deltaT);

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
	std::vector<int> allFishFlocks; //allFishFlocks[i] - flock id of fish 'number' i; needed in ogre module
};

#endif 