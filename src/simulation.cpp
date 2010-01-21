#include"simulation.h"
#include"Debug.h"
#include<time.h>
static const int SCREEN_X=500;
static const int SCREEN_Y=450;
static const int SCREEN_Z=5;

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
void Flock::updateAllFish(Ogre::Real deltaT, float direction,float resolution, float center,float friction,float cameraFactor,Ogre::Camera * camera)
{
	this->flockDirectionFactor=direction;
	this->resolutionFactor=resolution;
	this->flockCenterFactor=center;
	this->friction=friction;

	for(int i=0; i<getFlockSize(); ++i)
	{
		fishInTheFlock[i]->initFishDataBeforeUpdating();
	}
	for(int i=0; i<getFlockSize(); ++i)
	{
		int seenNumber=0;
		int seenPredators=0;
		for(int j=i+1; j<getFlockSize(); ++j)
		{
			//escaping from camera is a priority
			if((camera->getPosition()-fishInTheFlock[i]->getPosition()).length()<(getVisiblity()*cameraFactor)) // how far from camera the fish escapes = 2*visibility
			{
				fishInTheFlock[i]->updateEscapeFromCamera(camera->getPosition()-fishInTheFlock[i]->getPosition());
			}
			else
			{
				//looking for enemies:
				//predators
				for(int unsigned k=0; k<myPredators.size(); ++k)
				{
					const std::vector<Fish*>&enemies=myPredators[k]->getAllFish();
					for(int unsigned j=0; j<enemies.size(); ++j)
					{
						if(canSeeEachOther(fishInTheFlock[i],enemies[j]))
						{
							fishInTheFlock[i]->updateVisiblePredators(enemies[j]->getPosition()-fishInTheFlock[i]->getPosition());
							++seenPredators;
						}
					}
				}
				if(seenPredators==0)
				{
					//add to each fish friend direction vector, and vector between the fish and it's friend
					//ARTSEA_LOG<<"visibility"<<getFlockVisibility();
					if(canSeeEachOther(fishInTheFlock[i], fishInTheFlock[j]))
					{
						++seenNumber;
						fishInTheFlock[i]->incrementVisibleFish();
						fishInTheFlock[j]->incrementVisibleFish();
						fishInTheFlock[i]->updateFlockDirection(fishInTheFlock[j]->getForce());
						fishInTheFlock[j]->updateFlockDirection(fishInTheFlock[i]->getForce());
						fishInTheFlock[i]->updateVisibleFlockCenter(fishInTheFlock[j]->getPosition()-fishInTheFlock[i]->getPosition());
						fishInTheFlock[j]->updateVisibleFlockCenter(fishInTheFlock[i]->getPosition()-fishInTheFlock[j]->getPosition());

						if(getSquaredDistance(fishInTheFlock[i],fishInTheFlock[j])<=CLOSE_FRIENDS_DISTANCE) //a fish tries to be away not from all other fish he can see, but only from close friends
						{
							fishInTheFlock[i]->incrementCloseFriends();
							fishInTheFlock[j]->incrementCloseFriends();
							fishInTheFlock[i]->updatemyNearestFriendsCenter(fishInTheFlock[j]->getPosition()-fishInTheFlock[i]->getPosition());	//update...( vector between fish i and fish j)
							fishInTheFlock[j]->updatemyNearestFriendsCenter(fishInTheFlock[i]->getPosition()-fishInTheFlock[j]->getPosition());	
						}
					}
				
					//preys; predator can see preys at least at the beginning
					for(unsigned int k=0; k<myPreys.size(); ++k)
					{
						const std::vector<Fish*>&enemies=myPreys[k]->getAllFish();
						for(unsigned int j=0; j<enemies.size(); ++j)
						{
							if(canSeeEachOther(fishInTheFlock[i],enemies[j]))
							{
								fishInTheFlock[i]->updateVisiblePreys(enemies[j]->getPosition()-fishInTheFlock[i]->getPosition());
							}
						}
					}
				}
			}
		}
		//fishInTheFlock[i]->calculateForce(1.5,0.2,1.8);
		fishInTheFlock[i]->calculateForce(flockDirectionFactor,resolutionFactor,flockCenterFactor,friction,cameraFactor,deltaT);
		fishInTheFlock[i]->updatePosition(deltaT);
	}
}


//====================================================
// SimulationWorld (singleton)
//====================================================

bool SimulationWorld::singletonFlag=false;
SimulationWorld *SimulationWorld::singleton=0;

void SimulationWorld::createFlocks(int howMany, std::vector<int> & sizes,
								   std::vector<float>&directions,std::vector<float>&resolutions,
								   std::vector<float>&centers,std::vector<float>&frictions,std::vector<float>&cameraFactors)
{
	for(int i=0; i<howMany; ++i)
	{
		Flock * newFlock = new Flock(sizes[i],directions[i],resolutions[i],centers[i],frictions[i],cameraFactors[i]); 

		flocks.push_back(newFlock);
		newFlock->createAllFish();
	}
	flocks[1]->setVisibility(1000); // set predators visibility individually
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

void SimulationWorld::updateAllFish(Ogre::Real deltaT,std::vector<float>&directions,
									std::vector<float>&resolutions,std::vector<float>&centers,std::vector<float>&frictions,std::vector<float>&cameraFactors,Ogre::Camera*camera)
{
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		flocks[i]->updateAllFish(deltaT,directions[i],resolutions[i],centers[i],frictions[i],cameraFactors[i],camera);
	}
	allFishPositions.clear();
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		std::vector<Fish *> fishTmpVector=flocks[i]->getAllFish();
		for(int j=0; j<flocks[i]->getFlockSize(); ++j)
		{
			allFishPositions.push_back(fishTmpVector[j]->getPosition());
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