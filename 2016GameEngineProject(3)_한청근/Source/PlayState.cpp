#include "PlayState.h"
#include "TitleState.h"

using namespace Ogre;



PlayState PlayState::mPlayState;

void PlayState::enter(void)
{
	//�⺻ Play Scene ���� �ʱ�ȭ
	mRoot = Root::getSingletonPtr();
	mRoot->getAutoCreatedWindow()->resetStatistics();

	mSceneMgr = mRoot->getSceneManager("main");
	mCamera = mSceneMgr->getCamera("main");

	mCameraMoveVector = Vector3::ZERO;
	mSelectedEntity = nullptr;

	//�⺻ �� ����
	mMarineNum = 0;
	mReaperNum = 0;
	mMarauderNum = 0;
	mDamageNum = 0;
	mSpeedNum = 0;

	mCountDownStart = false;

	MCI_OPEN_PARMS mciOpen;
	MCI_PLAY_PARMS m_mciPlayParms;
	mciOpen.lpstrElementName = TEXT("Sounds/bgm.mp3"); // ���� ��� �Է�
	mciOpen.lpstrDeviceType = TEXT("mpegvideo");

	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
		(DWORD)(LPVOID)&mciOpen);
	mDeviceID_BGM = mciOpen.wDeviceID;
	mciSendCommand(mDeviceID_BGM, MCI_PLAY, MCI_DGV_PLAY_REPEAT, // play & repeat
		(DWORD)(LPVOID)&m_mciPlayParms);

	
	mStartTime = timeGetTime();
	mStageNumber =  0 ;
	mNextStageStartTime =  15.0f ;
	mIsGameWin = false;
	mIsDefeat = false;
	mStageStarted = false ;
	mStageElapsedTime = 0.0f ;

	mMineral = 300;

	//������Ʈ Data�� �⺻ ����
	mMarineData.mAttackSpeed = 1.4f;
	mMarineData.mDamage = 5;
	mMarineData.mRange = 5;
	mMarineData.mMoveSpeed = 4.0f;

	mReaperData.mAttackSpeed = 2.0f;
	mReaperData.mDamage = 7;
	mReaperData.mRange = 5;
	mReaperData.mMoveSpeed = 6.0f;

	mMarauderData.mAttackSpeed = 1.0f;
	mMarauderData.mDamage = 20;
	mMarauderData.mRange = 7;
	mMarauderData.mMoveSpeed = 4.0f;

	//Enemy ������Ʈ �ʱ�ȭ
	AnimateObject* pObject = nullptr;
	ObjectData* pObjectData = nullptr;
	char nameBuf[20];
	for (int i = 0; i < 25; ++i)
	{
		pObject = new AnimateObject();
		pObject->mIsDeath = true;
		sprintf(nameBuf, "enemy%d", i);
		pObject->mObjectEntity = mSceneMgr->createEntity(nameBuf, "DustinBody.mesh");
		SceneNode* pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(nameBuf, Vector3(500.0f, 0.25f, -500.0f ));
		pNode->attachObject(pObject->mObjectEntity);
		pNode->setScale(Vector3(0.012f, 0.012f, 0.012f));
		
		//�ִϸ��̼��� Move�� Die�� �ʿ��ϴ�. Die�� �����Ƿ� Throw�� ��ü�Ѵ�.
		pObject->mIdleState = nullptr;
		pObject->mAttackState = nullptr;
		pObject->mMoveState = pObject->mObjectEntity->getAnimationState("Run");
		pObject->mDieState = pObject->mObjectEntity->getAnimationState("Throw");
		pObject->mMoveState->setLoop(true);
		pObject->mDieState->setLoop(false);
		pObject->mCurrentAnimationState = pObject->mMoveState;
		pObject->mCurrentAnimationState->setEnabled(true);

		pObject->mObjectDirection = Vector3::ZERO;
		pObject->mObjectDestination = Vector3::ZERO;	//�ǹ��� ���������̹Ƿ� ZERO�� ���������� �ξ ������.
		pObject->mObjectState = OBJ_STATE::move;
		pObject->mObjectTargetObject = nullptr;
		
		pObjectData = new ObjectData();
		pObjectData->mHP = 30;
		pObjectData->mMoveSpeed = 5.0f;
		pObject->mObjectData = pObjectData;

		mEnemyObjects.push_back(pObject);
		mMapEnemyObjects.insert(make_pair(pObject->mObjectEntity, pObject));
	}

	//�ǹ� ����
	mBarrackEntity = mSceneMgr->createEntity("barrack", "barrack.mesh");
	SceneNode* barrackNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("barrack", Vector3(-3.0f, 0.0f, 0.0f));
	barrackNode->attachObject(mBarrackEntity);

	mEngineeringbayEntity = mSceneMgr->createEntity("engineeringbay", "engineeringbay.mesh");
	SceneNode* engineeringbayNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("engineeringbay", Vector3(+3.0f, 0.0f, -0.0f));
	engineeringbayNode->attachObject(mEngineeringbayEntity);
	
	//���� �� UI ������
	//_drawGridPlane();
	_setLights();
	_drawGroundPlane();
	
	//PlayState�� Overlay ����
	mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	//mInformationOverlay = OverlayManager::getSingletonPtr()->create("Overlay/Information");
	mInformationOverlay->show();
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
	
	for (auto d : mMyObjects)
	{
		delete d;
	}
	mMyObjects.clear();

	for (auto d : mEnemyObjects)
	{
		delete d->mObjectData;
		delete d;
	}
	mEnemyObjects.clear();

	mInformationOverlay->hide();
	// ---------------------------------------
}

void PlayState::pause(void)
{
}

void PlayState::resume(void)
{

}

bool PlayState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	if (true == mIsGameWin)
	{
		mIsGameWin = false;
		mCountDownStart = false;
		mciSendCommandW(mDeviceID_BGM, MCI_CLOSE, 0, NULL);
		game->mGameClearState = 2;
		game->setGameResult(mMarineNum, mReaperNum, mMarauderNum, mDamageNum, mSpeedNum, mStartTime);
		PlaySound(TEXT("Sounds/Win.wav"), 0, SND_ASYNC);
		game->changeState(TitleState::getInstance());	//�¸�ó��
		return true;
	}

	if (true == mIsDefeat) {
		mIsDefeat = false;
		mCountDownStart = false;
		mciSendCommandW(mDeviceID_BGM, MCI_CLOSE, 0, NULL);
		game->mGameClearState = 1;
		game->setGameResult(mMarineNum, mReaperNum, mMarauderNum, mDamageNum, mSpeedNum, 0.0f);
		PlaySound(TEXT("Sounds/Defeat.wav"), 0, SND_ASYNC);
		game->changeState(TitleState::getInstance());	//�й�ó��
		return true;
	}
	//wsad�� ���� ī�޶� �̵�
	mCamera->moveRelative(mCameraMoveVector * 20.0f * evt.timeSinceLastFrame);
	
	//�������� ���� ó��
	mNextStageStartTime -= evt.timeSinceLastFrame;

	if ( (mNextStageStartTime <= 5.0f) && !mCountDownStart)
	{
		mCountDownStart = true;
		PlaySound(TEXT("Sounds/CountDown.wav"), 0, SND_ASYNC);
		/*
		MCI_OPEN_PARMS mciOpen1;
		MCI_PLAY_PARMS m_mciPlayParms;
		mciOpen1.lpstrElementName = "Sounds/Countdown.mp3"; // ���� ��� �Է�
		mciOpen1.lpstrDeviceType = "mpegvideo";

		mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
			(DWORD)(LPVOID)&mciOpen1);
		mDeviceID_COUNT = mciOpen1.wDeviceID;
		//ó�� ��ġ��
		mciSendCommand(mDeviceID_COUNT, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL);
		mciSendCommand(mDeviceID_COUNT, MCI_PLAY, 0, (DWORD)(LPVOID)&m_mciPlayParms);
		*/
	}
	
	if (mNextStageStartTime <= 0.0f)
		stageStart();
	
	//Enemy ������Ʈ���� Update
	if (true == mStageStarted)
		stageUpdate(evt.timeSinceLastFrame);

	

	//�� ������Ʈ���� Update
	for (auto obj : mMyObjects)
	{
		//��ü ����
		switch (obj->mObjectState)
		{
		case OBJ_STATE::move:
		{	//������ ���콺�� ������ ����, �̵� ��û�� ���
			if (Vector3::ZERO != obj->mObjectDirection)
			{
				Real move = obj->mObjectData->mMoveSpeed * evt.timeSinceLastFrame;
				obj->mObjectEntity->getParentNode()->translate(obj->mObjectDirection * move);

				if (obj->mObjectEntity->getParentNode()->getPosition().distance(obj->mObjectDestination) <= (move + 0.2f))
				{ // ��ǥ ������ ������ ���
					obj->mObjectEntity->getParentNode()->setPosition(obj->mObjectDestination); // ��ǥ ������ ĳ���͸� ��ġ
					obj->mObjectDirection = Vector3::ZERO;

					obj->mCurrentAnimationState->setEnabled(false);
					obj->mCurrentAnimationState = obj->mIdleState;
					obj->mCurrentAnimationState->setEnabled(true);

					obj->mObjectState = OBJ_STATE::alert;
				}
			}

			break;
		}
		
		case OBJ_STATE::alert:
		{	//�����Ÿ� �� ���� �ִ��� �Ǵ��Ͽ�, �ִ� ��� �����Ѵ�
			Vector3 myPos = obj->mObjectEntity->getParentNode()->getPosition();
			Real nearDistance = FLT_MAX;
			AnimateObject* nearEnemy = nullptr;

			for(auto enemy : mEnemyObjects)
			{
				if (enemy->mIsDeath == true) continue;
				if (enemy->mIsRegen == false) continue;

				Vector3 otherPos = enemy->mObjectEntity->getParentNode()->getPosition();
				Real distance = myPos.distance(otherPos); //y�� ������ �����ǹǷ� �����ص� �ȴ�.
				if (distance < nearDistance)
				{
					nearDistance = distance;
					nearEnemy = enemy;
				}
			}

			if (nearDistance <= obj->mObjectData->mRange)
			{
				Quaternion srcQuat = obj->mObjectEntity->getParentNode()->getOrientation();
				Vector3 direction = (nearEnemy->mObjectEntity->getParentNode()->getPosition() - 
								obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
				Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(direction);
				Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
				obj->mObjectEntity->getParentNode()->setOrientation(delta);

				obj->mCurrentAnimationState->setEnabled(false);
				obj->mCurrentAnimationState = obj->mAttackState;
				obj->mCurrentAnimationState->setEnabled(true);

				obj->mObjectTargetObject = nearEnemy;
				obj->mObjectState = OBJ_STATE::have_target;
			}

			break;
		}
		
		case OBJ_STATE::have_target:
		{	//Ÿ���� ������ �� �����Ÿ� ���� �ִ��� Ȯ���Ѵ�.
			if (false == obj->mObjectTargetObject->mIsDeath)
			{
				Vector3 myPos = obj->mObjectEntity->getParentNode()->getPosition();
				Vector3 otherPos = obj->mObjectTargetObject->mObjectEntity->getParentNode()->getPosition();
				Real distance = myPos.distance(otherPos); //y�� ������ �����ǹǷ� �����ص� �ȴ�.
				if (distance <= obj->mObjectData->mRange)
				{	//���� �����Ÿ� �� ��ȿ�� ���� �ִٸ� ��� ������ ����; ���⸸ ����شޱ�
					Quaternion srcQuat = obj->mObjectEntity->getParentNode()->getOrientation();
					Vector3 direction = (obj->mObjectTargetObject->mObjectEntity->getParentNode()->getPosition() - 
								obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
					Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(direction);
					Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
					obj->mObjectEntity->getParentNode()->setOrientation(delta);
				}
				else
				{	//���� ������ �����ٸ� �ٽ� ��� ���·� ����.
					obj->mCurrentAnimationState->setEnabled(false);
					obj->mCurrentAnimationState = obj->mIdleState;
					obj->mCurrentAnimationState->setEnabled(true);

					obj->mObjectTargetObject = nullptr;
					obj->mObjectState = OBJ_STATE::alert;
				}		
			}
			else //Ÿ���� �׾��ٸ� ���ο� Ÿ���� �����ؾ��Ѵ�. �װ� �����¿��� �ϴϱ� �װɷ� �ѱ���
			{
				obj->mCurrentAnimationState->setEnabled(false);
				obj->mCurrentAnimationState = obj->mIdleState;
				obj->mCurrentAnimationState->setEnabled(true);

				obj->mObjectTargetObject = nullptr;
				obj->mObjectState = OBJ_STATE::alert;
			}

			break;
		}
		} //end switch

		//�ִϸ��̼� 50 ������ �� 1.67��
		if (obj->mCurrentAnimationState == obj->mAttackState)
		{
			if (obj->mCurrentAnimationState->hasEnded())
			{
				obj->mCurrentAnimationState->setTimePosition(0.0f);
				if(nullptr != obj->mObjectTargetObject)
					attackEnemy(obj, obj->mObjectTargetObject);
			}
			obj->mCurrentAnimationState->addTime(evt.timeSinceLastFrame * obj->mObjectData->mAttackSpeed);
		}
		else
			obj->mCurrentAnimationState->addTime(evt.timeSinceLastFrame);

		
	}	//end mMyObjects loop;


	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	static Ogre::DisplayString currFps = "now FPS: ";
	static Ogre::DisplayString avgFps = "avg FPS: ";
	static Ogre::DisplayString bestFps = "max FPS: ";
	static Ogre::DisplayString worstFps = "min FPS: ";


	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();
	
	guiAvg->setCaption(avgFps + to_string(stats.avgFPS));
	guiCurr->setCaption(currFps + to_string(stats.lastFPS));
	guiBest->setCaption(bestFps + to_string(stats.bestFPS));
	guiWorst->setCaption(worstFps + to_string(stats.worstFPS));

	
	//�� ���� ��������
	char buf1[60], buf2[30];
	OverlayElement* guiStageMonster = OverlayManager::getSingleton().getOverlayElement("StageMonster");
	sprintf(buf1, "Stage : %d   ", mStageNumber);
	sprintf(buf2, "MonsterHP : %d", mStageNumber * 20);
	strcat(buf1, buf2);
	guiStageMonster->setCaption(buf1);

	OverlayElement* guiGameTimer = OverlayManager::getSingleton().getOverlayElement("GameTime");
	if (false == mStageStarted)
		sprintf(buf1, "Waiting : %f  ", mNextStageStartTime);
	else
		sprintf(buf1, "Limit Time : %f  ", STAGE_TIME - mStageElapsedTime);
	guiGameTimer->setCaption(buf1);

	OverlayElement* guiMineral = OverlayManager::getSingleton().getOverlayElement("Mineral");
	sprintf(buf1, "Mineral : %d", mMineral);
	guiMineral->setCaption(buf1);

	OverlayElement* guiMarineData = OverlayManager::getSingleton().getOverlayElement("MarineData");
	sprintf(buf1, "Marine(1) 50 cost - \tDam : %d  Speed : %f Range : %d", mMarineData.mDamage, mMarineData.mAttackSpeed, mMarineData.mRange);
	guiMarineData->setCaption(buf1);

	OverlayElement* guiReaperData = OverlayManager::getSingleton().getOverlayElement("ReaperData");
	sprintf(buf1, "Reaper(2) 100 cost - \tDam : %d  Speed : %f Range : %d", mReaperData.mDamage, mReaperData.mAttackSpeed, mReaperData.mRange);
	guiReaperData->setCaption(buf1);

	OverlayElement* guiMarauderData = OverlayManager::getSingleton().getOverlayElement("MarauderData");
	sprintf(buf1, "Marauder(3) 150 cost - \tDam : %d  Speed : %f Range : %d", mMarauderData.mDamage, mMarauderData.mAttackSpeed, mMarauderData.mRange);
	guiMarauderData->setCaption(buf1);

	return true;
	
}


bool PlayState::keyReleased(GameManager* game, const OIS::KeyEvent &e)
{
	switch (e.key)
	{
	case OIS::KC_W: mCameraMoveVector.y -= 1; break;
	case OIS::KC_S: mCameraMoveVector.y += 1; break;
	case OIS::KC_A: mCameraMoveVector.x += 1; break;
	case OIS::KC_D: mCameraMoveVector.x -= 1; break;

	case OIS::KC_1:
	{
		createObject(OBJECT_TYPE_MARINE);
		break;
	}
	case OIS::KC_2:
	{
		createObject(OBJECT_TYPE_REAPER);
		break;
	}
	case OIS::KC_3:
	{
		createObject(OBJECT_TYPE_MARAUDER);
		break;
	}
	case OIS::KC_4:
	{
		upgradeUnit(UPGRADE_TYPE_DAMAGE);
		break;
	}
	case OIS::KC_5:
	{
		upgradeUnit(UPGRADE_TYPE_SPEED);
		break;
	}
	case OIS::KC_ADD:
	{
		mMineral += 500;
		break;
	}
	}
	return true;
}

bool PlayState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------
	switch (e.key)
	{
	case OIS::KC_W: mCameraMoveVector.y += 1; break;
	case OIS::KC_S: mCameraMoveVector.y -= 1; break;
	case OIS::KC_A: mCameraMoveVector.x -= 1; break;
	case OIS::KC_D: mCameraMoveVector.x += 1; break;
	case OIS::KC_ESCAPE:
		mciSendCommandW(mDeviceID_BGM, MCI_CLOSE, 0, NULL);
		game->mGameClearState = 0;
		game->changeState(TitleState::getInstance());
		break;
	}
	// -----------------------------------------------------

	return true;
}

bool PlayState::mousePressed(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	Ray mouseRay = mCamera->getCameraToViewportRay((float)e.state.X.abs / CLIENT_WIDTH,
		(float)e.state.Y.abs / CLIENT_HEIGHT);

	RaySceneQuery * rq = mRoot->getSceneManager("main")->createRayQuery(mouseRay);

	rq->setSortByDistance(true, 1);
	rq->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);

	RaySceneQueryResult res = rq->execute();
	RaySceneQueryResult::iterator it = res.begin();

	MovableObject* pSelectedEntity = NULL;
	Vector3 pickPosition;
	float SelectedEntityDist = 0.0f;
	bool successPicking = false;
	if (it != res.end())
	{
		pSelectedEntity = it->movable;
		SelectedEntityDist = it->distance;

		pickPosition = mouseRay.getPoint(it->distance);
		successPicking = true;
	}


	if (successPicking)
	{
		if (e.state.buttonDown(OIS::MB_Left))	//��ü ����
		{
			auto name = pSelectedEntity->getName();
			if ((name != "GroundPlaneInner") && (name != "GroundPlane"))
				mSelectedEntity = (Entity*)pSelectedEntity;
			else
				mSelectedEntity = nullptr;
		}

		if (e.state.buttonDown(OIS::MB_Right)) //��ü �̵�
		{			
			if ((nullptr != mSelectedEntity) && (((Entity*)pSelectedEntity)->getName() == "GroundPlaneInner"))
			{
				pickPosition.y = mSelectedEntity->getParentNode()->getPosition().y;
				AnimateObject* obj = nullptr;
				if(mMapMyObjects.end() != mMapMyObjects.find(mSelectedEntity))
					obj = mMapMyObjects.find(mSelectedEntity)->second;
				if (nullptr == obj) return true;
				obj->mObjectDestination = pickPosition;
				obj->mObjectDirection = (pickPosition - obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
				
				Quaternion srcQuat = obj->mObjectEntity->getParentNode()->getOrientation();
				Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(obj->mObjectDirection);
				Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
				obj->mObjectEntity->getParentNode()->setOrientation(delta);

				obj->mCurrentAnimationState->setEnabled(false);
				obj->mCurrentAnimationState = obj->mMoveState;
				obj->mCurrentAnimationState->setEnabled(true);

				obj->mObjectState = OBJ_STATE::move;
			}
		}
	}

	//�׽�Ʈ �����
#ifdef _DEBUG
	if (it != res.end())
	{
	pSelectedEntity = it->movable;
	SelectedEntityDist = it->distance;

	Vector3 temp = mouseRay.getPoint(it->distance);
	float xPos = temp.x;
	float yPos = temp.y;
	float zPos = temp.z;
	_cprintf("Pos : %f, %f, %f\n", xPos, yPos, zPos);
	_cprintf("clicked: %s\n", pSelectedEntity->getName().c_str());
	}
	else
	{
	_cprintf("cleared selection.\n");
	}
#endif

	mRoot->getSceneManager("main")->destroyQuery(rq);
	return true;
}

bool PlayState::mouseReleased(GameManager* game, const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}


bool PlayState::mouseMoved(GameManager* game, const OIS::MouseEvent &e)
{
	if (e.state.buttonDown(OIS::MB_Right))
	{

	}

	mCamera->moveRelative(Ogre::Vector3(0, 0, -e.state.Z.rel * 0.1f));
	return true;
}



void PlayState::_setLights(void)
{
	mSceneMgr->setAmbientLight(ColourValue(0.7f, 0.7f, 0.7f));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	mLightD = mSceneMgr->createLight("LightD");
	mLightD->setType(Light::LT_DIRECTIONAL);
	mLightD->setDirection(Vector3(1, -2.0f, -1));
	mLightD->setVisible(true);
}

void PlayState::_drawGroundPlane(void)
{
	MaterialPtr mat = MaterialManager::getSingleton().create("PlaneMat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	MaterialPtr matInner = MaterialManager::getSingleton().create("PlaneInnerMat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	TextureUnitState* t = mat->getTechnique(0)->getPass(0)->createTextureUnitState("OgreMap.png");
	t = matInner->getTechnique(0)->getPass(0)->createTextureUnitState("OgreMap_in.png");

	Plane plane(Vector3::UNIT_Y, 0);
	MeshManager::getSingleton().createPlane(
		"Ground",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane,
		32, 32,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundEntity = mSceneMgr->createEntity("GroundPlane", "Ground");
	groundEntity->setMaterialName("PlaneMat");
	groundEntity->setCastShadows(false);
	auto pPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	pPlaneNode->attachObject(groundEntity);
	pPlaneNode->setPosition(Vector3::ZERO);
	//
	Plane planeInner(Vector3::UNIT_Y, 0.005f);
	MeshManager::getSingleton().createPlane(
		"GroundInner",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		planeInner,
		25, 25,
		1, 1,
		true, 1, 1, 1,
		Vector3::NEGATIVE_UNIT_Z
		);

	Entity* groundInnerEntity = mSceneMgr->createEntity("GroundPlaneInner", "GroundInner");
	groundInnerEntity->setMaterialName("PlaneInnerMat");
	groundInnerEntity->setCastShadows(false);
	auto pInnerPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	pInnerPlaneNode->attachObject(groundInnerEntity);
	pInnerPlaneNode->setPosition(Vector3::ZERO);
}

void PlayState::_drawGridPlane(void)
{
	// ��ǥ�� ǥ��
	Ogre::Entity* mAxesEntity = mSceneMgr->createEntity("Axes", "axes.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode("AxesNode", Ogre::Vector3(0, 0, 0))->attachObject(mAxesEntity);
	mSceneMgr->getSceneNode("AxesNode")->setScale(5, 5, 5);

	Ogre::ManualObject* gridPlane = mSceneMgr->createManualObject("GridPlane");
	Ogre::SceneNode* gridPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("GridPlaneNode");

	Ogre::MaterialPtr gridPlaneMaterial = Ogre::MaterialManager::getSingleton().create("GridPlanMaterial", "General");
	gridPlaneMaterial->setReceiveShadows(false);
	gridPlaneMaterial->getTechnique(0)->setLightingEnabled(true);
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setDiffuse(1, 1, 1, 0);
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setAmbient(1, 1, 1);
	gridPlaneMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(1, 1, 1);

	gridPlane->begin("GridPlaneMaterial", Ogre::RenderOperation::OT_LINE_LIST);
	for (int i = 0; i<21; i++)
	{
		gridPlane->position(-500.0f, 0.0f, 500.0f - i * 50);
		gridPlane->position(500.0f, 0.0f, 500.0f - i * 50);

		gridPlane->position(-500.f + i * 50, 0.f, 500.0f);
		gridPlane->position(-500.f + i * 50, 0.f, -500.f);
	}

	gridPlane->end();

	gridPlaneNode->attachObject(gridPlane);
}


//���� �Լ���
void PlayState::stageStart()
{
	//��� �� ����Ʈ �ٽ� Ȱ��ȭ, ���� ����, �ð� ���� ��ŸƮ ���ο� ��ġ, target��ġ ����ֱ�
	mNextStageStartTime = 90.0f;	//stageEnd���� ó���ҰŶ� ���� �߿����� ���� �� �Լ��� �������� �Ҹ��� �ʰ�...
	mCountDownStart = false;
	mStageStarted = true;
	mStageElapsedTime = 0.0f;
	++mStageNumber;
	
	mEnemyListIter = mEnemyObjects.begin();

	for (auto obj : mEnemyObjects)
	{
		obj->mIsDeath = true;
		obj->mIsRegen = false;
		obj->mCurrentAnimationState->setEnabled(false);
		obj->mCurrentAnimationState = obj->mMoveState;
		obj->mCurrentAnimationState->setEnabled(true);
		obj->mDieState->setTimePosition(0.0f);

		obj->mObjectDirection = Vector3::ZERO;
		obj->mObjectDestination = Vector3::ZERO;
		obj->mObjectEntity->getParentNode()->setPosition(500.0f, 0.25f, -500.0f); //�����ġ
		obj->mObjectEntity->setVisible(true);

		obj->mObjectData->mHP = mStageNumber * 20.0f;
		obj->mObjectData->mMoveSpeed = 5.0f + (mStageNumber * 0.25f);
	}
}

void PlayState::stageUpdate(float elapsedTime)
{
	mStageElapsedTime += elapsedTime;
	int nRegenEnemy = int(mStageElapsedTime) / 2;

	if (mStageElapsedTime >= STAGE_TIME)
	{
		stageEnd(false);
		return;
	}

	else if (mStageElapsedTime >= 30.0f)	//��� ���� �� ���¿���
	{
		int nDeath = 0;
		for (auto obj : mEnemyObjects)
		{
			if ((obj->mIsRegen == true) && (obj->mIsDeath == true)) 
				++nDeath;
		}

		if (nDeath == 25)
		{
			stageEnd(true);
			return;
		}
	}

	//������ ���� Update
	if(mEnemyListIter != mEnemyObjects.end())
	{
		mEnemyListIter = mEnemyObjects.begin();
		for (int i = 1; i < nRegenEnemy; ++i)
		{
			if(mEnemyListIter != mEnemyObjects.end())
				++mEnemyListIter;
		}
		
		if(mEnemyListIter != mEnemyObjects.end() && nRegenEnemy >= 1)
		{
			if ((*mEnemyListIter)->mIsRegen == false)	//���� �������� ���� ��츸
			{
				(*mEnemyListIter)->mIsRegen = true;
				(*mEnemyListIter)->mIsDeath = false;

				(*mEnemyListIter)->mObjectEntity->getParentNode()->setPosition(mEnemyMovePosition1);

				(*mEnemyListIter)->mObjectDestination = mEnemyMovePosition2;
				(*mEnemyListIter)->mObjectDirection = (mEnemyMovePosition2 - (*mEnemyListIter)->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();

				Quaternion srcQuat = (*mEnemyListIter)->mObjectEntity->getParentNode()->getOrientation();
				Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo((*mEnemyListIter)->mObjectDirection);
				Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
				(*mEnemyListIter)->mObjectEntity->getParentNode()->setOrientation(delta);
			}
		}
	}

	//������ ���� Update
	for (auto obj : mEnemyObjects)
	{
		if (obj->mIsRegen == false) continue;
		if (obj->mIsDeath == true)
		{
			obj->mCurrentAnimationState->addTime(elapsedTime);
			if (obj->mCurrentAnimationState->hasEnded())
				obj->mObjectEntity->setVisible(false);
			continue;
		}
		Real move = obj->mObjectData->mMoveSpeed * elapsedTime;
		obj->mObjectEntity->getParentNode()->translate(obj->mObjectDirection * move);

		//Ÿ�� ��ġ�� ���� ���� ó�� �� �� Ÿ�� ��ġ ����ֱ� ������ �������� ���͸� �߻��ϳ�
		if (obj->mObjectEntity->getParentNode()->getPosition().distance(obj->mObjectDestination) <= (move + 0.2f))
		{ 
			obj->mObjectEntity->getParentNode()->setPosition(obj->mObjectDestination); // ��ǥ ������ ĳ���͸� ��ġ
		
			if(obj->mObjectDestination == mEnemyMovePosition2)	//���ϴ����� ���� ��쿴�ٸ�
			{
				obj->mObjectDestination = mEnemyMovePosition3;
				obj->mObjectDirection = (mEnemyMovePosition3 - obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
			}
			else if (obj->mObjectDestination == mEnemyMovePosition3)	//���ϴ����� ���� ��쿴�ٸ�
			{
				obj->mObjectDestination = mEnemyMovePosition4;
				obj->mObjectDirection = (mEnemyMovePosition4 - obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
			}
			else if (obj->mObjectDestination == mEnemyMovePosition4)	//�������� ���� ��쿴�ٸ�
			{
				obj->mObjectDestination = mEnemyMovePosition1;
				obj->mObjectDirection = (mEnemyMovePosition1 - obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
			}
			else // (obj->mObjectDestination == mEnemyMovePosition1) ��ŸƮ ��ġ�� ���� ��쿴�ٸ�
			{
				obj->mObjectDestination = mEnemyMovePosition2;
				obj->mObjectDirection = (mEnemyMovePosition2 - obj->mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
			}

			Quaternion srcQuat = obj->mObjectEntity->getParentNode()->getOrientation();
			Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(obj->mObjectDirection);
			Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
			obj->mObjectEntity->getParentNode()->setOrientation(delta);
		}

		obj->mCurrentAnimationState->addTime(elapsedTime);
	}
}

void PlayState::stageEnd(bool bStageClear)
{
	for (auto obj : mEnemyObjects)
	{
		obj->mObjectEntity->setVisible(false);
	}

	if(bStageClear)
	{
		if (mStageNumber == CLEAR_GAME_STAGE)	//Ŭ������
		{
			mIsGameWin = true;
			mStartTime = (timeGetTime() - mStartTime) / 1000.0f;
		}

		mStageElapsedTime = 0.0f;
		mStageStarted = false;
		mNextStageStartTime = WAIT_TIME;
		mCountDownStart = false;
		mMineral += 100 * mStageNumber;
	}
	else //false == bStageClear
	{
		mIsDefeat = true;
		mCountDownStart = false;
	}
}

void PlayState::attackEnemy(AnimateObject* my, AnimateObject* enemy)
{
	int damage = my->mObjectData->mDamage;
	enemy->mObjectData->mHP -= damage;
	if (enemy->mObjectData->mHP <= 0)
	{
		enemy->mObjectData->mHP = 0;
		enemy->mIsDeath = true;
		enemy->mCurrentAnimationState->setEnabled(false);
		enemy->mCurrentAnimationState = enemy->mDieState;
		enemy->mCurrentAnimationState->setEnabled(true);

		mMineral += 2;
	}
}

bool PlayState::createObject(int unitType)
{
	char nameBuf[20];
	char meshBuf[20];
	int requiredMineral = 0;
	ObjectData* pObjectData = nullptr;

	switch (unitType)
	{
	case OBJECT_TYPE_MARINE:
	{
		sprintf(meshBuf, "marine.mesh");
		sprintf(nameBuf, "marine%d", ++mID);
		requiredMineral = 50;
		pObjectData = &mMarineData;
		break;
	}
	case OBJECT_TYPE_REAPER:
	{
		sprintf(meshBuf, "reaper.mesh");
		sprintf(nameBuf, "reaper%d", ++mID);
		requiredMineral = 100;
		pObjectData = &mReaperData;
		break;
	}
	case OBJECT_TYPE_MARAUDER:
	{
		sprintf(meshBuf, "marauder.mesh");
		sprintf(nameBuf, "marauder%d", ++mID);
		requiredMineral = 150;
		pObjectData = &mMarauderData;
		break;
	}
	default:
	{
		printf("Error occured : createObject\n");
		//break;
		return false;
	}
	}	//end switch

	
		//�̳׶��� �䱸 �̳׶����� ������ �ٷ� false ����
	if (mMineral < requiredMineral)
	{
		_cprintf("not enough minerals\n");
		PlaySound(TEXT("Sounds/Alert_TerranNeedMoreMinerals.wav"), 0, SND_ASYNC);
		return false;
	}
	else
	{
		mMineral -= requiredMineral;
		_cprintf("new unit created!\n");
	}

	//���� �� �ʱ�ȭ
	AnimateObject* pObject = new AnimateObject();
	pObject->mObjectEntity = mSceneMgr->createEntity(nameBuf, meshBuf);
	SceneNode* pNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(nameBuf, Vector3(-3.0f, 0.25f, 3.0f));
	pNode->attachObject(pObject->mObjectEntity);
	
	if (unitType == OBJECT_TYPE_MARINE)
	{
		++mMarineNum;
		PlaySound(TEXT("Sounds/Marine_Create.wav"), 0, SND_ASYNC);
		pNode->setScale(Vector3(0.75f, 0.75f, 0.75f));	//������ ���� Ŀ��;
	}
	else if (unitType == OBJECT_TYPE_REAPER)
	{
		++mReaperNum;
		PlaySound(TEXT("Sounds/Reaper_Create.wav"), 0, SND_ASYNC);
		pNode->setScale(Vector3(1.5f, 1.5f, 1.5f));
	}
	else //MARAUDER
	{
		++mMarauderNum;
		PlaySound(TEXT("Sounds/Marauder_Create.wav"), 0, SND_ASYNC);
		pNode->setScale(Vector3(1.5f, 1.5f, 1.5f));
	}

	pObject->mIdleState = pObject->mObjectEntity->getAnimationState("Idle");
	pObject->mAttackState = pObject->mObjectEntity->getAnimationState("Attack");
	pObject->mMoveState = pObject->mObjectEntity->getAnimationState("Move");
	pObject->mDieState = pObject->mObjectEntity->getAnimationState("Die");
	pObject->mIdleState->setLoop(true);
	pObject->mAttackState->setLoop(false);	//���ݼӵ� ó�� �׽�Ʈ��
	pObject->mMoveState->setLoop(true);
	pObject->mDieState->setLoop(false);
	pObject->mCurrentAnimationState = pObject->mIdleState;
	pObject->mCurrentAnimationState->setEnabled(true);

	pObject->mObjectDirection = Vector3::ZERO;
	pObject->mObjectDestination = Vector3::ZERO;	//�ǹ��� ���������̹Ƿ� ZERO�� ���������� �ξ ������.
	pObject->mObjectState = OBJ_STATE::alert;
	pObject->mObjectTargetObject = nullptr;
	pObject->mObjectData = pObjectData;

	mMyObjects.push_back(pObject);
	mMapMyObjects.insert(make_pair(pObject->mObjectEntity, pObject));
	return true;
}

bool PlayState::upgradeUnit(int upgradeType)
{
	int upgradeFee = 50;
	if (mMineral < 50)
	{
		_cprintf("not enough minerals\n");
		PlaySound(TEXT("Sounds/Alert_TerranNeedMoreMinerals.wav"), 0, SND_ASYNC);
		return false;
	}
	else
	{
		_cprintf("upgrade complete!\n");
		mMineral -= 50;
	}
	switch (upgradeType)
	{
	case UPGRADE_TYPE_DAMAGE:
	{
		mMarineData.mDamage += 3;
		mReaperData.mDamage += 3;
		mMarauderData.mDamage += 10;
		++mDamageNum;
		break;
	}
	case UPGRADE_TYPE_SPEED:
	{
		mMarineData.mAttackSpeed += 0.2f;
		mReaperData.mAttackSpeed += 0.2f;
		mMarauderData.mAttackSpeed += 0.2f;
		++mSpeedNum;
		break;
	}
	}

	PlaySound(TEXT("Sounds/Alert_TerranUpgradeComplete.wav"), 0, SND_ASYNC);
	return true;
}