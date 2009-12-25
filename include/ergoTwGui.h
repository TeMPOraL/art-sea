/**
 ergoTw - a wrapper for TwOgreGui library that fixes some important elements missing from the library.

 FIXME insert some licence strings or whatever here.

*/

#ifndef __ARTSEA_ERGO_TW_GUI_H__
#define __ARTSEA_ERGO_TW_GUI_H__

#include <Ogre.h>
#include <TwOgre.h>

#include "Debug.h"


namespace ergoTw
{
	//================================================================
	//ergoTw Reference Variables
	//================================================================
	//Those classes wrap around TwOgre variables to enable reference
	//variables and a cascade syntax for variable properties.
	//For example:
	//testTweakBar->addRealVariable("Shared Real", testSharedReal)->range(0.0, 15.0, 0.3)
	//	->label("TestLabel")->group("Group1");
	//================================================================

	//----------------------------------------------------------------
	//Base class for ergoTw variables.
	//----------------------------------------------------------------
	//TODO
	// * Change those virtual functions so that they return proper type
	//	for each sub-class.
	//----------------------------------------------------------------
	class Variable
	{
	public:
		virtual Variable* helpString(const Ogre::String& str)
		{
			twOgreAsVariablePtr()->setHelp(str);
			return this;
		}
		virtual Variable* group(const Ogre::String& str)
		{
			twOgreAsVariablePtr()->setGroup(str);
			return this;
		}
		virtual Variable* label(const Ogre::String& str)
		{
			twOgreAsVariablePtr()->setLabel(str);
			return this;
		}

	protected:
		virtual TwOgre::Variable* twOgreAsVariablePtr() = 0;

	public:
		virtual void update() = 0;
	};

	//----------------------------------------------------------------
	//Boolean variable
	//----------------------------------------------------------------
	class BoolVariable : public Variable
	{
	public:
		BoolVariable(bool* varToBind, TwOgre::BoolVariable* twVar) : boundVar(varToBind), variable(twVar) { ARTSEA_ASSERT(boundVar != NULL, "NULL variable pointer passed to BoolVariable constructor."); ARTSEA_ASSERT(twVar != NULL, "NULL TwOgre Variable pointer passed to BoolVariable constructor."); prevState = *boundVar; twVar->setBoolValue(prevState); }

	public:
		virtual BoolVariable* displayStrings(const Ogre::String& trueString, const Ogre::String& falseString)
		{
			variable->setDisplayStrings(trueString, falseString);
			return this;
		}
		virtual BoolVariable* shortcut(const Ogre::String& toggle)
		{
			variable->setShortcut(toggle);
			return this;
		}

		virtual TwOgre::BoolVariable* getTwOgreVariable()
		{
			return variable;
		}

	protected:

		TwOgre::BoolVariable* variable;
		bool* boundVar;
		bool prevState;

		virtual TwOgre::Variable* twOgreAsVariablePtr()
		{
			return variable;
		}
	public:
		virtual void update()
		{
			//if prevState differs from twOgreGui state, then user probably changed something on the screen
			if( prevState != variable->getBoolValue() )
			{
				prevState = variable->getBoolValue();
				*boundVar = prevState;
			}
			else
			{
				prevState = *boundVar;
				variable->setBoolValue(*boundVar);
			}
		}
	};

	//----------------------------------------------------------------
	//Color variable
	//----------------------------------------------------------------
	class ColorVariable : public Variable
	{
	public:
		ColorVariable(Ogre::ColourValue* varToBind, TwOgre::ColorVariable* twVar) : boundVar(varToBind), variable(twVar) { ARTSEA_ASSERT(boundVar != NULL, "NULL variable pointer passed to ColorVariable constructor."); ARTSEA_ASSERT(twVar != NULL, "NULL TwOgre Variable pointer passed to ColorVariable constructor."); prevState = *boundVar; twVar->setColorValue(prevState); }

	public:
		virtual TwOgre::ColorVariable* getTwOgreVariable()
		{
			return variable;
		}

	protected:
		TwOgre::ColorVariable* variable;
		Ogre::ColourValue* boundVar;
		Ogre::ColourValue prevState;

		virtual TwOgre::Variable* twOgreAsVariablePtr()
		{
			return variable;
		}
	public:
		virtual void update()
		{
			//if prevState differs from twOgreGui state, then user probably changed something on the screen
			if( prevState != variable->getColorValue() )
			{
				prevState = variable->getColorValue();
				*boundVar = prevState;
			}
			else
			{
				prevState = *boundVar;
				variable->setColorValue(*boundVar);
			}
		}
	};

	//----------------------------------------------------------------
	//Enum variable
	//----------------------------------------------------------------
	class EnumVariable : public Variable
	{
	public:
		EnumVariable(int* varToBind, TwOgre::EnumVariable* twVar) : boundVar(varToBind), variable(twVar) { ARTSEA_ASSERT(boundVar != NULL, "NULL variable pointer passed to EnumVariable constructor."); ARTSEA_ASSERT(twVar != NULL, "NULL TwOgre Variable pointer passed to EnumVariable constructor."); prevState = *boundVar; twVar->setIntegerValue(prevState); }
	public:
		virtual EnumVariable* moreEnumValues(const TwOgre::EnumValueList& enumList)
		{
			variable->addEnumValues(enumList);
			return this;
		}
		virtual EnumVariable* shortcuts(const Ogre::String& increment, const Ogre::String& decrement)
		{
			variable->setShortcut(increment, decrement);
			return this;
		}

		virtual TwOgre::EnumVariable* getTwOgreVariable()
		{
			return variable;
		}

	protected:

		TwOgre::EnumVariable* variable;
		int* boundVar;
		int prevState;

		virtual TwOgre::Variable* twOgreAsVariablePtr()
		{
			return variable;
		}
	public:
		virtual void update()
		{
			//if prevState differs from twOgreGui state, then user probably changed something on the screen
			if( prevState != variable->getEnumValue() )
			{
				prevState = variable->getEnumValue();
				*boundVar = prevState;
			}
			else
			{
				prevState = *boundVar;
				variable->setIntegerValue(*boundVar);
			}
		}
	};

	//----------------------------------------------------------------
	//Integer variable
	//----------------------------------------------------------------
	class IntegerVariable : public Variable
	{
	public:
		IntegerVariable(int* varToBind, TwOgre::IntegerVariable* twVar) : boundVar(varToBind), variable(twVar) { ARTSEA_ASSERT(boundVar != NULL, "NULL variable pointer passed to IntegerVariable constructor."); ARTSEA_ASSERT(twVar != NULL, "NULL TwOgre Variable pointer passed to IntegerVariable constructor."); prevState = *boundVar; twVar->setIntegerValue(prevState); }

	public:
		virtual IntegerVariable* range(int min, int max=std::numeric_limits<int>::infinity(), int step=std::numeric_limits<int>::infinity())
		{
			variable->setLimits(min, max, step);
			return this;
		}

		virtual IntegerVariable* shortcuts(const Ogre::String& increment, const Ogre::String& decrement)
		{
			variable->setShortcut(increment, decrement);
			return this;
		}

		virtual IntegerVariable* displayAsHex()
		{
			variable->toHex();
			return this;
		}

		virtual IntegerVariable* displayAsDecimal()
		{
			variable->toDecimal();
			return this;
		}

		virtual TwOgre::IntegerVariable* getTwOgreVariable()
		{
			return variable;
		}

	protected:

		TwOgre::IntegerVariable* variable;
		int* boundVar;
		int prevState;

		virtual TwOgre::Variable* twOgreAsVariablePtr()
		{
			return variable;
		}
	public:
		virtual void update()
		{
			//if prevState differs from twOgreGui state, then user probably changed something on the screen
			if( prevState != variable->getIntegerValue() )
			{
				prevState = variable->getIntegerValue();
				*boundVar = prevState;
			}
			else
			{
				prevState = *boundVar;
				variable->setIntegerValue(*boundVar);
			}
		}
	};

	//----------------------------------------------------------------
	//Real variable
	//----------------------------------------------------------------
	class RealVariable : public Variable
	{
	public:
		RealVariable(Ogre::Real* varToBind, TwOgre::RealVariable* twVar) : boundVar(varToBind), variable(twVar) { ARTSEA_ASSERT(boundVar != NULL, "NULL variable pointer passed to RealVariable constructor."); ARTSEA_ASSERT(twVar != NULL, "NULL TwOgre Variable pointer passed to RealVariable constructor."); prevState = *boundVar; twVar->setRealValue(prevState); }

	public:
		virtual RealVariable* range(Ogre::Real min, Ogre::Real max=std::numeric_limits<Ogre::Real>::infinity(), Ogre::Real step=std::numeric_limits<Ogre::Real>::infinity())
		{
			variable->setLimits(min, max, step);
			return this;
		}

		virtual RealVariable* shortcuts(const Ogre::String& increment, const Ogre::String& decrement)
		{
			variable->setShortcut(increment, decrement);
			return this;
		}

		virtual RealVariable* precision(int precision)
		{
			variable->setPrecision(precision);
			return this;
		}

		virtual TwOgre::RealVariable* getTwOgreVariable()
		{
			return variable;
		}

	protected:

		TwOgre::RealVariable* variable;
		Ogre::Real* boundVar;
		Ogre::Real prevState;

		virtual TwOgre::Variable* twOgreAsVariablePtr()
		{
			return variable;
		}
	public:
		virtual void update()
		{
			//if prevState differs from twOgreGui state, then user probably changed something on the screen
			//FIXME change comparison to: abs(a - b) < eps
			if( prevState != variable->getRealValue() )
			{
				prevState = variable->getRealValue();
				*boundVar = prevState;
			}
			else
			{
				prevState = *boundVar;
				variable->setRealValue(*boundVar);
			}
		}
	};

	//----------------------------------------------------------------
	//String variable
	//----------------------------------------------------------------
	class StringVariable : public Variable
	{
	public:
		StringVariable(Ogre::String* varToBind, TwOgre::StringVariable* twVar) : boundVar(varToBind), variable(twVar) { ARTSEA_ASSERT(boundVar != NULL, "NULL variable pointer passed to StringVariable constructor."); ARTSEA_ASSERT(twVar != NULL, "NULL TwOgre Variable pointer passed to StringVariable constructor."); prevState = *boundVar; twVar->setStringValue(prevState); }
	public:
		virtual TwOgre::StringVariable* getTwOgreVariable()
		{
			return variable;
		}
	protected:

		TwOgre::StringVariable* variable;
		Ogre::String* boundVar;
		Ogre::String prevState;

		virtual TwOgre::Variable* twOgreAsVariablePtr()
		{
			return variable;
		}
	public:
		virtual void update()
		{
			//if prevState differs from twOgreGui state, then user probably changed something on the screen
			if( prevState != variable->getStringValue() )
			{
				prevState = variable->getStringValue();
				*boundVar = prevState;
			}
			else
			{
				prevState = *boundVar;
				variable->setStringValue(*boundVar);
			}
		}
	};

	//================================================================
	//TweakBar - replacement for TwOgre::Window
	//================================================================
	class TweakBar
	{
	public:

		TweakBar(TwOgre::Window* tweakWindow) : window(tweakWindow) {}
		~TweakBar();

		TwOgre::Window* getTwOgreWindow()
		{
			return window;
		}

		//----------------------------------------------------------------
		//Use following functions to create variables.

		BoolVariable* addBoolVariable(const Ogre::String& name, bool& variable, bool readOnly=false);

		ColorVariable* addColorVariable(const Ogre::String& name, Ogre::ColourValue& variable, bool readOnly=false);

		EnumVariable* addEnumVariable(const Ogre::String& name, int& value, const TwOgre::EnumValueList& enumList=TwOgre::EnumValueList(), bool readOnly=false);

		IntegerVariable* addIntegerVariable(const Ogre::String& name, int& value, bool readOnly=false);		

		RealVariable* addRealVariable(const Ogre::String& name, Ogre::Real& value, bool readOnly=false);

		StringVariable* addStringVariable(const Ogre::String& name, Ogre::String& value, bool readOnly=false);

		void updateVariables();

	protected:
		TwOgre::Window* window;

		typedef std::vector<Variable*> variableVector_t;
		variableVector_t variables;
	};

	//================================================================
	//TweakBarSupervisor - replacement for TwOgre::WindowManager
	//================================================================
	class TweakBarSupervisor : public Ogre::Singleton<TweakBarSupervisor>
	{
	public:
		/**
		 * Constructor.
		 * @param renderWindow Pointer to the Ogre::RenderWindow to render into.
		 * @param sceneManager Ponter to the Ogre::SceneManager to associate the Windows with.
		 */
		TweakBarSupervisor(Ogre::RenderWindow *renderWindow, Ogre::SceneManager *sceneManager);

		//constructor that attaches to an existing instance of TwOgre::WindowManager
		TweakBarSupervisor(TwOgre::WindowManager * wm);
		/** Destructor. */
		~TweakBarSupervisor();

		/** Returns the Listener singleton object */
		static TweakBarSupervisor& getSingleton();
		/** Returns a pointer to the Listener singleton object */
		static TweakBarSupervisor* getSingletonPtr();

		TweakBar* createTweakBar(const Ogre::String& name, const Ogre::String& title="",
		const Ogre::ColourValue& windowColor=Ogre::ColourValue::ZERO, const Ogre::String& help="");

		void destroyTweakBar(const Ogre::String& name);

		void destroyTweakBar(TweakBar *window);

		void update();

		TwOgre::WindowManager* getTwOgreWindowManager()
		{
			return windowManager;
		}

		//----------------------------------------------------------------
		//Use following functions in Ogre Window Event Listener to pass input
		//into TwOgreGui

		/** Notify the TweakBarSupervisor of a MouseMoved event */
		bool injectMouseMoved(const OIS::MouseEvent& mouseEvent)
		{
			ARTSEA_GUARD(TweakBarSupervisor::injectMouseMoved);
			ARTSEA_ASSERT(windowManager != NULL, "Pointer to TwOgre::WindowManager not set.");
			
			return windowManager->injectMouseMoved(mouseEvent);

			ARTSEA_UNGUARD;
		}
		/** Notify the TweakBarSupervisor of a MousePressed event */
		bool injectMousePressed(const OIS::MouseEvent& mouseEvent, OIS::MouseButtonID id)
		{
			ARTSEA_GUARD(TweakBarSupervisor::injectMousePressed);
			ARTSEA_ASSERT(windowManager != NULL, "Pointer to TwOgre::WindowManager not set.");
			
			return windowManager->injectMousePressed(mouseEvent, id);

			ARTSEA_UNGUARD;
		}
		/** Notify the TweakBarSupervisor of a MouseReleased event */
		bool injectMouseReleased(const OIS::MouseEvent& mouseEvent, OIS::MouseButtonID id)
		{
			ARTSEA_GUARD(TweakBarSupervisor::injectMouseReleased);
			ARTSEA_ASSERT(windowManager != NULL, "Pointer to TwOgre::WindowManager not set.");
			
			return windowManager->injectMouseReleased(mouseEvent, id);

			ARTSEA_UNGUARD;
		}

		/** Notify the TweakBarSupervisor of a KeyPressed event */
		bool injectKeyPressed(const OIS::KeyEvent& keyEvent)
		{
			ARTSEA_GUARD(TweakBarSupervisor::injectKeyPressed);
			ARTSEA_ASSERT(windowManager != NULL, "Pointer to TwOgre::WindowManager not set.");
			
			return windowManager->injectKeyPressed(keyEvent);

			ARTSEA_UNGUARD;
		}
		/** Notify the TweakBarSupervisor of a KeyReleased event */
		bool injectKeyReleased(const OIS::KeyEvent& keyEvent)
		{
			ARTSEA_GUARD(TweakBarSupervisor::injectKeyReleased);
			ARTSEA_ASSERT(windowManager != NULL, "Pointer to TwOgre::WindowManager not set.");
			
			return windowManager->injectKeyReleased(keyEvent);

			ARTSEA_UNGUARD;
		}

	public:
		typedef std::pair<Ogre::String, TweakBar*> tweakBar_t;

	protected:
		TwOgre::WindowManager* windowManager;	//TwOgre window manager
		bool iOwnTwOgre;	//true if this class was used to create TwOgre::WindowManager

		typedef std::vector<tweakBar_t> tweakBarsVector_t;
		tweakBarsVector_t tweakBars;
	};

} //end of namespace ergoTw

#endif //__ARTSEA_ERGO_TW_GUI_H__