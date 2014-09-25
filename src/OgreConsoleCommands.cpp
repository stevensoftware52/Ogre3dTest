#include "stdafx.h"
#include "OgreConsole.h"
#include "World.h"

// ----------------------
// executeCommand
bool OgreConsole::executeCommand(ChatCommand *table, std::string strText)
{
	if (strText.size() == 0)
		return false;

	char const* text = strText.c_str();
	char const* oldtext = text;
	std::string cmd = "";

	while (*text != ' ' && *text != '\0')
	{
		cmd += *text;
		++text;
	}

	while (*text == ' ')
		++text;

	for (unsigned int i = 0; table[i].Name != NULL; ++i)
	{
		if (!Util::hasStringAbbr(table[i].Name, cmd.c_str()))
			continue;

		// select subcommand from child commands list
		if (table[i].ChildCommands != NULL)
		{
			if (!executeCommand(table[i].ChildCommands, text))
			{
				if (text && text[0] != '\0')
					addLine("No such command.");
				else
					addLine("Error.");

				//ShowHelpForCommand(table[i].ChildCommands, text);
			}

			return true;
		}

		// must be available and have handler
		if (!table[i].Handler)
			continue;

		return (this->*(table[i].Handler))(strlen(table[i].Name) != 0 ? text : oldtext);
	}

	return false;
}

// --------------------
// getCommandTable
ChatCommand* OgreConsole::getCommandTable()
{
	// m_pRoot->getRenderSystem()->getna

	static ChatCommand reloadTable[] =
	{
		// Name				Handler											Child Commands
		{ "textures",		&OgreConsole::handleReloadTexturesCmd,			NULL },
		{ "interface",		&OgreConsole::handleReloadInterfaceCmd,			NULL },
        { NULL,             NULL,											NULL }
	};

	static ChatCommand saveTable[] =
	{
		// Name				Handler											Child Commands
		{ "terrain",		&OgreConsole::handleSaveTerrainCmd,				NULL },
        { NULL,             NULL,											NULL }
	};

	static ChatCommand commandTable[] =
	{
		// Name				Handler											Child Commands
		{ "fps",			&OgreConsole::handleFpsWindowCmd,				NULL		},
		{ "reload",			NULL,											reloadTable },
		{ "details",		&OgreConsole::handleDetailsCmd,					NULL		},
		{ "fullscreen",		&OgreConsole::handleFullscreenCmd,				NULL		},	
		{ "quit",			&OgreConsole::handleQuitCmd,					NULL		},
		{ "save",			NULL,											saveTable	},
        { NULL,             NULL,											NULL }
	};

	return commandTable;
}

// --------------------
// handleDetailsCmd
bool OgreConsole::handleDetailsCmd(const char* args)
{
	m_pApp->getInterface()->toggleObjectVisibility(INTFACE_PANEL_DETAILS);
	return true;
}

// --------------------
// handleFpsWindowCmd
bool OgreConsole::handleFpsWindowCmd(const char* args)
{
	if (!args)
		return false;

	std::string strArgs = args;

	if (strArgs == "show")
	{
		if (m_pTrayMgr->areFrameStatsVisible() == false)
			m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	}
	else if (strArgs == "hide")
	{
		if (m_pTrayMgr->areFrameStatsVisible() == true)
			m_pTrayMgr->hideFrameStats();
	}
	else if (strArgs == "basic")
	{
		m_pTrayMgr->toggleAdvancedFrameStats();
	}
	else
		addLine("`fps` options - show, hide, basic");

	return true;
}

// -----------------------------
// handleReloadTexturesCmd
bool OgreConsole::handleReloadTexturesCmd(const char* args)
{
	Ogre::TextureManager::getSingleton().reloadAll();
	addLine("Ogre TextureManager called `reloadAll`.");
	return true;
}

// -----------------------------
// handleQuitCmd
bool OgreConsole::handleQuitCmd(const char* args)
{
	m_pApp->shutdown();
	return true;
}

// -----------------------------
// handleReloadInterfaceCmd
bool OgreConsole::handleReloadInterfaceCmd(const char* args)
{
	m_pApp->reloadInterface();
	return true;
}

// ------------------------------
// handleFullscreenCmd
bool OgreConsole::handleFullscreenCmd(const char* args)
{
	int l, t;
	unsigned int w, h, c;
	bool value = !m_pApp->m_pWindow->isFullScreen();
	
	m_pApp->m_pWindow->getMetrics(w, h, c, l, t);
	m_pApp->m_pWindow->setFullscreen(value, w, h);
	m_pApp->m_pWindow->windowMovedOrResized();

	m_pApp->m_pRoot->getRenderSystem()->setConfigOption("Full Screen", value ? "Yes" : "No");
	m_pApp->m_pRoot->saveConfig();

	addLine("Full Screen set to `" + std::string(value ? "Yes" : "No") + "`.");
	return true;
}

// -------------------------
// handleSaveTerrainCmd
bool OgreConsole::handleSaveTerrainCmd(const char* args)
{
	m_pApp->getWorld()->saveTerrain();
	return true;
}