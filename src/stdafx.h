// stdafx.h
 
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <Windows.h>

// Ogre
#include <Ogre.h>
#include <OgreRay.h>
#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreLogManager.h>

// Ogre Terrain
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

// OIS
#include <OIS.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

// OgreSDK
#include <SdkTrays.h>
#include <SdkCameraMan.h>
 
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "Util.h"