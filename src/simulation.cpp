#include"simulation.h"
#include"Debug.h"
#include<time.h>
/**static const int SCREEN_X=500;
static const int SCREEN_Y=450;
static const int SCREEN_Z=5;*/

//====================================================
// Flock
//====================================================

Flock::Flock(unsigned int size,float centerFactor,float resolutionFactor,float directionFactor,float friction, int flockVisibility,int terytoryCenter,int terytorySize):
		  _size(size),
		  _centerFactor(centerFactor),
		  _resolutionFactor(resolutionFactor),
		  _directionFactor(directionFactor),
		  _frictionFactor(friction),
		  _flockVisibility(flockVisibility),
		  _startingTerytoryCenter(terytoryCenter),
		  _startignTerytorySize(terytorySize)
{
	
	for(unsigned int i=0; i<size; ++i)
	{
		int x=terytoryCenter+rand()%(terytorySize)-(terytorySize/2);
		int y=terytoryCenter+rand()%(terytorySize)-(terytorySize/2);
		int z=rand()%10-5;
		Ogre::Vector3 pos=Ogre::Vector3(x,y,z);
		fish.push_back(new Fish(pos));
	}
}
 

//update all fish based on the friends seen by him
void Flock::updateAllFish(Ogre::Real deltaT)
{
	for(unsigned int i=0; i<fish.size(); ++i)
	{
		fish[i]->init();
	}
	for(unsigned int i=0; i<fish.size(); ++i)
	{
		for(unsigned int j=i+1; j<fish.size(); ++j)
		{
			if(fish[i]->getPosition().squaredDistance(fish[j]->getPosition())<
				(_flockVisibility*_flockVisibility)) // two fish cansee each other
			{
				fish[i]->incrementVisibleFriends();
				fish[j]->incrementVisibleFriends();
				fish[i]->updateFriendsPosition(fish[j]->getPosition());
				fish[j]->updateFriendsPosition(fish[i]->getPosition());
			}
		}
		fish[i]->updateMyPosition(_centerFactor,_frictionFactor,deltaT);
	}
}