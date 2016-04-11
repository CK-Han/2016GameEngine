#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>

using namespace Ogre;

Camera* g_Camera;
//float g_rotateSeed;
#define ANGLE	3.141592 / 180.0f

class ESCListener : public FrameListener {
  OIS::Keyboard *mKeyboard;

public:
  ESCListener(OIS::Keyboard *keyboard) : mKeyboard(keyboard) {}
  bool frameStarted(const FrameEvent &evt)
  {
	  /*
	  //ī�޶� ���� ����
	  g_Camera->setPosition(cos(g_rotateSeed * ANGLE * 0.05f) * 300.0f, 1000.0f, sin(g_rotateSeed * ANGLE * 0.05f) * 300.0f);
	  g_Camera->lookAt(0.0f, 100.0f, 0.0f);
	  g_rotateSeed++;
	  */

	  static float deg = 90.0f;
	  g_Camera->setPosition(800.0f * cos(deg / 360.0f * 2.0f * 3.14f), 100.0f, 800.0f * sin(deg / 360.0f * 2.0f * 3.14f));
	  deg = (deg > 360.0f) ? 0.0f : deg + 0.05f;
	  g_Camera->lookAt(0.0f, 100.0f, 0.0f);

    mKeyboard->capture();
    return !mKeyboard->isKeyDown(OIS::KC_ESCAPE);
  }
};

class LectureApp {

  Root* mRoot;
  RenderWindow* mWindow;
  SceneManager* mSceneMgr;
  Camera* mCamera;
  Viewport* mViewport;
  OIS::Keyboard* mKeyboard;
  OIS::InputManager *mInputManager;
  ESCListener* mESCListener;

public:

  LectureApp() {}

  ~LectureApp() {}

  void go(void)
  {
    // OGRE�� ���� ��Ʈ ������Ʈ ����
#if !defined(_DEBUG)
    mRoot = new Root("plugins.cfg", "ogre.cfg", "ogre.log");
#else
    mRoot = new Root("plugins_d.cfg", "ogre.cfg", "ogre.log");
#endif


    // �ʱ� ������ ���ǱԷ��̼� ���� - ogre.cfg �̿�
    if (!mRoot->restoreConfig()) {
      if (!mRoot->showConfigDialog()) return;
    }
    mWindow = mRoot->initialise(true, "Professor & Ninja");

    // ESC key�� ������ ���, ����� ���� ������ ������ Ż���� ó��
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;
    OIS::ParamList pl;
    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
    mInputManager = OIS::InputManager::createInputSystem(pl);
    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, false));

    mESCListener = new ESCListener(mKeyboard);
    mRoot->addFrameListener(mESCListener);

    mSceneMgr = mRoot->createSceneManager(ST_GENERIC);

    mCamera = mSceneMgr->createCamera("camera");

    mCamera->setPosition(0.0f, 100.0f, 500.0f);
    mCamera->lookAt(0.0f, 100.0f, 0.0f);

    //mCamera->setPosition(0.0f, 100.0f, -500.0f);
    //mCamera->lookAt(0.0f, 100.0f, 0.0f);	

    mCamera->setNearClipDistance(5.0f);
    mCamera->setNearClipDistance(5.0f);

    mViewport = mWindow->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0f,0.0f,0.5f));
//    mViewport->setBackgroundColour(ColourValue(1.0f,1.0f,1.0f));
    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));
	
	

    ResourceGroupManager::getSingleton().addResourceLocation("resource.zip", "Zip");
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

    // ��ǥ�� ǥ��
    Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
    mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode",Ogre::Vector3(0,0,0))->attachObject(mAxesEntity);
    mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

    _drawGridPlane();


    // fill here --------------------------------------------------------------------------------------------------
	/*
	Entity* entity1 = mSceneMgr->createEntity("Professor", "DustinBody.mesh");
	Entity* entity2 = mSceneMgr->createEntity("Ninja", "ninja.mesh");

	SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor01", Vector3(-100.0f, 0.0f, 0.0f));
	node1->attachObject(entity1);

	SceneNode* node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("Professor02", Vector3(+100.0f, 0.0f, 0.0f));
	node2->attachObject(entity2);
	*/

	Entity* entity[12];
	SceneNode* node[12];

	char str[20];
	
	float rad = 360.0f / 12.0f;

	for (int i = 0; i < 12; ++i)
	{
		sprintf(str, "Professor%d", i);
		entity[i] = mSceneMgr->createEntity(str, "DustinBody.mesh");
		node[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(str);
		
		node[i]->translate(Vector3(cos(i * rad * ANGLE) * 200.0f, 0.0f, sin(i * rad * ANGLE) * 200.0f));
		node[i]->attachObject(entity[i]);
	}

	g_Camera = mCamera;
	// ------------------------------------------------------------------------------------------------------------

    mRoot->startRendering();

    mInputManager->destroyInputObject(mKeyboard);
    OIS::InputManager::destroyInputSystem(mInputManager);

    delete mRoot;
  }

private:
  void _drawGridPlane(void)
  {
    Ogre::ManualObject* gridPlane =  mSceneMgr->createManualObject("GridPlane"); 
    Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode"); 

    Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial","General"); 
    gridPlaneMaterial->setReceiveShadows(false); 
    gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1,1,1,0); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1,1,1); 
    gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,1); 

    gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST); 
    for(int i=0; i<21; i++)
    {
      gridPlane->position(-500, 0, 500-i*50);
      gridPlane->position(500, 0, 500-i*50);

      gridPlane->position(-500+i*50, 0, 500);
      gridPlane->position(-500+i*50, 0, -500);
    }

    gridPlane->end(); 

    gridPlaneNode->attachObject(gridPlane);
  }
};


#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
  int main(int argc, char *argv[])
#endif
  {
    LectureApp app;

    try {

      app.go();

    } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: " <<
        e.getFullDescription().c_str() << std::endl;
#endif
    }

    return 0;
  }

#ifdef __cplusplus
}
#endif

