/**
 ergoTw - a wrapper for TwOgreGui library that fixes some important elements missing from the library.

 FIXME insert some licence strings or whatever here.

*/


#include "ergoTwGui.h"

namespace ergoTw
{

	TweakBar::~TweakBar()
	{
		//delete all registered variables
		for(variableVector_t::iterator itor = variables.begin() ; itor != variables.end() ; ++itor)
		{
			ARTSEA_LOW_LEVEL_ASSERT( (*itor) != NULL, "Found NULL Variable pointer while destroying TweakBar.");

			delete (*itor);
		}
	}

	BoolVariable* TweakBar::addBoolVariable(const Ogre::String& name, bool& variable, bool readOnly)
	{
		ARTSEA_GUARD(TweakBard::addBoolVariable);

		TwOgre::BoolVariable* twVariable = window->addBoolVariable(name, readOnly);
		BoolVariable* var = new BoolVariable(&variable, twVariable);

		variables.push_back(var);

		return var;

		ARTSEA_UNGUARD;
	}

	ColorVariable* TweakBar::addColorVariable(const Ogre::String& name, Ogre::ColourValue& variable, bool readOnly)
	{
		ARTSEA_GUARD(TweakBard::addColorVariable);

		TwOgre::ColorVariable* twVariable = window->addColorVariable(name, readOnly);
		ColorVariable* var = new ColorVariable(&variable, twVariable);

		variables.push_back(var);

		return var;

		ARTSEA_UNGUARD;
	}

	EnumVariable* TweakBar::addEnumVariable(const Ogre::String& name, int& value, const TwOgre::EnumValueList& enumList, bool readOnly)
	{
		ARTSEA_GUARD(TweakBard::addEnumVariable);

		TwOgre::EnumVariable* twVariable = window->addEnumVariable(name, readOnly, "", enumList);
		EnumVariable* var = new EnumVariable(&value, twVariable);

		variables.push_back(var);

		return var;

		ARTSEA_UNGUARD;
	}

	IntegerVariable* TweakBar::addIntegerVariable(const Ogre::String& name, int& value, bool readOnly)
	{
		ARTSEA_GUARD(TweakBard::addIntegerVariable);

		TwOgre::IntegerVariable* twVariable = window->addIntegerVariable(name, readOnly);
		IntegerVariable* var = new IntegerVariable(&value, twVariable);

		variables.push_back(var);

		return var;

		ARTSEA_UNGUARD;
	}

	RealVariable* TweakBar::addRealVariable(const Ogre::String& name, Ogre::Real& value, bool readOnly)
	{
		ARTSEA_GUARD(TweakBard::addRealVariable);

		TwOgre::RealVariable* twVariable = window->addRealVariable(name, readOnly);
		RealVariable* var = new RealVariable(&value, twVariable);

		variables.push_back(var);

		return var;

		ARTSEA_UNGUARD;
	}

	StringVariable* TweakBar::addStringVariable(const Ogre::String& name, Ogre::String& value, bool readOnly)
	{
		ARTSEA_GUARD(TweakBard::addStringVariable);

		TwOgre::StringVariable* twVariable = window->addStringVariable(name, readOnly);
		StringVariable* var = new StringVariable(&value, twVariable);

		variables.push_back(var);

		return var;

		ARTSEA_UNGUARD;
	}

	static void updateSingleVar(Variable* var)
	{
		ARTSEA_GUARD(updateSingleVar);
		ARTSEA_ASSERT(var != NULL, "Received NULL ergoTw variable pointer!");

		var->update();

		ARTSEA_UNGUARD;
	}

	void TweakBar::updateVariables()
	{
		ARTSEA_GUARD(TweakBar::updateVariables);

		std::for_each(variables.begin(), variables.end(), updateSingleVar);

		ARTSEA_UNGUARD;
	}



	TweakBarSupervisor::TweakBarSupervisor(Ogre::RenderWindow *renderWindow, Ogre::SceneManager *sceneManager)
	{
		windowManager = new TwOgre::WindowManager(renderWindow, sceneManager);
		iOwnTwOgre = true;
	}

	TweakBarSupervisor::TweakBarSupervisor(TwOgre::WindowManager * wm)
	{
		windowManager = wm;
		iOwnTwOgre = false;
	}

	TweakBarSupervisor::~TweakBarSupervisor()
	{
		//delete all remaining TweakBars
		//TODO implement as stl algorithm
		for(tweakBarsVector_t::iterator itor = tweakBars.begin() ; itor != tweakBars.end() ; ++itor)
		{
			ARTSEA_LOW_LEVEL_ASSERT((*itor).second != NULL, "NULL pointer found in between TweakBars while destroying TweakBarSupervisor.");

			delete (*itor).second;
		}

		if(iOwnTwOgre)
		{
			delete windowManager;
		}
	}

	TweakBarSupervisor& TweakBarSupervisor::getSingleton()
	{
		ARTSEA_LOW_LEVEL_ASSERT( ms_Singleton, "Singleton pointer is NULL - tried to use this class before creating an instance?." );
		return (*ms_Singleton);
	}
	
	TweakBarSupervisor* TweakBarSupervisor::getSingletonPtr()
	{
		return ms_Singleton;
	}

	TweakBar* TweakBarSupervisor::createTweakBar(const Ogre::String& name, const Ogre::String& title,
	const Ogre::ColourValue& windowColor, const Ogre::String& help)
	{
		ARTSEA_GUARD(TweakBarSupervisor::createTweakBar);
		
		//1) search for duplicates
		for(tweakBarsVector_t::iterator itor = tweakBars.begin() ; itor != tweakBars.end() ; ++itor)
		{
			if( (*itor).first == name )
			{
				throw std::runtime_error("Tried to register TweakBar duplicate.");
			}
		}

		//2) no duplicates? then create a new window

		TwOgre::Window* wnd = TwOgre::WindowManager::getSingleton().createWindow(name, title, windowColor, help);
		TweakBar* bar = new TweakBar(wnd);
		tweakBars.push_back(tweakBar_t(name, bar));

		return bar;

		ARTSEA_UNGUARD;
	}

	void TweakBarSupervisor::destroyTweakBar(const Ogre::String& name)
	{
		ARTSEA_GUARD(TweakBarSupervisor::destroyTweakBar[byName]);
		
		for(tweakBarsVector_t::iterator itor = tweakBars.begin() ; itor != tweakBars.end() ; ++itor)
		{
			if( (*itor).first == name )
			{
				ARTSEA_ASSERT((*itor).second != NULL, "NULL TweakBar found while deleting.");
				delete (*itor).second;

				tweakBars.erase(itor);

				return;
			}
		}

		ARTSEA_ASSERT(0, "Tried to delete a window multiple times!");

		ARTSEA_UNGUARD;
	}

	void TweakBarSupervisor::destroyTweakBar(TweakBar *window)
	{
		ARTSEA_GUARD(TweakBarSupervisor::destroyTweakBar[byPointer]);
		ARTSEA_ASSERT(window != NULL, "NULL pointer passed to function.");

		for(tweakBarsVector_t::iterator itor = tweakBars.begin() ; itor != tweakBars.end() ; ++itor)
		{
			if( (*itor).second == window )
			{
				delete window;

				tweakBars.erase(itor);
				return;
			}
		}

		ARTSEA_ASSERT(0, "Tried to delete a window multiple times!");

		ARTSEA_UNGUARD;
	}

	static void updateSingleTweakBar(TweakBarSupervisor::tweakBar_t bar)
	{
		ARTSEA_GUARD(updateSingleTweakBar);
		ARTSEA_ASSERT(bar.second != NULL, "NULL TweakBar found while updating TweakBars!");

		bar.second->updateVariables();

		ARTSEA_UNGUARD;
	}

	void TweakBarSupervisor::update()
	{
		ARTSEA_GUARD(TweakBarSupervisor::update);

		std::for_each(tweakBars.begin(), tweakBars.end(), updateSingleTweakBar);

		ARTSEA_UNGUARD;
	}



} //end of namespace ergoTw

template<> ergoTw::TweakBarSupervisor* Ogre::Singleton<ergoTw::TweakBarSupervisor>::ms_Singleton = 0;

void justatest()
{
//	int test;
//	foobar.addVariable("Hello world", test);
//	foobar.addVariable("Hello sunshine", test, ergoTw::TweakerProperties<int>().max(2).min(1).step(1));
}

//sth goes here.