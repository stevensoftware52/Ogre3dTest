#ifndef __World_h_
#define __World_h_

#include "stdafx.h"
#include "Water.h"

// PagedGeometry
#include <BatchPage.h>
#include <ImpostorPage.h>
#include <WindBatchPage.h>
#include <GrassLoader.h>
#include <TreeLoader3D.h>
#include <TreeLoader2D.h>
#include <PagedGeometry.h>

using namespace Forests;

// SkyX
#include <SkyX.h>

class World
{
	public:
		World(BaseApplication* pApp);
		~World();

		void saveTerrain() { m_pTerrainGroup->saveAllTerrains(true); }

		bool frameRenderingQueued(const Ogre::FrameEvent& evt);

		static float getTerrainHeight(Ogre::Real x, Ogre::Real z, void* unk);

	private:
		void configureGrassLayer(GrassLayer* pLayer);
		void configureTerrainDefaults(Ogre::Light* light);
		void defineTerrain(long x, long y);
		void initBlendMaps(Ogre::Terrain* terrain);
		void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img);

		bool m_bTerrainsImported;

		BaseApplication* m_pApp;

		// Light
		Ogre::Light* m_pGlobalLight; 

		// Terrain
		Ogre::TerrainGroup* m_pTerrainGroup;
		Ogre::TerrainGlobalOptions* m_pTerrainGlobals;

		// PagedGeometry
		std::vector<PagedGeometry*> m_vPagedGeos;

		// SkyX
		SkyX::SkyX* m_pSkyX;
		SkyX::BasicController* m_pBasicController;

		// Water
		Water* m_pWater;
};

#endif 