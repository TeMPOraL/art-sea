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
		velocity(0,0,0),
		isSetMyOwnDirection(false)
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
	Ogre::Vector3 getForce()
	{
		return _myForce;
	}

	void updateFriendsPosition(Ogre::Vector3 position)
	{
		_friendsPosition+=position;
	}
	void updateTooCloseFriends(Ogre::Vector3 position)
	{
		_tooCloseFriendsPosition+=position;
	}

	void updateFriendsDirection(Ogre::Vector3 friendForce)
	{
		_friendsDirection+=friendForce;
	}
	void updateMyPosition(float centerFactor,float directionFactor,Ogre::Vector3 direction,float frictionFactor,Ogre::Real deltaT);
	void init()
	{
		_friendsPosition=Ogre::Vector3(0,0,0);
		_tooCloseFriendsPosition=Ogre::Vector3(0,0,0);
		_friendsDirection=Ogre::Vector3(0,0,0);
		howManyVisibleFriends=0;
		howManyTooCloseFriends=0;
	}
	void incrementVisibleFriends()
	{
		++howManyVisibleFriends;
	}
	void incrementTooCloseFriends()
	{
		++howManyTooCloseFriends;
	}

protected:
	Ogre::Vector3 _myPosition;
	Ogre::Vector3 _friendsDirection;
	Ogre::Vector3 _friendsPosition;
	Ogre::Vector3 _myForce;
	Ogre::Vector3 velocity;
	Ogre::Vector3 friction;
	Ogre::Vector3 _tooCloseFriendsPosition;
	int howManyVisibleFriends,howManyTooCloseFriends;
	int m;
	Ogre::Vector3 _myOwnDirection;
	bool isSetMyOwnDirection;

};

//====================================================
// Flock
//====================================================
class Flock
{
public:
	Flock(unsigned int size,float centerFactor,float resolutionFactor,float directionFactor,float friction, int flockVisibility,int terytoryCenter,int terytorySize,int minDistance);
	
	unsigned int getSize()
	{
		return _size;
	}
	
	std::vector<Fish*>& getAllFish()
	{
		std::vector<Fish*>& fishPointer=fish;
		return fishPointer;
	}
	void updateAllFish(Ogre::Real deltaT,float centerF,float directionF,float friction,float minDistance,int visibility);
	
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
	float _minDistance;
	Ogre::Vector3 direction;
	bool isDirectionSet;
	std::vector<Fish*>fish;

};

#endif 