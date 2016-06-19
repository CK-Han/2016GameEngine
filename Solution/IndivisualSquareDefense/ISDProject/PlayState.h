#pragma once

#include "GameState.h"

			//�̵���, ���� �� �ֺ����, ���� attack���� ���, ��ǥ�� ���� �ִ� ���
enum OBJ_STATE { move, alert, move_with_alert, have_target };

using namespace std;

class ObjectData	//��ü�� Type�� ���������� ���̴� �κ�, Enemy�� ���������� ���´�(HP������)
{
public:
	unsigned int		mHP;		//for Enemy
	unsigned int		mDamage;
	unsigned char		mRange;
	float				mAttackSpeed;
	float				mMoveSpeed;
};

class AnimateObject
{
public:
	bool		mIsDeath;		//for Enemy
	bool		mIsRegen;		//for Enemy

	Ogre::Entity* mObjectEntity;
	Ogre::AnimationState* mCurrentAnimationState;
	Ogre::AnimationState* mMoveState;
	Ogre::AnimationState* mIdleState;
	Ogre::AnimationState* mAttackState;
	Ogre::AnimationState* mDieState;

	Vector3 mObjectDirection;
	Vector3 mObjectDestination;
	
	int mObjectState;
	
	AnimateObject* mObjectTargetObject;

	ObjectData*		mObjectData;
};




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

  //���� ������ �Լ���
  bool createObject(int unitType);
  
  void stageStart();
  void stageUpdate(float elapsedTime);
  void stageEnd(bool bStageClear);

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

  Ogre::Entity*			mSelectedEntity;

  Ogre::Vector3			mCameraMoveVector;
  
  ///////////////////////////////////
  //�������� ����
  bool		mIsDefeat{ false };

  //1 �»��(����), 2 ���ϴ� 3 ���ϴ� 4 ����
  Ogre::Vector3			mEnemyMovePosition1{ Ogre::Vector3(-13.75f, 0.25f, -13.75f) };
  Ogre::Vector3			mEnemyMovePosition2{ Ogre::Vector3(-13.75f, 0.25f, +13.75f) };
  Ogre::Vector3			mEnemyMovePosition3{ Ogre::Vector3(+13.75f, 0.25f, +13.75f) };
  Ogre::Vector3			mEnemyMovePosition4{ Ogre::Vector3(+13.75f, 0.25f, -13.75f) };

  std::list<AnimateObject*>::iterator mEnemyListIter;
  
  int		 mStageNumber{ 0 };
  float		 mNextStageStartTime{ 10.0f };
  bool		 mStageStarted{ false };
  float		 mStageElapsedTime{ 0.0f };
  

  ///////////////////////////////////
  //��ü���� ����
  int mID{ 0 };	//Entity ����, ���п�
  std::list<AnimateObject*> mMyObjects;
  std::map<Ogre::Entity*, AnimateObject*> mMapMyObjects;
  ObjectData mMarineData;
  ObjectData mReaperData;
  ObjectData mMarauderData;

  std::list<AnimateObject*> mEnemyObjects;
  std::map<Ogre::Entity*, AnimateObject*> mMapEnemyObjects;

  Ogre::Entity* mBarrackEntity;
  Ogre::Entity* mEngineeringbayEntity;
};


