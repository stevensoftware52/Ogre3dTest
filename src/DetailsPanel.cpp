#include "stdafx.h"
#include "DetailsPanel.h"
#include "BaseApplication.h"

// ----------------------
// Constructor
DetailsPanel::DetailsPanel(BaseApplication* pApp, Interface* pInterface) : 
	InterfaceObject(pApp, pInterface),
    m_pDetailsPanel(0)
{
	Ogre::StringVector strVector;

	strVector.push_back("cam.pX");	
	strVector.push_back("cam.pY");
	strVector.push_back("cam.pZ");
	strVector.push_back("");
	strVector.push_back("cam.oW");
	strVector.push_back("cam.oX");
	strVector.push_back("cam.oY");
	strVector.push_back("cam.oZ");
	strVector.push_back("");
	strVector.push_back("Filtering");
	strVector.push_back("Poly Mode");

	m_pDetailsPanel = m_pTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, strVector);

    m_pDetailsPanel->setParamValue(9, "Bilinear");
    m_pDetailsPanel->setParamValue(10, "Solid");
    m_pDetailsPanel->hide();
}

// ----------------------
// Destructor
DetailsPanel::~DetailsPanel()
{
	// Objects created by SDKTrayMgr are cleaned up when that object is destroyed
}

// ----------------------
// toggleVisible
void DetailsPanel::toggleVisible()
{
	if (m_pDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
    {
        m_pTrayMgr->moveWidgetToTray(m_pDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
        m_pDetailsPanel->show();
    }
    else
    {
        m_pTrayMgr->removeWidgetFromTray(m_pDetailsPanel);
        m_pDetailsPanel->hide();
    }
}

// ----------------------
// frameRenderingQueued
bool DetailsPanel::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (!m_pDetailsPanel->isVisible())  
		return true; 

    m_pDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedPosition().x));
    m_pDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedPosition().y));
    m_pDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedPosition().z));
    m_pDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedOrientation().w));
    m_pDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedOrientation().x));
    m_pDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedOrientation().y));
    m_pDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(m_pApp->m_pCamera->getDerivedOrientation().z));
	return true;
}

// ----------------------
// keyPressed
bool DetailsPanel::keyPressed(const OIS::KeyEvent &arg)
{
	if (m_pDetailsPanel->isVisible() == false)  
		return true; 	

	return true;
}