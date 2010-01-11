/*
-----------------------------------------------------------------------------
Filename:    artSea.h
-----------------------------------------------------------------------------

This source file is generated by the Ogre AppWizard.

Check out: http://conglomerate.berlios.de/wiki/doku.php?id=ogrewizards

Based on the Example Framework for OGRE
(Object-oriented Graphics Rendering Engine)

Copyright (c) 2000-2007 The OGRE Team
For the latest info, see http://www.ogre3d.org/

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the OGRE engine.
-----------------------------------------------------------------------------
*/
#ifndef __artSea_h_
#define __artSea_h_


#include "BaseApplication.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif

#include <CEGUI.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <OgreCEGUIRenderer.h>

//#include <Hydrax/Hydrax.h>

#include <TwOgre.h>
#include "ergoTwGui.h"

#include<vector>
#include"simulation.h"

const char* ARTSEA_LOG_FILE_NAME = "artSea.log";

//--------------------------------------------------------
class AnimationEntity
{
public:
	AnimationEntity(Entity *mEntity,SceneNode *mNode, Ogre::Real mSwimSpeed,
		Ogre::Vector3 mDirection): 
		mEntity(mEntity),
		mNode(mNode),
		mSwimSpeed(mSwimSpeed),
		mDistance(mDistance),
		mDirection(mDirection),
		mDestination(mDestination){}

	void setInitialValues(Entity* mEntity, SceneNode *mNode, Ogre::Real mSwimSpeed,
			Ogre::Vector3 mDirection)
	{
		this->mEntity=mEntity;
		this->mNode=mNode;
		this->mSwimSpeed=mSwimSpeed;
		this->mDirection=mDirection;
	}
	void addMovement(Ogre::Vector3 position)
	{
		mSwimList.push_back(position);
	}

	Entity *mEntity;
	SceneNode *mNode;
	std::deque<Ogre::Vector3>mSwimList;
	Ogre::Real mSwimSpeed;
	Ogre::Real mDistance;
	Ogre::Vector3 mDirection;
	Ogre::Vector3 mDestination;

};

class artSeaApp : public BaseApplication
{
public:
	artSeaApp(void);
	virtual ~artSeaApp(void);

protected:
	//overloads from BaseApplication
	virtual void createScene(void);
	virtual bool frameStarted(const FrameEvent& evt);
	virtual bool frameRenderingQueued(const FrameEvent& evt);
	virtual bool setup();
	virtual void chooseSceneManager(void);

	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	virtual void updateStats();



protected:

	void requestSimulationStateUpdate(Real deltaT);

	void updateWorld(Real deltaT);


protected:
	//Member variables
	Real fixedStepTimeAccumulator;
	Real fixedStepRate;	//an update will be scheduled every fixedStepRate seconds

	Real fixedStepDTMax;	//maximum deltaTime between frames - deltaT received from timer
							//will be clamped to [0, fixedStepDTMax] -  if the game
							//starts to lag real badly, then the simulation will also slow down
	Real fixedStepMaxUpdatesPerFrame;	//maximum updates per single frame - accumulator will be
							//clamped to [0, fixedStepRate*fixedStepMaxUpdatesPerFrame] - without
							//this if a single simulation update always took more than fixedStepRate seconds,
							//then we would have a massive and fatal buildup of simulation updates, leaving no
							//time for rendering

	Real timescale;			//value that the deltaTime will be multiplied by - use this for "bullet time" effects
							//or temporal acceleration ;) It doesn't affect simulation rate.
	Real fixedStepsPerSecond;	//value for interactive tweaking - essencially inverse of fixedStepRate

//	Hydrax::Hydrax* hydraxModule;	//module for realistic rendering of underwater environment

	//Tweaking
	ergoTw::TweakBarSupervisor* tweakBarSupervisor;
	ergoTw::TweakBar* simulationTweakWindow;
	ergoTw::TweakBar* flocksTweakWindow;
	ergoTw::TweakBar* statsTweakWindow;

	//Display
	Real nearClippingDistance;
	Real farClippingDistance;

	//Statistics
	struct
	{
		Ogre::Real lastFPS;
		Ogre::Real avgFPS;
		Ogre::Real bestFPS;
		Ogre::Real worstFPS;
		int bestFrameTime;
		int worstFrameTime;
		int triangleCount;
		int batchCount;
	} statistics;

	//Flock parameters
	std::vector<float> resolutionFactors;
	std::vector<float> flockCenterFactors;
	std::vector<float> flockDirectionFactors;
	std::vector<float> frictions;

	static const int getMaxNumberOfFish()
	{
		return MAX_NUMBER_OF_FISH;
	}
	//animation
	bool nextLocation(AnimationEntity * ae);

private:
	std::vector<Entity*>fishEntities;
	std::vector<SceneNode*> fishNodes;
	int howManyFish;
	static const int MAX_NUMBER_OF_FISH=2000;
	std::vector<AnimationEntity *> animationEntities;
	SimulationWorld * ourWorld;

};

//-------------------------------------------------
class FlockDescription
{
public:
	FlockDescription(int flockSize, String modelFileName, int visibility):
	  flockSize(flockSize),modelFileName(modelFileName),visibility(visibility) {}
	int getFlockSize()
	{
		return flockSize;
	}
	int getFlockVisibility()
	{
		return visibility;
	}
	String getFlockModelFileName()
	{
		return modelFileName;
	}
private:
	int flockSize;
	String modelFileName;
	int visibility;
};


#endif // #ifndef __artSea_h_