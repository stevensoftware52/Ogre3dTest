#include "stdafx.h"
#include "BaseApplication.h"
#include "ShaderGenerator.h"

#include "res/resource.h"
#include "Interface.h"
#include "World.h"

BaseApplication::BaseApplication(void) :
	m_pRoot(0),
    m_pCamera(0),
    m_pSceneMgr(0),
    m_pWindow(0),
    m_sResourcesCfg(Ogre::StringUtil::BLANK),
    m_sPluginsCfg(Ogre::StringUtil::BLANK),
    m_pCameraMan(0),
    m_bCursorWasVisible(false),
    m_bShutDown(false),
    m_pMouse(NULL),
	m_pKeyboard(NULL),
	m_pShaderGenerator(0),
	m_pMaterialMgrListener(0),
	m_pInterface(0),
	m_bReloadInterface(false),
	m_pWorld(NULL)
{
	//AllocConsole();;
	//freopen("conin$", "r", stdin);
	//freopen("conout$", "w", stdout);
	//freopen("conout$", "w", stderr);
}

BaseApplication::~BaseApplication(void)
{
	if (m_pCameraMan)
		delete m_pCameraMan;

	delete m_pWorld;

	// NightZ objects
	delete m_pInterface;

    // Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(m_pWindow, this);
    windowClosed(m_pWindow);
    delete m_pRoot;
}

bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if (m_pRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        m_pWindow = m_pRoot->initialise(true, "NightZ");

        // Let's add a nice window icon
        HWND hwnd;
        m_pWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
        LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON));
        SetClassLong( hwnd, GCL_HICON, iconID);
        return true;
    }
    else
    {
        return false;
    }
}

void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager
    m_pSceneMgr = m_pRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
}

void BaseApplication::createCamera(void)
{
    // Create the camera
    m_pCamera = m_pSceneMgr->createCamera("PlayerCam");

	// Config the camera
	m_pCamera->setPosition(Ogre::Vector3(1683.0f, 3100.0f, 2116.0f));
	m_pCamera->lookAt(Ogre::Vector3(1963.0f, 3100.0f, 1660.0f));
	m_pCamera->setNearClipDistance(0.1f);
	m_pCamera->setFarClipDistance(50000.0f);

	if (m_pRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
		m_pCamera->setFarClipDistance(0);

	// SDK Camera Man
	m_pCameraMan = new OgreBites::SdkCameraMan(m_pCamera);
	m_pCameraMan->setTopSpeed(m_pCameraMan->getTopSpeed() * 10.0f);
}

void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    m_pWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    m_pInputManager = OIS::InputManager::createInputSystem(pl);

	m_pMouse = static_cast<OIS::Mouse*>(m_pInputManager->createInputObject( OIS::OISMouse, true));
    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputManager->createInputObject( OIS::OISKeyboard, true));

    m_pMouse->setEventCallback(this);
    m_pKeyboard->setEventCallback(this);

    // Set initial mouse clipping size
    windowResized(m_pWindow);

    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(m_pWindow, this);

	// create interface object
	m_pInterface = new Interface(this);

    m_pRoot->addFrameListener(this);
}

void BaseApplication::destroyRTShaderSystem()
{
	// Restore default scheme.
	Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

	// Unregister the material manager listener.
	if (m_pMaterialMgrListener != NULL)
	{			
		Ogre::MaterialManager::getSingleton().removeListener(m_pMaterialMgrListener);
		delete m_pMaterialMgrListener;
		m_pMaterialMgrListener = NULL;
	}

	// Destroy RTShader system.
	if (m_pShaderGenerator != NULL)
	{				
		Ogre::RTShader::ShaderGenerator::finalize();
		m_pShaderGenerator = NULL;
	}
}

void BaseApplication::destroyScene(void)
{

}

void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = m_pWindow->addViewport(m_pCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    m_pCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void BaseApplication::locateResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(m_sResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String sec, type, arch;

    while (seci.hasMoreElements())
    {
        sec = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;

        for (i = settings->begin(); i != settings->end(); ++i)
        {
            type = i->first;
            arch = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, sec);
        }
    }
	
    // Add locations for supported shader languages
    const Ogre::ResourceGroupManager::LocationList genLocs = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList("General");
	arch = genLocs.front()->archive->getName();

	type = "FileSystem";
	sec = "Popular";

    if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
    {
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/programs/GLSLES", type, sec);
    }
    else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
    {
        if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/programs/GLSL150", type, sec);
        else
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/programs/GLSL", type, sec);

        if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl400"))
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/programs/GLSL400", type, sec);
    }
    else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("hlsl"))
    {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/programs/HLSL", type, sec);
    }

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/programs/Cg", type, sec);

    if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
    {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/GLSLES", type, sec);
    }
    else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
    {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/GLSL", type, sec);
        
		if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/GLSL150", type, sec);
    }
    else if (Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("hlsl"))
    {
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/HLSL", type, sec);
    }

    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/Cg", type, sec);
}

bool BaseApplication::initializeShaderGenerator(Ogre::SceneManager* sceneMgr)
{	
	if (Ogre::RTShader::ShaderGenerator::initialize())
	{
		m_pShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

		// Set the scene manager.
		m_pShaderGenerator->addSceneManager(sceneMgr);

		// Setup core libraries and shader cache path.
		Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList("Popular");
		Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
		Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
		
		Ogre::String shaderCoreLibsPath;
		Ogre::String shaderCachePath;

		// Default cache path is current directory;
		shaderCachePath = "./";

		// Try to find the location of the core shader lib functions and use it
		// as shader cache path as well - this will reduce the number of generated files
		// when running from different directories.
		for (; it != itEnd; ++it)
		{
			if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
			{
				shaderCoreLibsPath = (*it)->archive->getName() + "/";
				shaderCachePath    = shaderCoreLibsPath;
				break;
			}
		}

		// Core shader libs not found -> shader generating will fail.
		if (shaderCoreLibsPath.empty())			
			return false;			

		// Add resource location for the core shader libs. 
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(shaderCoreLibsPath , "FileSystem");
			
		// Set shader cache path.
		m_pShaderGenerator->setShaderCachePath(shaderCachePath);		
								
		// Create and register the material manager listener.
		m_pMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(m_pShaderGenerator);				
		Ogre::MaterialManager::getSingleton().addListener(m_pMaterialMgrListener);
	}

	return true;
}

void BaseApplication::createResourceListener(void)
{

}

void BaseApplication::loadResources(void)
{	
	// Initialize shader generator.
	// Must be before resource loading in order to allow parsing extended material attributes.
	initializeShaderGenerator(m_pSceneMgr);

	// Resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// TODO: Solve why we need to do this
	if (Ogre::Font* font = (Ogre::Font*)Ogre::FontManager::getSingleton().getByName("SdkTrays/Value").getPointer())
		font->load();
}

void BaseApplication::go(void)
{
#ifdef _DEBUG
    m_sResourcesCfg = "resources_d.cfg";
    m_sPluginsCfg = "plugins_d.cfg";
#else
    m_sResourcesCfg = "resources.cfg";
    m_sPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;

    m_pRoot->startRendering();

    // clean up
    destroyScene();
	destroyRTShaderSystem();
}

bool BaseApplication::setup(void)
{
    m_pRoot = new Ogre::Root(m_sPluginsCfg);

	// Initalizes Window
    if (!configure()) 
		return false;

    chooseSceneManager();

	// Script parsing only works after a render window has been created.
	// It is not safe to initialize resources before creating the RenderWindow.
    locateResources();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Load resources
    loadResources();

    // Create any resource listeners (for loading screens)
	createResourceListener();

	createCamera();
	createViewports();
	createFrameListener();

    // Create the scene
    createScene();
    
	return true;
};

bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if (m_bShutDown || m_pWindow->isClosed())
        return false;

	if (m_bReloadInterface)
	{
		delete m_pInterface;
		m_pInterface = new Interface(this);

		m_bReloadInterface = false;
	}

	m_pWorld->frameRenderingQueued(evt);

	// Note that keyboard/mouse capture(); below will call keyPressed accordingly, 
	//	so this code being above is intended
	if (clock() >= m_input.nextFire)
	{
		m_input.nextFire = clock() + 50;
		m_input.active = true;
	}

    // Need to capture/update each device
    m_pKeyboard->capture();
	m_pMouse->capture();

	bool bResult = true;

	// if dialog isn't up, then update the camera
	if (bResult)
		bResult = m_pCameraMan->frameRenderingQueued(evt);

	// interface
	if (bResult)
		bResult = m_pInterface->frameRenderingQueued(evt);

	m_input.active = false;
    return true;
}

bool BaseApplication::keyPressed(const OIS::KeyEvent &arg)
{
	// Acts as a middle ground between buffered & unbuffered input so we can 
	//	emulate auto key pressing every X ms when the user holds a key down
	m_input.key = arg.key;
	m_input.character = arg.text;
	m_input.nextFire = clock() + 500;
	m_input.active = true;

	// Interface returns false when an interface objects wants to stop all other input processing
	if (m_pInterface->keyPressed(arg) == false)
		return true;

	if (arg.key == OIS::KC_ESCAPE)
	{
		m_bShutDown = true;
	}

    m_pCameraMan->injectKeyDown(arg);
    return true;
}

bool BaseApplication::keyReleased(const OIS::KeyEvent &arg)
{
	// Reset AutoRepeatInput on key release
	if (arg.key == arg.key)
		m_input = AutoRepeatInput();

    m_pCameraMan->injectKeyUp(arg);
    return true;
}

bool BaseApplication::mouseMoved(const OIS::MouseEvent &arg)
{
	if (m_pInterface->mouseMoved(arg) == false)
		return true;

	m_pCameraMan->injectMouseMove(arg);
    return true;
}

bool BaseApplication::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (m_pInterface->mousePressed(arg, id) == false)
		return true;

    m_pCameraMan->injectMouseDown(arg, id);
    return true;
}

bool BaseApplication::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (m_pInterface->mouseReleased(arg, id) == false)
		return true;

    m_pCameraMan->injectMouseUp(arg, id);
    return true;
}

void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
	// Adjust mouse clipping area

    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = m_pMouse->getMouseState();
    ms.width = width;
    ms.height = height;

	if (m_pInterface)
		reloadInterface();
}

void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
	// Unattach OIS before window shutdown (very important under Linux)
    // Only close for window that created OIS (the main window in these demos)
    if (rw == m_pWindow)
    {
        if (m_pInputManager)
        {
			m_pInputManager->destroyInputObject(m_pMouse);
			m_pInputManager->destroyInputObject(m_pKeyboard);

            OIS::InputManager::destroyInputSystem(m_pInputManager);
            m_pInputManager = 0;
        }
    }
}
