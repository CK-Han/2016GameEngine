#pragma once

#include <vector>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#define DEBUG_PRINTF(format, ...)     {fprintf (stderr, format, ##__VA_ARGS__); TCHAR __debugStr[1000]; _stprintf(__debugStr, _T(format), ##__VA_ARGS__); OutputDebugString(__debugStr);}

#endif


#include <Ogre.h>
#include <Overlay/OgreOverlay.h>
#include <Overlay/OgreOverlaySystem.h>
#include <Overlay/OgreOverlayManager.h>
#include <Overlay/OgreOverlayElement.h>
#include <Overlay/OgreTextAreaOverlayElement.h>
#include <Overlay/OgreFontManager.h>
#include <OIS/OIS.h>

#include <mmsystem.h>
#include <Digitalv.h>
#pragma comment(lib,"winmm.lib")

#include <conio.h> //for debug
#include <thread>
#include <list>
#include <map>
#include "stdafx.h"

using namespace std;
using namespace Ogre;

class GameState;

class GameManager :  
  public Ogre::FrameListener,
  public OIS::KeyListener, 
  public OIS::MouseListener
{
public:
  GameManager();
  ~GameManager();

  void init(void);
  void changeState(GameState* state);
  void pushState(GameState* state);
  void popState();

  void go(void);

  bool mouseMoved( const OIS::MouseEvent &e );
  bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
  bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );

  bool keyPressed( const OIS::KeyEvent &e );
  bool keyReleased( const OIS::KeyEvent &e );		

  //정의 코드
  int	mGameClearState{ 0 };	//0 게임 전 1 패배 2 클리어
  float mGameClearTime{ 0.0f };
  int mGameClearMarine{ 0 };
  int mGameClearReaper{ 0 };
  int mGameClearMarauder{ 0 };
  int mGameClearDamageUp{ 0 };
  int mGameClearSpeedUp{ 0 };

  void setGameResult(int marine, int reaper, int marauder, int dam, int spd, float time)
  {
	  mGameClearMarine = marine;
	  mGameClearReaper = reaper;
	  mGameClearMarauder = marauder;
	  mGameClearDamageUp = dam;
	  mGameClearSpeedUp = spd;
	  mGameClearTime = time;
  }


protected:
  Ogre::Root* mRoot;
  Ogre::RenderWindow* mWindow;

  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;
  OIS::InputManager *mInputManager;

  bool frameStarted(const Ogre::FrameEvent& evt);
  bool frameEnded(const Ogre::FrameEvent& evt);

private:
  std::vector<GameState*> states;

  Ogre::SceneManager* mSceneMgr;
  Ogre::Camera* mCamera;
  Ogre::Viewport* mViewport;
};

/*
//전역함수
std::wstring string2wstring(std::string str) 
{
	std::wstring wstr(str.length(), L' ');
	copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}
*/