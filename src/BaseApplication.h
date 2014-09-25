#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include "stdafx.h"
#include "ShaderGenerator.h"

class World;
class Interface;

struct AutoRepeatInput
{
	AutoRepeatInput() : key(0), character(0), nextFire(0), active(false) {}

	int key;
	int character;
	int nextFire;
	bool active; 
};

class BaseApplication : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener, public OgreBites::SdkTrayListener
{
	public:
	    BaseApplication(void);
	    virtual ~BaseApplication(void);
	
		virtual void go(void);

		void shutdown() { m_bShutDown = true; }
		void reloadInterface() { m_bReloadInterface = true; }

		int autoInputGetKey() const { return m_input.active ? m_input.key : NULL; }
		int autoInputGetChar() const { return m_input.active ? m_input.character : NULL; }

		World* getWorld() const { return m_pWorld; }
		Interface* getInterface() const { return m_pInterface; }

	    Ogre::Root* m_pRoot;
	    Ogre::Camera* m_pCamera;
	    Ogre::SceneManager* m_pSceneMgr;
	    Ogre::RenderWindow* m_pWindow;
	    Ogre::String m_sResourcesCfg;
	    Ogre::String m_sPluginsCfg;
	
	    // OgreBites
	    OgreBites::SdkCameraMan* m_pCameraMan;     // basic camera controller
	
	    // OIS Input devices
	    OIS::InputManager* m_pInputManager;

		// RTSHADER_SYSTEM
		Ogre::RTShader::ShaderGenerator* m_pShaderGenerator;				// The Shader generator instance.
		ShaderGeneratorTechniqueResolverListener* m_pMaterialMgrListener;	// Shader generator material manager listener.
		
		// OIS
		OIS::Mouse* m_pMouse;
		OIS::Keyboard* m_pKeyboard;

	protected:
	    virtual bool setup();
	    virtual bool configure(void);
		virtual bool initializeShaderGenerator(Ogre::SceneManager* sceneMgr);

	    virtual void chooseSceneManager(void);
	    virtual void createCamera(void);
	    virtual void createFrameListener(void);
	    virtual void createScene(void) = 0; // Override me!
	    virtual void destroyScene(void);
	    virtual void createViewports(void);
	    virtual void locateResources(void);
	    virtual void createResourceListener(void);
	    virtual void loadResources(void);
		virtual void destroyRTShaderSystem();		

	    // Ogre::FrameListener
	    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	
	    // OIS::KeyListener
	    virtual bool keyPressed(const OIS::KeyEvent &arg);
		virtual bool keyReleased(const OIS::KeyEvent &arg);
		virtual bool processUnbufferedInput(const Ogre::FrameEvent& evt) { printf("event\n"); return true; }

	    // OIS::MouseListener
	    virtual bool mouseMoved( const OIS::MouseEvent &arg );
	    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	
	    // Ogre::WindowEventListener
	    // Adjust mouse clipping area
	    virtual void windowResized(Ogre::RenderWindow* rw);

	    // Unattach OIS before window shutdown (very important under Linux)
	    virtual void windowClosed(Ogre::RenderWindow* rw);
				
	    // OgreBites
	    bool m_bCursorWasVisible;                  // was cursor visible before dialog appeared
		
		Interface* m_pInterface;
		World* m_pWorld;

	private:
		bool m_bReloadInterface;
		bool m_bShutDown;

		AutoRepeatInput m_input;
};

#endif
