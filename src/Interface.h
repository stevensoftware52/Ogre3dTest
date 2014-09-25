#ifndef __Interface_h_
#define __Interface_h_

#include "stdafx.h"
#include "BaseApplication.h"

enum InterfacePanel
{
	INTFACE_PANEL_ERROR,
	INTFACE_PANEL_CONSOLE,
	INTFACE_PANEL_DETAILS,

	NUM_INTERFACE_PANELS
};

class OgreConsole;

class InterfaceObject
{
	public:
		InterfaceObject(BaseApplication* pApp, Interface* pInterface);
	
		virtual void toggleVisible() {}
	
		// Return false to stop other interfaces from happening
		virtual bool keyPressed(const OIS::KeyEvent &arg) { return true; }
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) { return true; }
	
		virtual InterfacePanel getPanelType() const { return INTFACE_PANEL_ERROR; }
	
	protected:
		Interface* m_pInterface;
		BaseApplication* m_pApp;
		OgreBites::SdkTrayManager* m_pTrayMgr;
};

class Interface
{
	public:
		Interface(BaseApplication* pApp);
		~Interface();
	
		void toggleObjectVisibility(InterfacePanel panel)
		{
			InterfaceObject* pObj = getInterfacePanel(panel);
			assert(pObj);
			pObj->toggleVisible();
		}

		virtual bool mouseMoved(const OIS::MouseEvent &arg);
		virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

		// return false means to terminate
		bool keyPressed(const OIS::KeyEvent &arg);
		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		OgreConsole* getConsole() { return (OgreConsole*)getInterfacePanel(INTFACE_PANEL_DETAILS); }

		// Ogre3d's basic interface
		OgreBites::SdkTrayManager* m_pTrayMgr;

	private:
		InterfaceObject* getInterfacePanel(InterfacePanel panel)
		{
			for (size_t i = 0; i < m_vInterfaceObjects.size(); ++i)
				if (m_vInterfaceObjects[i]->getPanelType() == panel)
					return m_vInterfaceObjects[i];

			return NULL;
		}

		std::vector<InterfaceObject*> m_vInterfaceObjects;

};

#endif