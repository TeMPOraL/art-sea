#include"simulation.h"
#include"Debug.h"
//====================================================
// SimulationWorld (singleton)
//====================================================
void Flock::createFish(Ogre::Vector3 position)
{
	Fish * fish= new Fish(position);
	fishInTheFlock.push_back(fish);
}
void Flock::createAllFish()
{
	for(int i=0; i<flockSize; ++i)
	{
		createFish(Ogre::Vector3(i,i,i));
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

void SimulationWorld::createFlocks(int howMany)
{
	for(int i=0; i<howMany; ++i)
	{
		Flock * newFlock = new Flock(6,7);
		flocks.push_back(newFlock);
		newFlock->createAllFish();
	}
}


std::vector<Fish*> SimulationWorld::getAllFish()
{
	//FIXIT !!!!!!!!!!!!!!!!
	std::vector<Fish*>allFish;

	//ATTENTION: iterators into vectors become invalidated whenever the number of
	//elements in the vector changes.
	/**ARTSEA_DEBUG_LOG<<"begin";
	for(int i=0; i<getHowManyFlocks(); ++i)
	{
		std::vector<Fish*>tmp=flocks[i]->getAllFish();
		ARTSEA_DEBUG_LOG<<tmp.size();
		//allFish.reserve(allFish.size()+flocks[i]->getFlockSize());
		//allFish.insert(allFish.begin(),flocks[i]->getAllFish().begin(),
		//	flocks[i]->getAllFish().end());
	}
	ARTSEA_DEBUG_LOG<<"end"; */
	return allFish;
}