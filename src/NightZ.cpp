#include "stdafx.h"
#include "NightZ.h"
#include "World.h"

NightZ::NightZ(void)
{

}

NightZ::~NightZ(void)
{

}

void NightZ::createScene(void)
{
	m_pWorld = new World(this);
}

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	NightZ app;

	try
	{
		app.go();
	}
	catch (Ogre::Exception& e)
	{
		MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
	}

    return 0;
}