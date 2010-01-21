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
//static const float k=0.3;
//====================================================
// Fish
//====================================================
class Fish
{
public:

	Fish(Ogre::Vector3 position,int hunger=0):
	  myNearestFriendsCenter(0,0,0),
	  visibleFlockDirection(0,0,0),
	  howManyVisible(0),
	  howManyCloseFriends(0),
	  velocity(0,0,0),
	  myHunger(hunger),
	  myPosition(position),
	  myForce(Ogre::Vector3(Fish::getRandomVector()))
	{}

	Ogre::Vector3 getPosition()
	{
		return myPosition;
	}

	Ogre::Vector3 getMyNearestFriendDirection()
	{
		return myNearestFriendsCenter;
	}

	Ogre::Vector3 getForce()
	{
		return myForce;
	}

	Ogre::Vector3 getVibibleFlockDirection()
	{
		return visibleFlockDirection;
	}

	void updatemyNearestFriendsCenter(Ogre::Vector3 direction)
	{	
		if(howManyCloseFriends>1)
		{
			myNearestFriendsCenter*=(howManyCloseFriends-1);
			this->myNearestFriendsCenter+=direction;
			myNearestFriendsCenter/=howManyCloseFriends;
		}
		else
		{
			this->myNearestFriendsCenter+=direction;
		}

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

	//visibleFlockCenter - vector between this fish and the other fish (visible from 
	//'his' point of view
	void updateVisibleFlockCenter(Ogre::Vector3 meFriendDistance) 
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

	void updateVisiblePreys(Ogre::Vector3 mePreyVector)
	{
		if(mePreyVector.length()>0.5)
		{
			visiblePreys+=ESCAPING_CONSTANT/(mePreyVector*mePreyVector);
		}
	}
	void updateVisiblePredators(Ogre::Vector3 mePredatorVector)
	{
		if(mePredatorVector.length()>0.5)
		{
			visiblePredators+=ESCAPING_CONSTANT/(mePredatorVector*mePredatorVector);
		}
	}
	void updateEscapeFromCamera(Ogre::Vector3 meCameraVector)
	{
		cameraEscapeForce=-meCameraVector;
	}

	//force is NORMALIZED sum of COMPONENTS: resolution - not stick too close to the nearest friends, 
	//flock center - all fish try to go to the center, flock direction - all fish try to follow
	//the direction AND FRICTION FORCE
	void calculateForce(float flockDirectionFactor,float resolutionFactor,float flockCenterFactor,float frictionFactor,float cameraFactor, Ogre::Real deltaT)
	{
		/**if(myNearestFriendsCenter.length()> 0.0001f)
		{
			myNearestFriendsCenter.normalise();
		}
		myNearestFriendsCenter*=SPEED_FACTOR;

		if(visibleFlockDirection.length()> 0.0001f)
		{
			//myForce/=myForce.length();
			visibleFlockDirection.normalise();
		}
		visibleFlockDirection*=SPEED_FACTOR;

		if(visibleFlockCenter.length()> 0.0001f)
		{
			//myForce/=myForce.length();
			visibleFlockCenter.normalise();
		}
		visibleFlockCenter*=SPEED_FACTOR;*/
		
		int howBigForceToPut=SPEED_FACTOR;

		//if the fish doesn't see anybody and doesn't know where to go it takes a random 
		//direction. Since it has a direction, even if it doesn't see anybody it should go
		//this direction
		if(howManyVisible==0 && isSetMyOwnDirection==false)
		{
			isSetMyOwnDirection=true;
			myForce=getRandomVector();
			howBigForceToPut=100;
		}
		else
		{
			ARTSEA_LOG<<"direction"<<visibleFlockDirection<<"center"<<visibleFlockCenter<<"res"<<myNearestFriendsCenter;
			isSetMyOwnDirection=false;
			myForce=(flockDirectionFactor*visibleFlockDirection+flockCenterFactor*visibleFlockCenter+cameraFactor*cameraEscapeForce); 
		}
		Ogre::Vector3 friction=frictionFactor*velocity; 
		myForce-=friction; 
		//myForce/=myForce.length();
		myForce.normalise();
		myNearestFriendsCenter.normalise();
		myForce-=resolutionFactor*myNearestFriendsCenter;
		myForce.normalise();
		myForce+=10*visiblePreys; //how much going prey's direction
		if(visiblePredators!=Ogre::Vector3::ZERO)
		{
			myForce=-visiblePredators;
		}
		
		myForce.normalise();
		if(visiblePredators!=Ogre::Vector3::ZERO)
		{
			myForce*=(2*howBigForceToPut); //escaping speed
		}
		else
		{
			myForce*=howBigForceToPut;
		}
		if(visiblePreys!=Ogre::Vector3::ZERO)
		{
			ARTSEA_LOG<<"sa ofiary"<<myForce;
		}

		//ARTSEA_LOG<<"how many close"<<howManyCloseFriends;
	}	

	// update position based on myForce - physics model
	void updatePosition(Ogre::Real deltaT)
	{
		this->myPosition += velocity*deltaT;
		this->velocity += (myForce/m)*deltaT;
	}

	void initFishDataBeforeUpdating()
	{
		visibleFlockCenter=Ogre::Vector3::ZERO;
		visibleFlockDirection=Ogre::Vector3::ZERO;
		myNearestFriendsCenter=Ogre::Vector3::ZERO;
		visiblePredators=Ogre::Vector3::ZERO;
		visiblePreys=Ogre::Vector3::ZERO;
		cameraEscapeForce=Ogre::Vector3::ZERO;
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

protected:
	static Ogre::Vector3 getRandomVector()
	{
		int x=rand()%(2*RANDOM_VECTOR_LENGTH)-RANDOM_VECTOR_LENGTH;
		int y=rand()%(2*RANDOM_VECTOR_LENGTH)-RANDOM_VECTOR_LENGTH;
		int z=rand()%5 - 5;
		return Ogre::Vector3(x,y,z);
	}

protected:

	Ogre::Vector3 myPosition; // fish position
	Ogre::Vector3 velocity;	//velocity of fish
	Ogre::Vector3 visibleFlockDirection; // flock direction visible from thisfish point of view
	Ogre::Vector3 myNearestFriendsCenter; //show the point where the closest fish are - used in resolution
	Ogre::Vector3 visibleFlockCenter; //flock center from this fish point of view
	Ogre::Vector3 visiblePreys;
	Ogre::Vector3 visiblePredators;
	Ogre::Vector3 myOwnDirection;
	Ogre::Vector3 cameraEscapeForce;
	Ogre::Vector3 myForce; //force whish cause fish movements
	int howManyVisible; //how many othe fish this fish can see
	int howManyCloseFriends; // how many close frends he has
	int myHunger; //how hungry th fish is
	static const int m=1; // weight right now the same for all fish
	bool isSetMyOwnDirection;
};

//====================================================
// Flock
//====================================================
class Flock
{
public:
	Flock():
		visibility(DEFAULT_FLOCK_SIZE), 
		flockSize(DEFAULT_VISIBILITY),
		flockDirectionFactor(DEFAULT_DIRECTION_FACTOR),
		resolutionFactor(DEFAULT_RESOLUTION_FACTOR),
		flockCenterFactor(DEFAULT_CENTER_FACTOR),
		friction(DEFAULT_FRICTION_FACTOR)
	{
	}
	~Flock(){};

	//myPositions of fish in the flock setting randomly
	Flock(int howMany,float directionFactor,float resolutionFactor,float centerFactor,float frictionFactor,float camFactor,int flockVisibility=DEFAULT_VISIBILITY):
		flockSize(howMany),
		visibility(flockVisibility),
		flockDirectionFactor(directionFactor),
		resolutionFactor(resolutionFactor),
		flockCenterFactor(centerFactor),	
		friction(frictionFactor),
		cameraFactor(camFactor)
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
		return fishInTheFlock;
	}

	void updateAllFish(Ogre::Real deltaT,float direction,float resolution,float center,float friction,float cameraFactor, Ogre::Camera * camera);

	void addPredator(Flock * predator)
	{
		myPredators.push_back(predator);
	}

	void addPrey(Flock * prey)
	{
		myPreys.push_back(prey);
	}

	const std::vector<Flock*>& getPredators()
	{
		return myPredators;
	}

	const std::vector<Flock*>& getPreys()
	{
		return myPreys;
	}
	void setVisibility(int visibility)
	{
		this->visibility=visibility;
	}
	int getVisiblity()
	{
		return this->visibility;
	}

private:
	void createFish(Ogre::Vector3 myPosition);
	//TODO optimalization problem; the fact that one fish can see another
	//doesn't mean the second one can see the first one. For right now - ok; later - to change
	// this function is to optimalize algorithms. Can be used only 
										  // for fish from the same folk==fish having the same visibility
protected:
	bool canSeeEachOther(Fish *a, Fish *b)
	{
		if(getSquaredDistance(a,b)<getFlockVisibility()*getFlockVisibility())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

protected:
	std::vector<Fish*> fishInTheFlock;	//fish which belong to this flock
										//TODO  right know fish can see everywhere around; change it!
	int visibility;				//how far each fish can see; the same for all fish from the flock;
								//fish can see everywhere arund in the sphere. visibility=radius of this sphere
	int flockSize;
	float flockDirectionFactor,resolutionFactor,flockCenterFactor,cameraFactor;
	float friction;
	std::vector<Flock*> myPredators; //pointers to flocks whisch run for me
	std::vector<Flock*> myPreys; 
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
};

#endif 