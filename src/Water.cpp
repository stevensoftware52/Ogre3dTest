#include "stdafx.h"
#include "Water.h"
#include "BaseApplication.h"

Water::Water(BaseApplication* pApp, SkyX::SkyX* pSkyX) :
	m_pApp(pApp),
	m_pHydrax(NULL)
{
	// Create Hydrax object
	m_pHydrax = new Hydrax::Hydrax(pApp->m_pSceneMgr, pApp->m_pCamera, pApp->m_pWindow->getViewport(0));

	// Create our projected grid module  
	Hydrax::Module::ProjectedGrid *mModule = new Hydrax::Module::ProjectedGrid(
		
		// Hydrax parent pointer
		m_pHydrax,

		// Noise module
		new Hydrax::Noise::Perlin(/*Generic one*/),

		// Base plane
		Ogre::Plane(Ogre::Vector3(0, 1, 0), Ogre::Vector3(0, 0, 0)),

		// Normal mode
		Hydrax::MaterialManager::NM_RTT,

		// Projected grid options
		Hydrax::Module::ProjectedGrid::Options(/*264 /*Generic one*/));

	// Set our module
	m_pHydrax->setModule(static_cast<Hydrax::Module::Module*>(mModule));

	// Load all parameters from config file
	// Remarks: The config file must be in Hydrax resource group.
	// All parameters can be set/updated directly by code(Like previous versions),
	// but due to the high number of customizable parameters, since 0.4 version, Hydrax allows save/load config files.
	m_pHydrax->loadCfg("hydrax_config.hdx");

	// Create water
    m_pHydrax->create();
}

Water::~Water()
{
	delete m_pHydrax;
}

void Water::applyTerrainTechnique(Ogre::TerrainGroup* pTerrainGroup, Ogre::TerrainGlobalOptions* pTerrainOptions)
{
	Ogre::TerrainGroup::TerrainIterator ti = pTerrainGroup->getTerrainIterator();
	
	while(ti.hasMoreElements())
	{
	    Ogre::Terrain* t = ti.getNext()->instance;
		auto tech = t->getMaterial()->createTechnique();
		m_pHydrax->getMaterialManager()->addDepthTechnique(tech);
	}
}

bool Water::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	m_pHydrax->update(evt.timeSinceLastFrame);
	return true;
}

/*
	m_materialName = "WaterSurface" + std::to_string(g_numWaters++);

	// Define a plane mesh that will be used for the ocean surface
	Ogre::Plane oceanSurface;
	oceanSurface.normal = Ogre::Vector3::UNIT_Y;
	oceanSurface.d = -2600;
	Ogre::MeshManager::getSingleton().createPlane(m_materialName.c_str(), Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, oceanSurface,
		1000, 1000, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
	
	// Surface Entity
	m_pWaterSurfaceEnt = m_pApp->m_pSceneMgr->createEntity(m_materialName.c_str(), m_materialName.c_str());

	// Shader Material
	m_activeMaterial = Ogre::MaterialManager::getSingleton().getByName("Ocean2_Cg");
	m_activeMaterial->load();

	// Config material
	if (!m_activeMaterial.isNull() && m_activeMaterial->getNumSupportedTechniques())
	{
		if (Ogre::Technique* currentTechnique = m_activeMaterial->getSupportedTechnique(0))
		{
			if (m_pActivePass = currentTechnique->getPass(0))
			{
                if (m_pActivePass->hasFragmentProgram())
                {
					m_activeFragmentProgram = m_pActivePass->getFragmentProgram();
					m_activeFragmentParameters = m_pActivePass->getFragmentProgramParameters();
                }

                if (m_pActivePass->hasVertexProgram())
                {
					m_activeVertexProgram = m_pActivePass->getVertexProgram();
					m_activeVertexParameters = m_pActivePass->getVertexProgramParameters();
                }

				if (!m_activeVertexParameters.isNull())
				{
					m_activeVertexParameters->_writeRawConstant(20, 0.200000003f);
					m_activeVertexParameters->_writeRawConstant(24, 25.0f);
					m_activeVertexParameters->_writeRawConstant(25, 26.0f);
					m_activeVertexParameters->_writeRawConstant(28, 0.0149999997f);
					m_activeVertexParameters->_writeRawConstant(29, 0.00499999989f);
				}
			}
		}
	}

	// Assign
	m_pWaterSurfaceEnt->setMaterialName("Ocean2_Cg");
	m_pApp->m_pSceneMgr->getRootSceneNode()->attachObject(m_pWaterSurfaceEnt);
*/

/*
	Ogre::MeshManager::getSingleton().remove(m_materialName.c_str());

	m_materialName.clear();
*/

/*
		BaseApplication* m_pApp;
		Ogre::Entity* m_pWaterSurfaceEnt;
		Ogre::MaterialPtr m_activeMaterial;
		Ogre::Pass* m_pActivePass;
		Ogre::GpuProgramPtr m_activeFragmentProgram;
		Ogre::GpuProgramPtr m_activeVertexProgram;
		Ogre::GpuProgramParametersSharedPtr m_activeFragmentParameters;
		Ogre::GpuProgramParametersSharedPtr m_activeVertexParameters;

		std::string m_materialName;
*/