/*
-----------------------------------------------------------------------------
Filename:    artSea.cpp
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

//FRAMEWORK / PROJECT TODOs:
//* TwErgoGui - TwOgreGui modification //do we really need that?
//[v] FMOD for sound
//[v] Configurable & fixed update rate
//[v] Call stack (debugging)
//[v] Custom logging target
//[v] Own assertions?
//* More comments 
//* Even more comments
//* Hydrax

////////////////////////////////////////////////////
// Dorota's TODOs:
//* change all copied stuff to const references
//* think of rybka.position = new Position(...);  
//* check ALL 'passing arguments' copy vs. pointers, fps at the beginning of simulation (init)

//#include <Hydrax/Hydrax.h>
//#include <Hydrax/Noise/Perlin/Perlin.h>
//#include <Hydrax/Modules/ProjectedGrid/ProjectedGrid.h>

#include "artSea.h"

#include "Debug.h"

#include<stdio.h>
//[snd] #include <FMOD.hpp>
#include "lexical_cast.h"
#include "simulation.h"

static const Real DEFAULT_FIXED_STEP_SIMULATION_RATE = 0.030;	//30 msec pause
static const Real DEFAULT_FIXED_STEP_SIMULATION_DT_MAX = 0.25;	//default max for deltaT
static const Real DEFAULT_FIXED_STEP_SIMULATION_MAX_UPDATES_PER_FRAME = 30.0;	//default max for simulations per frame
static const Real MINIMUM_SIMULATION_STEPS_PER_SECOND = 0.00001;
static const Real DEFAULT_TIMESCALE = 1.0;
static const Real DEFAULT_FIXED_STEPS_PER_SECOND = 30.0;
static const Real DEFAULT_NEAR_CLIPPING_DISTANCE = 0.1;
static const Real DEFAULT_FAR_CLIPPING_DISTANCE = 10000.0;
static const int FIRST_FLOCK_SIZE=400;
static const int SECOND_FLOCK_SIZE=30;
static const int THIRD_FLOCK_SIZE=5;


//-------------------------------------------------------------------------------------
artSeaApp::artSeaApp(void)
{
	fixedStepTimeAccumulator = 0.0f;
	fixedStepRate = DEFAULT_FIXED_STEP_SIMULATION_RATE;
	fixedStepDTMax = DEFAULT_FIXED_STEP_SIMULATION_DT_MAX;
	fixedStepMaxUpdatesPerFrame = DEFAULT_FIXED_STEP_SIMULATION_MAX_UPDATES_PER_FRAME;
	timescale = DEFAULT_TIMESCALE;
	fixedStepsPerSecond = DEFAULT_FIXED_STEPS_PER_SECOND;

	tweakBarSupervisor = NULL;
	simulationTweakWindow = NULL;
	flocksTweakWindow = NULL;
	statsTweakWindow = NULL;
	nearClippingDistance = DEFAULT_NEAR_CLIPPING_DISTANCE;
	farClippingDistance = DEFAULT_FAR_CLIPPING_DISTANCE;

	//[snd] soundSystem = NULL;
}

//-------------------------------------------------------------------------------------
artSeaApp::~artSeaApp(void)
{
//	if(hydraxModule)
//	{
//		delete hydraxModule;
//	}

	delete tweakBarSupervisor;

	//[snd] soundSystem->release();
}

//================================================================
// Simulation update stuff
//================================================================
bool artSeaApp::frameStarted(const FrameEvent& evt)
{
	//call superclass function
	bool result = BaseApplication::frameStarted(evt);

//	hydraxModule->update(evt.timeSinceLastFrame);


	tweakBarSupervisor->update();

	//update variables
	mCamera->setNearClipDistance(nearClippingDistance);
	mCamera->setFarClipDistance(farClippingDistance);

	//[snd] soundSystem->update();

	return (result && true);
}

bool artSeaApp::frameRenderingQueued(const FrameEvent& evt)
{
	requestSimulationStateUpdate(evt.timeSinceLastFrame);
	return true;
}

void artSeaApp::requestSimulationStateUpdate(Real deltaT)
{
	ARTSEA_GUARD(artSeaApp::requestSimulationStateUpdate);

	//clamp dT to ie. 0 - 0.25s
	//clamp accumulator to ie. 0..30xfixedStepRate
	Math::Clamp(deltaT, Real(0.0), fixedStepDTMax);

	if(fixedStepTimeAccumulator > fixedStepMaxUpdatesPerFrame*fixedStepRate)
	{
		ARTSEA_LOG << "WARNING - exceeded max updates per frame (accumulator: " << fixedStepTimeAccumulator << ", max updates per frame: " << fixedStepMaxUpdatesPerFrame*fixedStepRate << ")";
	}
	Math::Clamp(fixedStepTimeAccumulator, Real(0.0), fixedStepMaxUpdatesPerFrame*fixedStepRate);

	//fixed time step simulation
	fixedStepTimeAccumulator += deltaT;

	ARTSEA_LOG << "World update. " << deltaT << " " << fixedStepTimeAccumulator;

	while(fixedStepTimeAccumulator > fixedStepRate)
	{
		fixedStepTimeAccumulator -= fixedStepRate;
		updateWorld(fixedStepRate*timescale);
	}

	//update rate
	ARTSEA_ASSERT(fixedStepRate >= MINIMUM_SIMULATION_STEPS_PER_SECOND, "Not enough simulations per second.");
	fixedStepRate = 1.0/fixedStepsPerSecond;

	ARTSEA_ASSERT(fixedStepTimeAccumulator >= 0, "Accumulator gone negative.");

	ARTSEA_UNGUARD;
}

void artSeaApp::updateWorld(Real deltaT)
{
	ARTSEA_GUARD(artSeaApp::updateWorld);

	ARTSEA_ASSERT(deltaT >= 0, "Negative deltaT."); 
	
	unsigned int counter=0;
	for(int i=0; i<howManyFlocks; ++i)
	{
		flocks[i]->updateAllFish(deltaT,flockCenterFactors[i],flockDirectionFactors[i],frictions[i],minDistances[i],visibilities[i]);
		std::vector<Fish*> & fish=flocks[i]->getAllFish();
		for(unsigned int j=0; j<fish.size(); ++j)
		{
			fishNodes[counter]->setPosition(fish[j]->getPosition());
			++counter;
		}
	}
	

	/**ourWorld->updateAllFish(deltaT,flockDirectionFactors,resolutionFactors,flockCenterFactors,frictions,cameraFactors, mCamera); 
	std::vector<Ogre::Vector3> & newPositions = ourWorld->getAllFishPositions(); // next position
	//set new position and orientation after updates
	for(unsigned int i=0; i<fishEntities.size(); ++i)
	{
		//set new orientation
		Ogre::Vector3 orientation=fishNodes[i]->getOrientation()*Ogre::Vector3(-1,0,0);//*(-1*Ogre::Vector3::UNIT_SCALE); // unit_x the direction of the fish naturally faces
		Ogre::Vector3 direction=newPositions[i]-fishNodes[i]->getPosition();
		if ((1.0f + orientation.dotProduct(direction)) < 0.0001f) 
		{
           fishNodes[i]->yaw(Degree(180));
		}
		else
		{
			Ogre::Quaternion quat=orientation.getRotationTo(direction);
			fishNodes[i]->rotate(quat);
		}
		//set new position
		//fishNodes[i]->setPosition(newPositions[i]);

		fishNodes[i]->translate(direction);

	}*/

	//NOTE Uncomment this to see an example of how artSea's debugging framework works.
	//ARTSEA_ASSERT(0, "Should never get here.");

	ARTSEA_UNGUARD;
}

//================================================================
// Application setup section
//================================================================
void artSeaApp::createScene(void)
{
	//////////////////////////////////////////////////////////////
	//simulation 
	srand(time(NULL));
	howManyFlocks=3;
	flocks.push_back(new Flock(100,50,1,1,0.1,50,0,50,5)); //Flock(size,centerFactor,resFactor,dirFactor,friction,visibility,terytory ceer,teritory size,distance
	flocks.push_back(new Flock(10,50,1,100,0.1,100,30,50,5));
	flocks.push_back(new Flock(100,50,1,1,0.1,50,-50,50,5));
	modelNames.push_back("fish.mesh");
	modelNames.push_back("rybka.mesh");
	modelNames.push_back("fish.mesh");
	


	for(int i=0; i<howManyFlocks; ++i)
	{
		flockCenterFactors.push_back(50);
		flockDirectionFactors.push_back(0);
		minDistances.push_back(5);
		frictions.push_back(0.1);
		visibilities.push_back(50);		
	}
	std::vector<Fish*>& predators=flocks[1]->getAllFish();
	for(unsigned int  i=0; i<predators.size(); ++i)
	{
		flocks[0]->addPreadator(predators[i]);
		flocks[2]->addPreadator(predators[i]);
	}
	std::vector<Fish*>&victims=flocks[0]->getAllFish();
	for(unsigned int  i=0; i<30; ++i) 
	{
		flocks[1]->addVictim(victims[i]);
	}
	/**victims=flocks[2]->getAllFish();
	for(unsigned int  i=0; i<5; ++i) 
	{
		flocks[1]->addVictim(victims[i]);
	}*/

	int counter=0;

	for(int i=0; i<howManyFlocks; ++i)
	{
		unsigned int thisFlockSize=flocks[i]->getSize();
		String modelName=modelNames[i];
		for(unsigned int j=0; j<thisFlockSize; ++j)
		{
			String fishName="fish"+lexical_cast<String>(counter);
			fishEntities.push_back(mSceneMgr->createEntity(fishName,modelName));
			fishNodes.push_back(mSceneMgr->getRootSceneNode()->createChildSceneNode());
			fishNodes[counter]->setPosition(flocks[i]->getAllFish()[j]->getPosition());
			fishNodes[counter]->attachObject(fishEntities[counter]);
			if(modelName=="rybka.mesh")
			{
				fishNodes[counter]->scale(10,10,10);
			}
			++counter;
		}
	}
	


	//end of simulation part
	////////////////////////////////////////////////////////////////////////////

	// setup GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow,
	 Ogre::RENDER_QUEUE_OVERLAY, true, 3000, mSceneMgr);

	mGUISystem = new CEGUI::System(mGUIRenderer);

	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);


	// load scheme and set up defaults
	CEGUI::SchemeManager::getSingleton().loadScheme(
	 (CEGUI::utf8*)"TaharezLookSkin.scheme");
	mGUISystem->setDefaultMouseCursor(
	 (CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	mGUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
	CEGUI::MouseCursor::getSingleton().show( );
	setupEventHandlers();

	// Set ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// Create a light
	Light* l = mSceneMgr->createLight("MainLight");
	l->setPosition(20,80,50);

//	hydraxModule = new Hydrax::Hydrax(mSceneMgr, mCamera, mWindow->getViewport(0));

//	Hydrax::Module::ProjectedGrid* mModule =
//						new Hydrax::Module::ProjectedGrid(	hydraxModule,
//															new Hydrax::Noise::Perlin(),
//															Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),
//															Hydrax::MaterialManager::NM_VERTEX,
//															Hydrax::Module::ProjectedGrid::Options() );
//
//	hydraxModule->setModule(static_cast<Hydrax::Module::Module*>(mModule));

//	hydraxModule->loadCfg("HydraxDemo.hdx");

	// Create water
	//hydraxModule->create();

//	hydraxModule->setSunPosition(Ogre::Vector3(0,10000,90000));
//	hydraxModule->setSunColor(Ogre::Vector3(1,0.6,0.4));

	tweakBarSupervisor = new ergoTw::TweakBarSupervisor(mWindow, mSceneMgr);



	//==== REAL TWEAKERS
	simulationTweakWindow = tweakBarSupervisor->createTweakBar("Simulation", "Simulation", Ogre::ColourValue::Blue, "Tweak window for core simulation parameters.");
	simulationTweakWindow->getTwOgreWindow()->iconify();
	simulationTweakWindow->addRealVariable("step duration", fixedStepRate, true)
		->precision(4)
		->group("Fixed step simulation")
		->helpString("Single step duration - how long does it take in seconds.");
	simulationTweakWindow->addRealVariable("steps per second", fixedStepsPerSecond)
		->precision(4)
		->range(MINIMUM_SIMULATION_STEPS_PER_SECOND, std::numeric_limits<Ogre::Real>::infinity(), 0.1)
		->group("Fixed step simulation")
		->helpString("Simulation steps per real-time second.");
	simulationTweakWindow->addRealVariable("timescale", timescale)
		->range(0.0, std::numeric_limits<Ogre::Real>::infinity(), 0.01)
		->precision(2)
		->group("Fixed step simulation")
		->helpString("Each simulation frame has its deltaTime multiplied by this value. It does not, however, change the simulation rate.");
	simulationTweakWindow->addRealVariable("near clipping distance", nearClippingDistance)
		->range(0.01, std::numeric_limits<Ogre::Real>::infinity(), 0.01)
		->precision(2)
		->group("Rendering")
		->helpString("Controls the distance of near clipping plane from camera. Too high values cause object close to camera to disappear. \
Too small values could mess rendering up completely.");
	simulationTweakWindow->addRealVariable("far clipping distance", farClippingDistance)
		->range(0.01, std::numeric_limits<Ogre::Real>::infinity(), 0.1)
		->precision(2)
		->group("Rendering")
		->helpString("Controls the distance of far clipping plane from camera. Too high values could mess rendering up completely. \
Avoid getting far distance < near distance - may cause III World War, or worse.");


	//==== FLOCKS TWEAKERS
	flocksTweakWindow=tweakBarSupervisor->createTweakBar("Flocks","Flocks",Ogre::ColourValue::Green, "Tweak window for flocks parameters");
	flocksTweakWindow->getTwOgreWindow()->setPosition(300,300);
	flocksTweakWindow->getTwOgreWindow()->iconify();
	for(int i=0; i<howManyFlocks; ++i)
	{
		String groupName="Flock "+lexical_cast<String>(i);
		flocksTweakWindow->addRealVariable("Minimum distance" + groupName,minDistances[i])
			->precision(2)
			->label("Minimum distance")
			->group(groupName)
			->helpString("Minimum distance between fish");
		flocksTweakWindow->addRealVariable("Direction factor" + groupName, flockDirectionFactors[i])
			->precision(2)
			->label("Direction factor")
			->group(groupName)
			->helpString("Flock's direction factor - how strong direction effects flock's movement");
		flocksTweakWindow->addRealVariable("Center factor" + groupName, flockCenterFactors[i])
			->precision(2)
			->label("Center factor")
			->group(groupName)
			->helpString("Flock's center factor - how strong flock's center effects flock's movement");
		flocksTweakWindow->addRealVariable("friction" + groupName, frictions[i])
			->precision(2)
			->label("Friction factor")
			->group(groupName)
			->helpString("Friction factor for flock members.");
		flocksTweakWindow->addIntegerVariable("Flock visibility" + groupName, visibilities[i])
			->label("Flock visibility")
			->group(groupName)
			->helpString("Set how far fish wants to stay from the camera. The bigger, the further");

	} 

	//==== PURE STATISTICS :)
	statsTweakWindow = tweakBarSupervisor->createTweakBar("Stats", "Statistics", Ogre::ColourValue::Black, "Tweak window with read-only realtime statistics.");
	statsTweakWindow->getTwOgreWindow()->setPosition(500, 300);
	statsTweakWindow->getTwOgreWindow()->iconify();
	statsTweakWindow->addRealVariable("Current FPS", statistics.lastFPS, true)
		->precision(2)
		->group("Rendering");
	statsTweakWindow->addRealVariable("Average FPS", statistics.avgFPS, true)
		->precision(2)
		->group("Rendering");
	statsTweakWindow->addRealVariable("Best FPS", statistics.bestFPS, true)
		->precision(2)
		->group("Rendering");
	statsTweakWindow->addRealVariable("Worst FPS", statistics.worstFPS, true)
		->precision(2)
		->group("Rendering");

	statsTweakWindow->addIntegerVariable("Best frame time", statistics.bestFrameTime, true)
		->group("Rendering");
	statsTweakWindow->addIntegerVariable("Worst frame time", statistics.worstFrameTime, true)
		->group("Rendering");
	statsTweakWindow->addIntegerVariable("Triangle count", statistics.triangleCount, true)
		->group("Rendering");
	statsTweakWindow->addIntegerVariable("Batch count", statistics.batchCount, true)
		->group("Rendering");

	mSceneMgr->setSkyBox(true, "artSea/SkyBox");

	//sound init
	//[snd] FMOD_RESULT res = FMOD::System_Create(&soundSystem);
	//[snd] if(res != FMOD_OK)
	//[snd] {
	//[snd] 	throw std::runtime_error( "Failed to create sound system.");
	//[snd] }
	//[snd] res = soundSystem->init(10, FMOD_INIT_NORMAL, NULL);

	//[snd] if(res != FMOD_OK)
	//[snd] {
	//[snd] 	throw std::runtime_error("Failed to initialize sound system.");
	//[snd] }

	//[snd] FMOD::Sound* sound;
	//[snd] res = soundSystem->createStream("../../media/music.mp3", FMOD_DEFAULT | FMOD_LOOP_NORMAL, NULL, &sound);

	//[snd] if(res != FMOD_OK)
	//[snd] {
	//[snd] 	throw std::runtime_error("Failed to load music.");
	//[snd] }

	//[snd] soundSystem->playSound(FMOD_CHANNEL_FREE, sound, false, NULL);

}

void artSeaApp::chooseSceneManager(void)
{
	ARTSEA_GUARD(artSeaApp::chooseSceneManager);

	// Get the SceneManager, in this case an exterior one - we're doing an underwater simulation here.
	mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE);

	ARTSEA_UNGUARD;
}

bool artSeaApp::setup()
{
	ARTSEA_GUARD(artSeaApp::setup);

	//call parent setup
	bool result = BaseApplication::setup();

	//create artSea log
	CDebugTools::createLog(ARTSEA_LOG_FILE_NAME);

	//TODO move createScene here!

	ARTSEA_LOG << "artSeaApp::setup() - setup completed." ;

	return (result && true);	//if base setup() fails, we fail too.

	ARTSEA_UNGUARD;
}

//================================================================
// Input handling
//================================================================

bool artSeaApp::keyPressed( const OIS::KeyEvent &arg )
{
	tweakBarSupervisor->injectKeyPressed(arg);

	return BaseApplication::keyPressed(arg);
}

bool artSeaApp::keyReleased( const OIS::KeyEvent &arg )
{
	tweakBarSupervisor->injectKeyReleased(arg);

	return BaseApplication::keyReleased(arg);
}

bool artSeaApp::mouseMoved( const OIS::MouseEvent &arg )
{
	tweakBarSupervisor->injectMouseMoved(arg);

	return BaseApplication::mouseMoved(arg);
}

bool artSeaApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	tweakBarSupervisor->injectMousePressed(arg, id);

	return BaseApplication::mousePressed(arg, id);
}

bool artSeaApp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	tweakBarSupervisor->injectMouseReleased(arg, id);

	return BaseApplication::mouseReleased(arg, id);
}

void artSeaApp::updateStats()
{
	const RenderTarget::FrameStats& stats = mWindow->getStatistics();

	statistics.lastFPS = stats.lastFPS;
	statistics.avgFPS = stats.avgFPS;
	statistics.bestFPS = stats.bestFPS;
	statistics.worstFPS = stats.worstFPS;
	statistics.batchCount = static_cast<int>(stats.batchCount);
	statistics.bestFrameTime = static_cast<int>(stats.bestFrameTime);
	statistics.worstFrameTime = static_cast<int>(stats.worstFrameTime);
	statistics.triangleCount = static_cast<int>(stats.triangleCount);
}

//================================================================
// Main
//================================================================

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
		int main(int argc, char *argv[])
#endif
		{
			// Create application object
			artSeaApp app;

			try
			{
				ARTSEA_GUARD(Main);

				app.go();

				ARTSEA_UNGUARD;
			}
			catch( Ogre::Exception& e )
			{
				CDebugTools::displayException("Ogre exception", e.getFullDescription().c_str());
			}
			catch (std::exception& except)
			{
				CDebugTools::displayException("STD Exception", except.what());
			}
			catch(std::exception* except)
			{
				CDebugTools::displayException("STD Exception pointer (too much Java or C#?)", except->what());
			}
			catch(...)
			{
				CDebugTools::displayException("Unrecognized exception.", "");
			}

			return 0;
		}

#ifdef __cplusplus
	}
#endif
