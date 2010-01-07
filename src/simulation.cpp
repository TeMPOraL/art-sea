#include"simulation.h"
#include"Debug.h"
#include<time.h>
static const int SCREEN_X=300;
static const int SCREEN_Y=200;
static const int SCREEN_Z=30;

//====================================================
// Flock
//====================================================

void Flock::createFish(Ogre::Vector3 position)
{
	Fish * const fish= new Fish(position);
	fishInTheFlock.push_back(fish);
}
void Flock::createAllFish()
{
	for(int i=0; i<flockSize; ++i)
	{
		int x,y,z;
		x=rand()%SCREEN_X-SCREEN_X/2;
		y=rand()%SCREEN_Y-SCREEN_Y/2;
		z=rand()%SCREEN_Z;
		createFish(Ogre::Vector3(x,y,z));
	}
}



//calculate the vector where the flock is going from each fish point of view
//and calculate a vector between the fish and visible friends (distance)
//based on these vectors calculates force where each fish should go and the next position
void Flock::updateAllFish(Ogre::Real deltaT)
{
	//ARTSEA_LOG<<"flock size"<<getFlockSize();
	//flock sizes ok
	for(int i=0; i<getFlockSize(); ++i)
	{
		for(int j=i+1; j<getFlockSize(); ++j)
		{
			//add to each fish friend direction vector, and vector between the fish and it's friend
			if(canSeeEachOther(fishInTheFlock[i], fishInTheFlock[j]))
			{
				
				fishInTheFlock[i]->updateFlockDirection(fishInTheFlock[j]->getForce());
				fishInTheFlock[j]->updateFlockDirection(fishInTheFlock[i]->getForce());
				fishInTheFlock[i]->updateVisibleFlockCenter(fishInTheFlock[j]->getPosition()-fishInTheFlock[i]->getPosition());
				fishInTheFlock[j]->updateVisibleFlockCenter(fishInTheFlock[i]->getPosition()-fishInTheFlock[j]->getPosition());

				if(getSquaredDistance(fishInTheFlock[i],fishInTheFlock[j])<=CLOSE_FRIENDS_DISTANCE) //a fish tries to be away not from all other fish he can see, but only from close friends
				{
					fishInTheFlock[i]->updateMyNearestFriendsDirection(fishInTheFlock[j]->getPosition()-fishInTheFlock[i]->getPosition());	//update...( vector between fish i and fish j)
					fishInTheFlock[j]->updateMyNearestFriendsDirection(fishInTheFlock[i]->getPosition()-fishInTheFlock[j]->getPosition());	
				}
			}
		}
		fishInTheFlock[i]->calculateForce();
		fishInTheFlock[i]->updatePosition(deltaT);
	}
}


//====================================================
// SimulationWorld (singleton)
//====================================================

bool SimulationWorld::singletonFlag=false;
SimulationWorld *SimulationWorld::singleton=0;

void SimulationWorld::createFlocks(int howMany, std::vector<int> & sizes)
{
	ARTSEA_DEBUG_LOG<<howMany;
	for(int i=0; i<howMany; ++i)
	{
		Flock * newFlock = new Flock(sizes[i]); //default visibiity - 7; fix it

		flocks.push_back(newFlock);
		newFlock->createAllFish();
	}
}

void SimulationWorld::setAllFishPositionsAndFlocks()
{
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		for(int j=0; j<flocks[i]->getFlockSize(); ++j)
		{
			allFishPositions.push_back(flocks[i]->getAllFish()[j]->getPosition());
			allFishFlocks.push_back(i);
		}
	}
}

void SimulationWorld::updateAllFish(Ogre::Real deltaT)
{
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		flocks[i]->updateAllFish(deltaT);
	}
	allFishPositions.clear();
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		for(int j=0; j<flocks[i]->getFlockSize(); ++j)
		{
			allFishPositions.push_back(flocks[i]->getAllFish()[j]->getPosition());
		}
	}
}


//debug log's positions & flocks of all fish
void SimulationWorld::showWorld()
{
	ARTSEA_DEBUG_LOG<<"I'm gonna show you my world";
	for(unsigned int i=0; i<allFishPositions.size(); ++i)
	{
		ARTSEA_DEBUG_LOG<<"positions:"<<allFishPositions[i].x<<allFishPositions[i].y<<
			allFishPositions[i].z<<" "<<allFishFlocks[i];
}
}