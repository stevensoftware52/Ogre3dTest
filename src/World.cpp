#include "stdafx.h"

#include "BaseApplication.h"
#include "World.h"
#include "Interface.h"
#include "OgreConsole.h"

Ogre::TerrainGroup* g_terrainGroup = 0;

#ifdef _DEBUG
	const float g_worldSize = 48000.0f;
	const float g_worldBounds = (g_worldSize / 2.0f);
	const float g_seaLevel = 1300.0f;
	const float g_treeDistance = 500.0f;
	const float g_treeWindFactor = 7.0f;
	const float g_grassDistance = 500.0f;
	const float g_grassDensity = 0.04f;
	const float g_plantWindFactor = 3.0f;
	const float g_plantDistance = 500.0f;
	const float g_cloudOpacity = 1.0f;
	const float g_cloudSpeed = 100.0f;
	const float g_cloudFallX = 0;
	const float g_cloudFallY = 0;
	const float g_compositeMapDistance = g_worldSize;
	const float g_numberOfPlants = 50000;
	const float g_fogStartDist = 2000.0f;
	const float g_fogEndDist = 3000.0f;
	const float g_pageSize_Grass = 200.0f;
	const float g_pageSize_Plants = 100.0f;
	const float g_pageSize_Trees = 300.0f;
	
	const bool g_enableSkyX = false;
	const bool g_enablePagedGeometry = true;

	const Forests::FadeTechnique g_grassFadeType = FADETECH_ALPHA;
#else	
	const bool g_enableSkyX = true;
	const bool g_enableWater = true;
	const bool g_enablePagedGeometry = true;
	const bool g_enableFog = false;
	const bool g_saveTerrain = false;

	const float g_worldSize = 48000.0f;
	const float g_worldBounds = (g_worldSize / 2.0f);
	const float g_seaLevel = 1300.0f;
	const float g_treeDistance = 2000.0f;
	const float g_treeImposterDistance = 3000.0f;
	const float g_treeWindFactor = 7.0f;
	const float g_grassDistance = 2500.0f;
	const float g_grassDensity = 0.05f;
	const float g_plantWindFactor = 3.0f;
	const float g_plantDistance = 1000.0f;
	const float g_numberOfPlants = 1000000;
	const float g_cloudOpacity = 1.0f;
	const float g_cloudSpeed = 100.0f;
	const float g_cloudFallX = 0;
	const float g_cloudFallY = 0;
	const float g_fogStartDist = 8000.0f;
	const float g_fogEndDist = 10000.0f;
	const float g_pageSize_Grass = 250.0f;
	const float g_pageSize_Plants = 100.0f;
	const float g_pageSize_Trees = 250.0f;
	const float g_compositeMapDistance = g_enableFog ? g_fogEndDist + 1000.0f : g_worldSize;

	const Forests::FadeTechnique g_grassFadeType = FADETECH_ALPHA;
#endif
	
World::World(BaseApplication* pApp) :
	m_pApp(pApp),
	m_bTerrainsImported(false),
	m_pTerrainGroup(NULL),
	m_pTerrainGlobals(NULL),
	m_pSkyX(NULL),
	m_pBasicController(NULL),
	m_pGlobalLight(NULL),
	m_pWater(NULL)
{
	// Fog
	//

	if (g_enableFog)
	{
		m_pApp->m_pSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue(0.7, 0.7, 0.7), 0.0f, g_fogStartDist, g_fogEndDist);
	}

	// Camera
	//
	
	{
		pApp->m_pCamera->setFarClipDistance(30000.0f);
	}

	// Lighting
	//

	{		
		Ogre::Vector3 lightdir(0.55f, -0.3f, 0.75f);
		lightdir.normalise();

		m_pGlobalLight = m_pApp->m_pSceneMgr->createLight("tstLight");
		m_pGlobalLight->setType(Ogre::Light::LT_DIRECTIONAL);
		m_pGlobalLight->setDirection(lightdir);
		m_pGlobalLight->setDiffuseColour(Ogre::ColourValue::White);
		m_pGlobalLight->setSpecularColour(Ogre::ColourValue(0.4f, 0.4f, 0.4f));

		m_pApp->m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	}

	// Sky
	//

	if (g_enableSkyX)
	{
		// Create SkyX object
		m_pBasicController = new SkyX::BasicController();
		m_pSkyX = new SkyX::SkyX(m_pApp->m_pSceneMgr, m_pBasicController);
		
		// Create the sky
		m_pSkyX->create();
	
		// Add clouds
		m_pSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options(/* Default options */));
		//m_pSkyX->getAtmosphereManager()->setOptions(
		//	SkyX::AtmosphereManager::Options(9.77501f, 10.2963f, 0.01f, 0.00545f, 0.000375f, 30, Ogre::Vector3(0.55f, 0.54f, 0.52f), -0.991f, 1, 4));

		// Register listeners
		m_pApp->m_pRoot->addFrameListener(m_pSkyX);
		m_pApp->m_pWindow->addListener(m_pSkyX);

		// Time
		m_pSkyX->setTimeMultiplier(0.0f);
		m_pBasicController->setMoonPhase(0.0f);
		m_pBasicController->setTime(Ogre::Vector3(18.0f, 7.5f, 20.5f));
	}
	else
	{
		m_pApp->m_pSceneMgr->setSkyBox(true, "Sky/Clouds");
	}
	
	// Sea Level
	//

	if (g_enableWater)
	{
		// Must be created after SkyX, but before anything else for the purpose of
		// apply depth techniques
		m_pWater = new Water(pApp, m_pSkyX);		
	}

	// Terrain
	//
	
	{	
		// First, we create a new set of global terrain options, using the TerrainGlobalOptions class. 
		// This is an options class that just stores default options for all terrains we will create and provides a few getters and setters. 
		// There are also local options to each TerrainGroup called DefaultImportSettings, which you will see later.
			
		m_pTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();	
		
		// Then we construct our TerrainGroup object 
		//	- a helper class to manage a grid of terrains but it does not do any paging (which is done by the paging component you can see in Intermediate tutorials soon).
			
		m_pTerrainGroup = OGRE_NEW Ogre::TerrainGroup(pApp->m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, g_worldSize);
		m_pTerrainGroup->setFilenameConvention(Ogre::String("composite_terrain"), Ogre::String("dat"));
		m_pTerrainGroup->setOrigin(Ogre::Vector3::ZERO);	

		g_terrainGroup = m_pTerrainGroup;

		// The TerrainGroup class constructor takes our SceneManager instance, Terrain alignment option, terrain size and terrain world size as parameters. 
		// Then we tell the TerrainGroup what name we would like it to use when saving our terrain, using the setFilenameConvention function.
		// And lastly we set the origin of the terrain group.
		//
		// Now it's time to configure our terrain:

		configureTerrainDefaults(m_pGlobalLight);

		// And then we define our terrains and instruct the TerrainGroup to load them all:
		// Since we only have one terrain, we'll only be calling the defineTerrain function once. 
		// But if we had multiple, we would do that there. 

		defineTerrain(0, 0);

		// sync load since we want everything in place when we start
		m_pTerrainGroup->loadAllTerrains(true);

		// Now, if we just imported our terrains, we would like our blendmaps to be calculated:
		// The code loops through the available terrains and calls initBlendMaps on each. 

		if (m_bTerrainsImported)
		{
			Ogre::TerrainGroup::TerrainIterator ti = m_pTerrainGroup->getTerrainIterator();
			while (ti.hasMoreElements())
			{
				Ogre::Terrain* t = ti.getNext()->instance;
				assert(t);
				initBlendMaps(t);
			}
		}
	
		// Now, all there is left to do is clean up after the initial terrain creation:

		m_pTerrainGroup->freeTemporaryResources();

		// HydraX needs to know about the terrain in order to apply depth technique

		if (g_enableWater)
			m_pWater->applyTerrainTechnique(m_pTerrainGroup, m_pTerrainGlobals);
	}

	// PagedGeometry (Grass)
	//

	if (g_enablePagedGeometry)
	{
		// Create and configure a new PagedGeometry instance for grass
		PagedGeometry* grass = new PagedGeometry(m_pApp->m_pCamera, g_pageSize_Grass);
		grass->addDetailLevel<GrassPage>(g_grassDistance);
	
		// Create a GrassLoader object
		GrassLoader* grassLoader = new GrassLoader(grass);
		
		// Assign the loader to be used to load geometry for the PagedGeometry instance
		grass->setPageLoader(grassLoader);
	
		// Supply a height function to GrassLoader so it can calculate grass Y values
		grassLoader->setHeightFunction(&getTerrainHeight);
	
		// Add some grass to the scene with GrassLoader::addLayer()
		GrassLayer* pLayer = grassLoader->addLayer("grass");

		// Configure the grass layer properties (size, density, animation properties, fade settings, etc.)
		pLayer->setMinimumSize(10.0f, 10.0f);
		pLayer->setMaximumSize(10.0f, 10.0f);
		
		// Enable animations
		pLayer->setAnimationEnabled(true);
		
		// Sway fairly unsynchronized
		pLayer->setSwayDistribution(7.0f);
		
		// Sway back and forth 0.5 units in length
		pLayer->setSwayLength(0.5f);
		
		// Sway 1/2 a cycle every second
		pLayer->setSwaySpeed(0.4f);
		
		// Relatively dense grass
		pLayer->setDensity(g_grassDensity);
		pLayer->setRenderTechnique(GRASSTECH_CROSSQUADS);
		
		// Distant grass should slowly raise out of the ground when coming in range
		pLayer->setFadeTechnique(Forests::FadeTechnique(g_grassFadeType));
		
		// [NOTE] This sets the color map, or lightmap to be used for grass. All grass will be colored according
		// to this texture. In this case, the colors of the terrain is used so grass will be shadowed/colored
		// just as the terrain is (this usually makes the grass fit in very well).
		pLayer->setColorMap("grass_texture_d.png");
		
		// This sets the density map that will be used to determine the density levels of grass all over the
		// terrain. This can be used to make grass grow anywhere you want to; in this case it's used to make
		// grass grow only on fairly level ground (see densitymap.png to see how this works).
		//pLayer->setDensityMap("densitymap.png");
		
		// setMapBounds() must be called for the density and color maps to work (otherwise GrassLoader wouldn't
		// have any knowledge of where you want the maps to be applied). In this case, the maps are applied
		// to the same boundaries as the terrain.
		pLayer->setMapBounds(TBounds(-g_worldBounds, -g_worldBounds, g_worldBounds, g_worldBounds));
		
		// Done, add it
		m_vPagedGeos.push_back(grass);
	}

	// Paged Geometry (Plants)
	// 

	if (g_enablePagedGeometry)
	{
		// Create and configure a new PagedGeometry instance
		PagedGeometry* pPlants = new PagedGeometry(m_pApp->m_pCamera, g_pageSize_Plants);	
	
		// Use infinite paging mode
		pPlants->setInfinite();			
	
		// Use X up to Y units away, and fade for Z more units
		pPlants->addDetailLevel<WindBatchPage>(g_plantDistance, 0);
	
		// Create a new TreeLoader3D object
		TreeLoader3D* pTreeLoader = new TreeLoader3D(pPlants, TBounds(-g_worldBounds, -g_worldBounds, g_worldBounds, g_worldBounds));
		pPlants->setPageLoader(pTreeLoader);
	
		// Load plant entities
		std::vector<Ogre::Entity*> vPlants;
		vPlants.push_back(m_pApp->m_pSceneMgr->createEntity("farn1", "farn1.mesh"));
		vPlants.push_back(m_pApp->m_pSceneMgr->createEntity("farn2", "farn2.mesh"));
	
		for (size_t i = 0; i < vPlants.size(); ++i)
			pPlants->setCustomParam(vPlants[i]->getName(), "windFactorX", g_plantWindFactor);
		
		assert(vPlants.size());
	
		// Randomly place copies of the plants on the terrain
		for (int i = 0; i < g_numberOfPlants; i++)
		{
			// Random facing
			Ogre::Radian yaw = Ogre::Degree(Ogre::Math::RangeRandom(0, 360));
	
			// Random position
			//	[NOTE] Unlike TreeLoader3D, TreeLoader2D's addTree() function accepts a Vector2D position (x/z).
			//		   The Y value is calculated during runtime (to save memory) from the height function supplied (above).
	
			Ogre::Vector3 position = Ogre::Vector3::ZERO;
			position.x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
			position.z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
			position.y = getTerrainHeight(position.x, position.z, NULL);
	
			// Not near sea level
			if (position.y <= g_seaLevel + 100.0f)
				continue;
	
			// Random plant
			Ogre::Entity* randomTree = vPlants[int(Ogre::Math::RangeRandom(0, vPlants.size() - 1))];
			
			// Add it
			pTreeLoader->addTree(randomTree, position, yaw, 1.0f);
		}
	
		// Done, add it
		m_vPagedGeos.push_back(pPlants);
	}

	// Paged Geometry (Trees)
	// 

	if (g_enablePagedGeometry)
	{
		// Create and configure a new PagedGeometry instance
		PagedGeometry* pTrees = new PagedGeometry(m_pApp->m_pCamera, g_pageSize_Trees);
	
		// Use infinite paging mode
		pTrees->setInfinite();
	
		// Use X up to Y units away, and fade for Z more units
		pTrees->addDetailLevel<WindBatchPage>(g_treeDistance, 0);
		pTrees->addDetailLevel<ImpostorPage>(g_treeImposterDistance, 0);
	
		// Create a new TreeLoader3D object
		TreeLoader3D* pTreeLoader = new TreeLoader3D(pTrees, TBounds(-g_worldBounds, -g_worldBounds, g_worldBounds, g_worldBounds));
		pTrees->setPageLoader(pTreeLoader);
	
		// Load tree entities
		std::vector<Ogre::Entity*> vTrees;
		vTrees.push_back(m_pApp->m_pSceneMgr->createEntity("fir05_30", "fir05_30.mesh"));
		vTrees.push_back(m_pApp->m_pSceneMgr->createEntity("fir06_30", "fir06_30.mesh"));
		vTrees.push_back(m_pApp->m_pSceneMgr->createEntity("fir14_25", "fir14_25.mesh"));
		//vTrees.push_back(m_pApp->m_pSceneMgr->createEntity("Palm", "Palm.mesh"));

		for (size_t i = 0; i < vTrees.size(); ++i)
			pTrees->setCustomParam(vTrees[i]->getName(), "windFactorX", g_treeWindFactor);
	
		assert(vTrees.size());
	
		// Randomly place copies of the tree on the terrain
		for (int i = 0; i < 100000; i++)
		{
			// Random facing
			Ogre::Radian yaw = Ogre::Degree(Ogre::Math::RangeRandom(0, 360));
	
			// Random position
			//	[NOTE] Unlike TreeLoader3D, TreeLoader2D's addTree() function accepts a Vector2D position (x/z).
			//		   The Y value is calculated during runtime (to save memory) from the height function supplied (above).
	
			Ogre::Vector3 position = Ogre::Vector3::ZERO;
			position.x = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
			position.z = Ogre::Math::RangeRandom(-g_worldBounds, g_worldBounds);
			position.y = getTerrainHeight(position.x, position.z, NULL);
	
			// Not near sea level
			if (position.y <= g_seaLevel + 100.0f)
				continue;
	
			// Random tree
			Ogre::Entity* randomTree = vTrees[int(Ogre::Math::RangeRandom(0, vTrees.size() - 1))];
	
			// Add it
			pTreeLoader->addTree(randomTree, position, yaw, 1.0f);
		}
	
		// Done, add it
		m_vPagedGeos.push_back(pTrees);
	}
}

World::~World()
{
	if (g_enablePagedGeometry)
	{
		for (size_t i = 0; i < m_vPagedGeos.size(); ++i)
		{
			delete m_vPagedGeos[i]->getPageLoader();
			delete m_vPagedGeos[i];
		}
	}

	m_pTerrainGroup->removeAllTerrains();

	delete m_pTerrainGroup;
	delete m_pTerrainGlobals;

	m_pApp->m_pSceneMgr->destroyLight(m_pGlobalLight);
	
	delete m_pWater;

	if (g_enableSkyX)
		delete m_pSkyX;	
}

bool World::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	// Composite map save
	//
	
	if (g_saveTerrain)
	{
		if (!m_pTerrainGroup->isDerivedDataUpdateInProgress())
		{
			if (m_bTerrainsImported)
			{
				m_pTerrainGroup->saveAllTerrains(true);
				m_bTerrainsImported = false;
			}
		}
	}

	// Paged Geometry
	//

	if (g_enablePagedGeometry)
	{
		for (size_t i = 0; i < m_vPagedGeos.size(); ++i)
			m_vPagedGeos[i]->update();
	}
	
	// SkyX (TODO: Lighting Proper)
	//

	if (g_enableSkyX)
	{
		// Lighting
		Ogre::SceneManager* sceneManager = m_pApp->m_pSceneMgr;
		
		Ogre::Vector3 lightDir = m_pBasicController->getSunDirection();
		Ogre::Vector3 moonDir = m_pBasicController->getMoonDirection();
		
		// Calculate current colour gradient point
		float point = (-lightDir.y + 1.0f) / 2.0f;
		
		// Sun and ambient colors
		Ogre::ColourValue sunCol = Util::vectorToColor(m_pSkyX->getVCloudsManager()->getSunGradient().getColor(point));
		Ogre::ColourValue ambCol = Util::vectorToColor(m_pSkyX->getVCloudsManager()->getAmbientGradient().getColor(point));
		
		// Set sun direction and colors
		m_pGlobalLight->setDirection(-lightDir);
		m_pGlobalLight->setSpecularColour(ambCol * Ogre::ColourValue(1.9f, 1.9f, 1.9f));
		m_pGlobalLight->setDiffuseColour(sunCol * Ogre::ColourValue(1.9f, 1.9f, 1.9f));
	}

	// Water
	//

	if (g_enableWater)
	{
		m_pWater->frameRenderingQueued(evt);
	}

	return true;
}

void World::configureTerrainDefaults(Ogre::Light* light)
{
	// Configure global
	m_pTerrainGlobals->setMaxPixelError(8);

	// testing composite map
	m_pTerrainGlobals->setCompositeMapDistance(g_compositeMapDistance);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	m_pTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	m_pTerrainGlobals->setCompositeMapAmbient(m_pApp->m_pSceneMgr->getAmbientLight());
	m_pTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
	m_pTerrainGlobals->setSkirtSize(1);

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = m_pTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 1024;
	defaultimp.worldSize = g_worldSize;
	defaultimp.inputScale = 4096; // due terrain.png is 8 bpp
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;

	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 128;
	defaultimp.layerList[0].textureNames.push_back("desert_sand_d.jpg");
	defaultimp.layerList[0].textureNames.push_back("desert_sand_n.jpg");
	defaultimp.layerList[1].worldSize = 128;
	defaultimp.layerList[1].textureNames.push_back("savanna_yellow_d.jpg");
	defaultimp.layerList[1].textureNames.push_back("savanna_yellow_n.jpg");
	defaultimp.layerList[2].worldSize = 64;
	defaultimp.layerList[2].textureNames.push_back("grass_texture_d.png");	
	defaultimp.layerList[2].textureNames.push_back("grass_texture_n.png");
}

void World::initBlendMaps(Ogre::Terrain* terrain)
{
	// This function defines which texture is used where
	//	Seemingly we can only do it by heights ?

	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = g_seaLevel;
	Ogre::Real fadeDist0 = 50;
	Ogre::Real minHeight1 = g_seaLevel + fadeDist0;
	Ogre::Real fadeDist1 = 300;
	
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}

	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}

void World::defineTerrain(long x, long y)
{
	Ogre::String filename = m_pTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(m_pTerrainGroup->getResourceGroup(), filename))
	{
		m_pTerrainGroup->defineTerrain(x, y);
	}
	else
	{
		Ogre::Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		m_pTerrainGroup->defineTerrain(x, y, &img);
		m_bTerrainsImported = true;
	}
}

void World::getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("map.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	
	if (flipX)
		img.flipAroundY();

	if (flipY)
		img.flipAroundX();
}

float World::getTerrainHeight(Ogre::Real x, Ogre::Real z, void* unk)
{
	Ogre::Ray ray;
	ray.setOrigin(Ogre::Vector3(x, 10000.0f, z));
	ray.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);

	Ogre::TerrainGroup::RayResult rayResult = g_terrainGroup->rayIntersects(ray);

	if (rayResult.hit)
		return rayResult.position.y;

	return 0.0f;
}