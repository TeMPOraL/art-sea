#include"simulation.h"
#include"Debug.h"
#include<time.h>

Ogre::Vector3 getRandomVector(int maxX,int maxY, int maxZ)
{
	float x=rand()%maxX - maxX/2;
	float y=rand()%maxY-maxY/2;
	float z=rand()%maxZ-maxZ/2;
	return Ogre::Vector3(x,y,z);
}
/**static const int SCREEN_X=500;
static const int SCREEN_Y=450;
static const int SCREEN_Z=5;*/

//====================================================
// Fish
//====================================================
void Fish::updateMyPosition(float centerFactor,float directionFactor,Ogre::Vector3 direction,float frictionFactor,Ogre::Real deltaT)
{

	if(howManyPredators>0) //escaping from the predator
	{
		isSetMyOwnDirection=false;
		_predatorsPos/=howManyPredators;
		_myForce=-(_predatorsPos-_myPosition);
	}
	else if(howManyVictims>0) //going after victims
	{
		isSetMyOwnDirection=false;
		_victimsPos/=howManyVictims;
		_myForce=(_victimsPos-_myPosition);
	}
	else	//if there is no danger
	{
		if(howManyVisibleFriends>0) //see some friends
		{
			isSetMyOwnDirection=false;
			_friendsPosition/=howManyVisibleFriends;
			_myForce=centerFactor*(_friendsPosition-_myPosition);
			_myForce+=directionFactor*direction;
		   
			if(howManyTooCloseFriends>0) // have at least one too close friend - need to keep distance
			{
				_tooCloseFriendsPosition/=howManyTooCloseFriends;
				_myForce=-(100*(_tooCloseFriendsPosition-_myPosition));
			}
		}
		else if(isSetMyOwnDirection==false) //doesn't have any friends; doesn't know wahat to do
		{
			isSetMyOwnDirection=true;
			_myForce=getRandomVector(1000,1000,50);
		}
	}
		friction=frictionFactor*velocity;
		_myForce.normalise();
		_myForce-=friction;
		_myForce*=10;
		_myPosition+=velocity*deltaT;
		velocity+=(_myForce/m)*deltaT;
}

//====================================================
// Flock
//====================================================

Flock::Flock(unsigned int size,float centerFactor,float resolutionFactor,float directionFactor,float friction, int flockVisibility,int terytoryCenter,int terytorySize,int minDistance):
		  _size(size),
		  _centerFactor(centerFactor),
		  _resolutionFactor(resolutionFactor),
		  _directionFactor(directionFactor),
		  _frictionFactor(friction),
		  _flockVisibility(flockVisibility),
		  _startingTerytoryCenter(terytoryCenter),
		  _startignTerytorySize(terytorySize),
		  _minDistance(minDistance)
{
	
	for(unsigned int i=0; i<size; ++i)
	{
		int x=terytoryCenter+rand()%(terytorySize)-(terytorySize/2);
		int y=terytoryCenter+rand()%(terytorySize)-(terytorySize/2);
		int z=rand()%(terytorySize)-(terytorySize/2);
		Ogre::Vector3 pos=Ogre::Vector3(x,y,z);
		fish.push_back(new Fish(pos));
	}
	isDirectionSet=false;
}
 

//update all fish based on the friends seen by him
void Flock::updateAllFish(Ogre::Real deltaT,float centerF,float directionF,float friction,float minDistance, int visibility)
{
	_minDistance=minDistance;
	_centerFactor=centerF;
	_directionFactor=directionF;
	_frictionFactor=friction;
	_flockVisibility=visibility;

	bool isEnemy=false;

		if(_directionFactor>0 && isDirectionSet==false)
		{
			isDirectionSet=true;
			direction=getRandomVector(100,100,50);
		}
		if(_directionFactor==0)
		{
			isDirectionSet=false;
			direction=Ogre::Vector3(0,0,0);
		}

		for(unsigned int i=0; i<fish.size(); ++i)
		{
			fish[i]->init();
		}
		for(unsigned int i=0; i<fish.size(); ++i)
		{
			for(unsigned int j=0; j<predators.size(); ++j)
			{
				if(fish[i]->getPosition().squaredDistance(predators[j]->getPosition())<
					(_flockVisibility*_flockVisibility))
				{
					fish[i]->incrementPredators();
					fish[i]->updatePredatorsPosition(predators[j]->getPosition());
					isEnemy=true;
				}
			}
			for(unsigned int j=0; j<victims.size(); ++j)
			{
				if(fish[i]->getPosition().squaredDistance(victims[j]->getPosition())<
					(_flockVisibility*_flockVisibility))
				{
					fish[i]->incrementVictims();
					fish[i]->updateVictimsPosition(victims[j]->getPosition());
					isEnemy=true;
				}
			}
			if(isEnemy==false)
			{
				for(unsigned int j=i+1; j<fish.size(); ++j)
				{
					if(fish[i]->getPosition().squaredDistance(fish[j]->getPosition())<
						(_flockVisibility*_flockVisibility)) // two fish can see each other
					{
						fish[i]->incrementVisibleFriends();
						fish[j]->incrementVisibleFriends();
						fish[i]->updateFriendsPosition(fish[j]->getPosition());
						fish[j]->updateFriendsPosition(fish[i]->getPosition());
						fish[i]->updateFriendsDirection(fish[j]->getForce());
						fish[j]->updateFriendsDirection(fish[i]->getForce());


						if(fish[i]->getPosition().squaredDistance(fish[j]->getPosition())<
								(_minDistance*_minDistance))
						{
							fish[i]->incrementTooCloseFriends();
							fish[j]->incrementTooCloseFriends();
							fish[i]->updateTooCloseFriends(fish[j]->getPosition());
							fish[j]->updateTooCloseFriends(fish[i]->getPosition());
						}
					}

				}
			}
			fish[i]->updateMyPosition(_centerFactor,_directionFactor,direction,_frictionFactor,deltaT);
		}
}

void Flock::addPreadator(Fish * fish)
{
	predators.push_back(fish);
}

void Flock::addVictim(Fish* fish)
{
	victims.push_back(fish);
}