#ifndef __DetailsPanel_h_
#define __DetailsPanel_h_

#include "stdafx.h"
#include "Interface.h"

class BaseApplication;

class DetailsPanel : public InterfaceObject
{
	public:
		DetailsPanel(BaseApplication* pApp, Interface* pInterface);
		~DetailsPanel();

		virtual void toggleVisible();

		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
		virtual bool keyPressed(const OIS::KeyEvent &arg);

		InterfacePanel getPanelType() const { return INTFACE_PANEL_DETAILS; }
		
	private:
	    OgreBites::ParamsPanel* m_pDetailsPanel;   
};

#endif