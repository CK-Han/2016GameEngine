#pragma once

#include "GameState.h"

			//이동중, 정지 및 주변경계, 땅에 attack찍은 경우, 목표인 적이 있는 경우
enum OBJ_STATE { move, alert, move_with_alert, have_target };

class PlayState : public GameState
{
public:
  void enter(void);
  void exit(void);

  void pause(void);
  void resume(void);

  bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
  bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

  bool mouseMoved(GameManager* game, const OIS::MouseEvent &e) ;
  bool mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id );
  bool mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
  bool keyReleased(GameManager* game, const OIS::KeyEvent &e);

  static PlayState* getInstance() { return &mPlayState; }



private:

  void _setLights(void);
  void _drawGroundPlane(void);
  void _drawGridPlane(void);


  static PlayState mPlayState;

  Ogre::Root *mRoot;
  Ogre::RenderWindow* mWindow;
  Ogre::SceneManager* mSceneMgr;
  Ogre::Camera* mCamera;

  Ogre::Light *mLightP, *mLightD, *mLightS;

  Ogre::Overlay*           mInformationOverlay;

  Ogre::Entity*	mSelectedEntity;

  Ogre::Vector3 mCameraMoveVector;
  


  //객체별로
  Entity* mTempEnemyEntity;

  Ogre::Entity* mObjectEntity;
  Ogre::AnimationState* mCurrentAnimationState;
  Ogre::AnimationState* mMoveState;
  Ogre::AnimationState* mIdleState;
  Ogre::AnimationState* mAttackState;
  Ogre::AnimationState* mDieState;
  Real mObjectSpeed;
  Vector3 mObjectDirection;
  Vector3 mObjectDestination;
  int mObjectState;
  float mObjectRange;

  Entity* mObjectTargetEntity;
  AnimationState* mTempDieState;
  
  

};


