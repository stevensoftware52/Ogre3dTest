#include "stdafx.h"
#include "Interface.h"
#include "DetailsPanel.h"
#include "OgreConsole.h"

// ------------------------------------------
// Constructor (InterfaceObject)
InterfaceObject::InterfaceObject(BaseApplication* pApp, Interface* pInterface) :
	m_pInterface(pInterface),
	m_pApp(pApp),
	m_pTrayMgr(pInterface->m_pTrayMgr)
{

}

// ------------------------------------------
// Constructor (Interface)
Interface::Interface(BaseApplication* pApp)
{
	m_pTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", pApp->m_pWindow, pApp->m_pMouse, pApp);
	m_pTrayMgr->hideCursor();

	// Order of operation somewhat important to decide initial "whats on top" order
	//

	// OgreConsole
	m_vInterfaceObjects.push_back(new OgreConsole(pApp, this));

	// DetailsPanel
	m_vInterfaceObjects.push_back(new DetailsPanel(pApp, this));
}

// ------------------
// Destructor
Interface::~Interface()
{
	delete m_pTrayMgr;

	for (size_t i = 0; i < m_vInterfaceObjects.size(); ++i)
		delete m_vInterfaceObjects[i];

	m_vInterfaceObjects.clear();
}

// ------------------
// keyPressed
bool Interface::keyPressed(const OIS::KeyEvent &arg)
{
	if (m_pTrayMgr->isDialogVisible() == true)
		return false;

	// Interface grabs input when object return false
	for (size_t i = 0; i < m_vInterfaceObjects.size(); ++i)
		if (m_vInterfaceObjects[i]->keyPressed(arg) == false)
			return false;

	return true;
}

// -----------------------
// frameRenderingQueued
bool Interface::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	m_pTrayMgr->frameRenderingQueued(evt);

	// Interface grabs input when object return false
	for (size_t i = 0; i < m_vInterfaceObjects.size(); ++i)
		if (m_vInterfaceObjects[i]->frameRenderingQueued(evt) == false)
			return false;

	return true;
}

// -----------------------
// mouseMoved
bool Interface::mouseMoved(const OIS::MouseEvent &arg)
{
	if (m_pTrayMgr->injectMouseMove(arg))
		return false;

	// Interface grabs mouse when cursor is visible
	if (m_pTrayMgr->isCursorVisible())
		return false;

	return true;
}

// -----------------------
// mousePressed
bool Interface::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (m_pTrayMgr->injectMouseDown(arg, id))
		return false;

	// Interface grabs mouse when cursor is visible
	if (m_pTrayMgr->isCursorVisible())
		return false;

	return true;
}

// -----------------------
// mouseReleased
bool Interface::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (m_pTrayMgr->injectMouseUp(arg, id))
		return false;

	// Interface grabs mouse when cursor is visible
	if (m_pTrayMgr->isCursorVisible())
		return false;

	return true;
}