#include"simulation.h"
#include"Debug.h"
#include<time.h>
//====================================================
// SimulationWorld (singleton)
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
		x=rand()%15;
		y=rand()%20;
		z=rand()%8;
		createFish(Ogre::Vector3(x,y,z));
	}
}

bool Flock::canSeeEachOther(Fish a, Fish b)
{
	Ogre::Vector3 posA=a.getPosition();
	if(getSquaredDistance(a,b)<getFlockVisibility()*getFlockVisibility())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Flock::updateAllFish()
{
	for(int i=0; i<getFlockSize(); ++i)
	{
		for(int j=i+1; j<getFlockSize(); ++j)
		{
			if(canSeeEachOther(*fishInTheFlock[i], *fishInTheFlock[j]))
			{

			}
		}
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
		Flock * newFlock = new Flock(sizes[i],7);
		flocks.push_back(newFlock);
		newFlock->createAllFish();
	}
}


/**std::vector<Fish*const > SimulationWorld::getAllFish()
{
	//FIXIT !!!!!!!!!!!!!!!!
	std::vector<Fish*const>allFish;

	//ATTENTION: iterators into vectors become invalidated whenever the number of
	//elements in the vector changes.
	ARTSEA_DEBUG_LOG<<"begin";
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		std::vector<Fish*const >tmp=flocks[i]->getAllFish();
		ARTSEA_DEBUG_LOG<<tmp.size();
		//allFish.reserve(allFish.size()+flocks[i]->getFlockSize());
		allFish.insert(allFish.begin(),tmp.begin(),tmp.end());
	}
	ARTSEA_DEBUG_LOG<<"end"; 
	return allFish;
}*/

//returns onluy positions for entities
/**std::vector <Ogre::Vector3> & SimulationWorld::getAllFishPositions()
{
	std::vector<Ogre::Vector3> & temp=allFishPositions;
	/**for(int i=0; i<getHowManyFlocks(); ++i)
	{
		for(int j=0; j<flocks[i]->getFlockSize(); ++j)
		{
			allFishPositions.push_back(flocks[i]->getAllFish()[j]->getPosition());
		}
	}*/
	/**return temp;
}*/

std::vector<Ogre::Vector3>&  SimulationWorld::getAllFishPositions()
{
	std::vector<Ogre::Vector3>& temp=allFishPositions;
	return temp;
}

std::vector<int> & SimulationWorld::getAllFishFlocks()
{
	return allFishFlocks;
}


//creates vector with positions and adequate vector with flocks' ids
//allFishPositions[i] - positions of fish 'number' i
//allFishFlocks[i] - flock id of fish 'number' i
//needed for ogre module
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

//debug log's positions & flocks of all fish
void SimulationWorld::showWorld()
{
	ARTSEA_DEBUG_LOG<<"I'm gonna show you my world";
	for(int i=0; i<allFishPositions.size(); ++i)
	{
		ARTSEA_DEBUG_LOG<<"positions:"<<allFishPositions[i].x<<allFishPositions[i].y<<
			allFishPositions[i].z<<" "<<allFishFlocks[i];
}
}