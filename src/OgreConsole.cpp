#include "stdafx.h"
#include "OgreConsole.h"
#include "BaseApplication.h"

#define CONSOLE_LINE_LENGTH 85
#define CONSOLE_LINE_COUNT 15

// ----------------------
// Constructor
OgreConsole::OgreConsole(BaseApplication* pApp, Interface* pInterface) : 
	InterfaceObject(pApp, pInterface),
	m_pTitleLabel(NULL),
	m_pWindow(NULL),
	m_pPrompt(NULL),
	m_uiPromptTimer(clock())
{
	m_pTitleLabel = m_pTrayMgr->createLabel(OgreBites::TL_TOPLEFT, "ConsoleTitle", "");
	m_pWindow = m_pTrayMgr->createTextBox(OgreBites::TL_TOPLEFT, "Console Window", "Console Window", 640, 380);
	m_pPrompt = m_pTrayMgr->createTextBox(OgreBites::TL_TOPLEFT, "ConsolePrompt", "Prompt", 640, 100);

	m_pOverylayContainer = pInterface->m_pTrayMgr->getTrayContainer(OgreBites::TL_TOPLEFT);

	addLine("Hello world!");
	addLine("Welcome to the NightZ Console Window!");

	m_prompt = "";

	// Console window hidden by default
	m_pOverylayContainer->hide();
}

// ----------------------
// Destructor
OgreConsole::~OgreConsole()
{
	m_prompt.clear();

	// Objects created by SDKTrayMgr are cleaned up when that object is destroyed
}

// ----------------------
// toggleVisible
void OgreConsole::toggleVisible()
{
	if (m_pOverylayContainer->isVisible())
	{
		m_pOverylayContainer->hide();
		m_pTrayMgr->hideCursor();
	}
	else
	{
		m_pTrayMgr->showCursor();
		m_pOverylayContainer->show();
	}
}

// ----------------------
// frameRenderingQueued
bool OgreConsole::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	m_pPrompt->setText(m_prompt);

	if (clock() - m_uiPromptTimer < 500)
		;
	else if (clock() - m_uiPromptTimer < 1000)
		m_pPrompt->appendText("|");
	else
		m_uiPromptTimer = clock();

	// Prompt input
	//	Done in here for the purpose of holding a key down to spam a letter or perhaps backspace
	if (m_pOverylayContainer->isVisible())
	{
		if (m_pApp->autoInputGetKey() == OIS::KC_BACK)
		{
			if (m_prompt.size())
				m_prompt.erase(m_prompt.end() - 1);
		}
		else if (char character = m_pApp->autoInputGetChar())
		{
			static std::string legal_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890+!\"#%&/()=?[]\\*-_.:,; ";

			if (legal_characters.find(character) != std::string::npos)
				m_prompt.push_back(character);
		}
	}

    return true;
}

// ----------------------
// keyPressed
bool OgreConsole::keyPressed(const OIS::KeyEvent &arg)
{
	// Grave key always toggle
	if (arg.key == OIS::KC_GRAVE)
		toggleVisible();

	if (!m_pOverylayContainer->isVisible())
		return true;

	// Escape key to close
	if (arg.key == OIS::KC_ESCAPE)
		toggleVisible();

	// Enter key
	if (arg.key == OIS::KC_RETURN)
	{
		performCommand(m_prompt);
		m_prompt.clear();
	}

	// Console has priority over other interfaces
	return false;
}

// ----------------------
// addLine
void OgreConsole::addLine(std::string text) 
{
	m_pWindow->appendText(text + "\n");
}

// ----------------------
// performCommand
void OgreConsole::performCommand(std::string text)
{
	addLine("[CMD]: " + text);

	if (!executeCommand(getCommandTable(), text))
		addLine("Failed to execute command `" + text + "`.");
}