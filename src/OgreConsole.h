#ifndef __OgreConsole_h_
#define __OgreConsole_h_

#include "stdafx.h"
#include "Interface.h"

class ChatCommand;
class BaseApplication;

class OgreConsole : public InterfaceObject
{
	public:
		OgreConsole(BaseApplication* pApp, Interface* pInterface);
		~OgreConsole();

		virtual void toggleVisible();

		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool keyPressed(const OIS::KeyEvent &arg);

		void addLine(std::string text);
		void performCommand(std::string text);

		InterfacePanel getPanelType() const { return INTFACE_PANEL_CONSOLE; }

	private:
		bool executeCommand(ChatCommand *table, std::string text);

		bool handleFullscreenCmd(const char* args);
		bool handleQuitCmd(const char* args);
		bool handleFpsWindowCmd(const char* args);
		bool handleReloadTexturesCmd(const char* args);
		bool handleDetailsCmd(const char* args);
		bool handleReloadInterfaceCmd(const char* args);
		bool handleSaveTerrainCmd(const char* args);

		ChatCommand * getCommandTable();

		OgreBites::Label* m_pTitleLabel;
		OgreBites::TextBox* m_pWindow;
		OgreBites::TextBox* m_pPrompt;

		Ogre::OverlayContainer* m_pOverylayContainer;

		DWORD m_uiPromptTimer;

		std::string m_prompt;
};

class ChatCommand
{
	public:
		const char* Name;
		bool (OgreConsole::*Handler)(const char* args);
		ChatCommand* ChildCommands;
};

#endif