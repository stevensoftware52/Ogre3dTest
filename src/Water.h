#ifndef __Water_h_
#define __Water_h_

#include "stdafx.h"

#include <SkyX/SkyX.h>

#include <Hydrax/Hydrax.h>
#include <Hydrax/Noise/Perlin/Perlin.h>
#include <Hydrax/Modules/ProjectedGrid/ProjectedGrid.h>

class BaseApplication;

class Water
{
	public:
		Water(BaseApplication* pApp, SkyX::SkyX* pSkyX);
		~Water();

		void applyTerrainTechnique(Ogre::TerrainGroup* pTerrainGroup, Ogre::TerrainGlobalOptions* pTerrainOptions);

		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

	private:
		BaseApplication* m_pApp;
		Hydrax::Hydrax* m_pHydrax;
};

#endif