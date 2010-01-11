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
#include "lexical_cast.h"
#include "simulation.h"

static const Real DEFAULT_FIXED_STEP_SIMULATION_RATE = 0.030;	//30 msec pause
static const Real DEFAULT_FIXED_STEP_SIMULATION_DT_MAX = 0.25;	//default max for deltaT
static const Real DEFAULT_FIXED_STEP_SIMULATION_MAX_UPDATES_PER_FRAME = 30.0;	//default max for simulations per frame
static const Real MINIMUM_SIMULATION_STEPS_PER_SECOND = 0.00001;
static const int FIRST_FLOCK_SIZE=6;
static const int SECOND_FLOCK_SIZE=12;

//-------------------------------------------------------------------------------------
artSeaApp::artSeaApp(void)
{
	fixedStepTimeAccumulator = 0.0f;
	fixedStepRate = DEFAULT_FIXED_STEP_SIMULATION_RATE;
	fixedStepDTMax = DEFAULT_FIXED_STEP_SIMULATION_DT_MAX;
	fixedStepMaxUpdatesPerFrame = DEFAULT_FIXED_STEP_SIMULATION_MAX_UPDATES_PER_FRAME;
	timescale = 1.0;
	fixedStepsPerSecond = 30.0f;

	tweakBarSupervisor = NULL;
	simulationTweakWindow = NULL;
	flocksTweakWindow = NULL;
}

//-------------------------------------------------------------------------------------
artSeaApp::~artSeaApp(void)
{
//	if(hydraxModule)
//	{
//		delete hydraxModule;
//	}

	if(tweakBarSupervisor)
	{
		delete tweakBarSupervisor;
	}
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
	
	ourWorld->updateAllFish(deltaT,flockDirectionFactors,resolutionFactors,flockCenterFactors,frictions); 
	std::vector<Ogre::Vector3> & newPositions = ourWorld->getAllFishPositions(); // next position
	for(unsigned int i=0; i<fishEntities.size(); ++i)
	{
		Ogre::Vector3 orientation=fishNodes[i]->getOrientation()*Ogre::Vector3::UNIT_X; // why unit_x?
		Ogre::Vector3 direction=fishNodes[i]->getPosition()- newPositions[i];
		if ((1.0f + orientation.dotProduct(direction)) < 0.0001f) 
		{
           fishNodes[i]->yaw(Degree(180));
		}
		else
		{
			Ogre::Quaternion quat=orientation.getRotationTo(direction);
			fishNodes[i]->rotate(quat);
		}
		fishNodes[i]->setPosition(newPositions[i]);		
		
	}

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
	//simulation test
	//setting simulations parameters: howManyFlocks, flockSizes, model files
	srand(time(NULL));
	int howManyFlocks=2; // howManyFlocks setting
	std::vector<int>flockSizes;
	flockSizes.push_back(FIRST_FLOCK_SIZE); //flockSizes setting ; 100
	flockSizes.push_back(SECOND_FLOCK_SIZE);
	std::vector<int> & sizes= flockSizes;
	//creates howManyFlocks with given sizes; calls createFlocks() and setAllFishPositionsAndFlocks
	
	//standard flocking parameters
	for(int i=0; i<howManyFlocks; ++i)
	{
		resolutionFactors.push_back(0); //0.2
		flockDirectionFactors.push_back(1);
		flockCenterFactors.push_back(0); //1.3
		frictions.push_back(2000);
	}
	ourWorld=SimulationWorld::getSimulationWorld(howManyFlocks,sizes,
		flockDirectionFactors,resolutionFactors,flockCenterFactors,frictions); 
	std::vector<FlockDescription*> flockDesc;
	flockDesc.push_back(new FlockDescription(flockSizes.at(0),"fish.mesh",5));//model files settings
	flockDesc.push_back(new FlockDescription(flockSizes.at(1),"rybka.mesh",5));
	std::vector<Vector3> & positions=ourWorld->getAllFishPositions();
	std::vector<int> & flocks = ourWorld->getAllFishFlocks();
	int prev=0;
	ARTSEA_DEBUG_LOG<<positions.size();

	//creating entities and settingother ogre module stuff (entities,nodes,positions...) 
	//based on simulation module
	for(unsigned int i=0; i<positions.size(); ++i)
	{

		ARTSEA_DEBUG_LOG<<positions[i].x<<positions[i].y<<positions[i].z;
		String name="fish";
		String postfix = lexical_cast<String>(i);
		name+=postfix;
		String modelName;
		prev=0;
		modelName=flockDesc[flocks[i]]->getFlockModelFileName();
		ARTSEA_DEBUG_LOG<<modelName;

		fishEntities.push_back(mSceneMgr->createEntity(name,modelName));
		fishNodes.push_back(mSceneMgr->getRootSceneNode()->createChildSceneNode());
		fishNodes[i]->setPosition(positions[i]);
		fishNodes[i]->attachObject(fishEntities[i]);
		//fishNodes[i]->scale(2,2,2);
		if(modelName=="rybka.mesh")
		{
			fishNodes[i]->scale(15,15,15);
		}
	}
	/**for(unsigned int i=0; i<positions.size(); ++i)
	{

	}*/
	//the end of simulation test
	////////////////////////////////////////////////////////////////////////////

	// setup GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow,
	 Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);

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


	//==== FLOCKS TWEAKERS
	flocksTweakWindow=tweakBarSupervisor->createTweakBar("Flocks","Flocks",Ogre::ColourValue::Green, "Tweak window for flocks parameters");
	flocksTweakWindow->getTwOgreWindow()->setPosition(300,300);
	for(int i=0; i<howManyFlocks; ++i)
	{
		String groupName="Flock "+lexical_cast<String>(i);
		flocksTweakWindow->addRealVariable("resolution factor"+groupName,resolutionFactors[i])
			->precision(2)
			->group(groupName)
			->helpString("Resolution factor - how strong resolution effects flock's movement");
		flocksTweakWindow->addRealVariable("flock's direction factor"+groupName, 
			flockDirectionFactors[i])
			->precision(2)
			->group(groupName)
			->helpString("Flock's direction factor - how strong direction effects flock's movement");
		flocksTweakWindow->addRealVariable("flock's center factor"+groupName, 
			flockCenterFactors[i])
			->precision(2)
			->group(groupName)
			->helpString("Flock's center factor - how strong flock's center effects flock's movement");
		flocksTweakWindow->addRealVariable("friction"+groupName, frictions[i])
			->precision(2)
			->group(groupName)
			->helpString("Friction factor");
	}
	//mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox"); 
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
