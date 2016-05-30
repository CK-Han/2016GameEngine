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
	
	
	//���� �� UI ������
	//_drawGridPlane();
	//_setLights();
	_drawGroundPlane();
	
	//PlayState�� Overlay ����
	//mInformationOverlay = OverlayManager::getSingleton().getByName("Overlay/Information");
	//mInformationOverlay->show();
	
#ifdef _DEBUG
	AllocConsole();
#endif

	// �� �� ��� ����
	mObjectEntity = mSceneMgr->createEntity("marine", "marine.mesh");
	SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("marine", Vector3(0.0f, 0.25f, 0.0f));
	node1->attachObject(mObjectEntity);
	node1->showBoundingBox(true);

	//�ӽ�
	mTempEnemyEntity = mSceneMgr->createEntity("marineEnemy", "marine.mesh");
	SceneNode* node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode("marineEnemy", Vector3(-13.0f, 0.25f, -13.0f));
	node2->attachObject(mTempEnemyEntity);
	node2->showBoundingBox(true);

	mTempDieState = mRoot->getSceneManager("main")->getEntity("marineEnemy")->getAnimationState("Die");
	mTempDieState->setLoop(false);
	mTempDieState->setEnabled(false);

	//��ü ���� �⺻ ���� ����
	mMoveState = mRoot->getSceneManager("main")->getEntity("marine")->getAnimationState("Move");
	mIdleState = mRoot->getSceneManager("main")->getEntity("marine")->getAnimationState("Idle");
	mAttackState = mRoot->getSceneManager("main")->getEntity("marine")->getAnimationState("Attack");
	mDieState = mRoot->getSceneManager("main")->getEntity("marine")->getAnimationState("Die");

	mMoveState->setLoop(true);
	mIdleState->setLoop(true);
	mAttackState->setLoop(true);
	mDieState->setLoop(false);

	mCurrentAnimationState = mIdleState;
	mCurrentAnimationState->setEnabled(true);
	mObjectDirection = Vector3::ZERO;
	mObjectSpeed = 3.0f;
	mObjectRange = 4.0f;
	mObjectState = OBJ_STATE::alert;
	mObjectTargetEntity = nullptr;
}

void PlayState::exit(void)
{
	// Fill Here -----------------------------
	mSceneMgr->clearScene();
#ifdef _DEBUG
	FreeConsole();
#endif
	//mInformationOverlay->hide();
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
	mCamera->moveRelative(mCameraMoveVector * 20.0f * evt.timeSinceLastFrame);
	
	//temp
	static bool isEnemyDeath = false;
	
	//��ü ����
	switch (mObjectState)
	{
	case OBJ_STATE::move:
	{	//������ ���콺�� ������ ����, �̵� ��û�� ���
		if (Vector3::ZERO != mObjectDirection)
		{
			Real move = mObjectSpeed * evt.timeSinceLastFrame;
			mObjectEntity->getParentNode()->translate(mObjectDirection * move);

			if (mObjectEntity->getParentNode()->getPosition().distance(mObjectDestination) <= 0.05f)
			{ // ��ǥ ������ ������ ���
				mObjectEntity->getParentNode()->setPosition(mObjectDestination); // ��ǥ ������ ĳ���͸� ��ġ
				mObjectDirection = Vector3::ZERO;

				mCurrentAnimationState->setEnabled(false);
				mCurrentAnimationState = mIdleState;
				mCurrentAnimationState->setEnabled(true);

				mObjectState = OBJ_STATE::alert;
			}
		}
	}
	break;

	case OBJ_STATE::alert:
	{	//�����Ÿ� �� ���� �ִ��� �Ǵ��Ͽ�, �ִ� ��� �����Ѵ�
		Vector3 myPos = mObjectEntity->getParentNode()->getPosition();
		Vector3 otherPos = mTempEnemyEntity->getParentNode()->getPosition();
		Real distance = myPos.distance(otherPos); //y�� ������ �����ǹǷ� �����ص� �ȴ�.
		if (distance <= mObjectRange)
		{
			Quaternion srcQuat = mObjectEntity->getParentNode()->getOrientation();
			Vector3 direction = (mTempEnemyEntity->getParentNode()->getPosition() - mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
			Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(direction);
			Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
			mObjectEntity->getParentNode()->setOrientation(delta);

			mCurrentAnimationState->setEnabled(false);
			mCurrentAnimationState = mAttackState;
			mCurrentAnimationState->setEnabled(true);

			mObjectTargetEntity = mTempEnemyEntity;
			mObjectState = OBJ_STATE::have_target;
		}
	}
	break;

	case OBJ_STATE::have_target:
	{	//Ÿ���� ������ �� �����Ÿ� ���� �ִ��� Ȯ���Ѵ�.


		if (false == isEnemyDeath)
		{

			Vector3 myPos = mObjectEntity->getParentNode()->getPosition();
			Vector3 otherPos = mTempEnemyEntity->getParentNode()->getPosition();
			Real distance = myPos.distance(otherPos); //y�� ������ �����ǹǷ� �����ص� �ȴ�.
			if (distance <= mObjectRange)
			{	//���� �����Ÿ� �� ��ȿ�� ���� �ִٸ� ��� ������ ����; ���⸸ ����شޱ�
				Quaternion srcQuat = mObjectEntity->getParentNode()->getOrientation();
				Vector3 direction = (mTempEnemyEntity->getParentNode()->getPosition() - mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
				Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(direction);
				Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
				mObjectEntity->getParentNode()->setOrientation(delta);

				static float damaged_time = 0.0f;
				damaged_time += evt.timeSinceLastFrame;

				if (damaged_time >= 3.0f)
				{
					mTempDieState->setEnabled(true);
					isEnemyDeath = true;

					mCurrentAnimationState->setEnabled(false);
					mCurrentAnimationState = mIdleState;
					mCurrentAnimationState->setEnabled(true);
				}
			}
			else
			{	//���� ������ �����ٸ� �ٽ� ��� ���·� ����.
				mCurrentAnimationState->setEnabled(false);
				mCurrentAnimationState = mIdleState;
				mCurrentAnimationState->setEnabled(true);

				mObjectTargetEntity = nullptr;
				mObjectState = OBJ_STATE::alert;
			}

		}
	}
	break;

	} //end switch
	
	static float death_event_time = 0.0f;
	if (true == isEnemyDeath)
	{
		death_event_time += evt.timeSinceLastFrame;
		mTempDieState->addTime(evt.timeSinceLastFrame);
		if (death_event_time >= 5.0f)
			mTempEnemyEntity->getParentNode()->setPosition(FLT_MAX, FLT_MAX, FLT_MAX);
	}

	mCurrentAnimationState->addTime(evt.timeSinceLastFrame);
	return true;
}

bool PlayState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	/*
	static Ogre::DisplayString currFps = L"���� FPS: ";
	static Ogre::DisplayString avgFps = L"��� FPS: ";
	static Ogre::DisplayString bestFps = L"�ְ� FPS: ";
	static Ogre::DisplayString worstFps = L"���� FPS: ";

	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("WorstFps");

	const RenderTarget::FrameStats& stats = mRoot->getAutoCreatedWindow()->getStatistics();

	guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
	guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
	guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS));
	guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS));
	
	*/
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
	case OIS::KC_2:
	case OIS::KC_3:
	case OIS::KC_4:
	{
		mCurrentAnimationState->setEnabled(false);
		if (OIS::KC_1 == e.key)
			mCurrentAnimationState = mIdleState;
		else if (OIS::KC_2 == e.key)
			mCurrentAnimationState = mAttackState;
		else if (OIS::KC_3 == e.key)
			mCurrentAnimationState = mMoveState;
		else if (OIS::KC_4 == e.key)
			mCurrentAnimationState = mDieState;
		mCurrentAnimationState->setEnabled(true);
	}
	break;
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
		}
		if (e.state.buttonDown(OIS::MB_Right)) //��ü �̵�
		{
			if (mSelectedEntity->getName() == "marine")
			{
				pickPosition.y = mSelectedEntity->getParentNode()->getPosition().y;
				mObjectDestination = pickPosition;
				mObjectDirection = (pickPosition - mObjectEntity->getParentNode()->getPosition()).normalisedCopy();
				
				Quaternion srcQuat = mObjectEntity->getParentNode()->getOrientation();
				Quaternion destQuat = Vector3(Vector3::UNIT_Z).getRotationTo(mObjectDirection);
				Quaternion delta = Quaternion::Slerp(1.0f, srcQuat, destQuat, true);
				mObjectEntity->getParentNode()->setOrientation(delta);

				mCurrentAnimationState->setEnabled(false);
				mCurrentAnimationState = mMoveState;
				mCurrentAnimationState->setEnabled(true);

				mObjectState = OBJ_STATE::move;
			}
			else if (mSelectedEntity->getName() == "marineEnemy")
			{
				pickPosition.y = mSelectedEntity->getParentNode()->getPosition().y;
				mTempEnemyEntity->getParentNode()->setPosition(pickPosition);
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
	Plane planeInner(Vector3::UNIT_Y, 0.05f);
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