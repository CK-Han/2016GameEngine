#include "TitleState.h"
#include "PlayState.h"

using namespace Ogre;

TitleState TitleState::mTitleState;

void TitleState::enter(void)
{
	mContinue = true;
	mTitleOverlay = OverlayManager::getSingleton().getByName("Overlay/Title");
	mTitleOverlay->show();

	mStartMsg = OverlayManager::getSingleton().getOverlayElement("StartMsg");
	mStartMsg->show();
	
	mGameStateMsg = OverlayManager::getSingleton().getOverlayElement("GameStateMessage");
	mGameStateMsg->show();
	
	mResultMarineNum = OverlayManager::getSingleton().getOverlayElement("ResultMarineMessage");
	mResultReaperNum = OverlayManager::getSingleton().getOverlayElement("ResultReaperMessage");
	mResultMarauderNum = OverlayManager::getSingleton().getOverlayElement("ResultMarauderMessage");
	mResultDamageUpNum = OverlayManager::getSingleton().getOverlayElement("ResultDamageMessage");
	mResultSpeedUpNum = OverlayManager::getSingleton().getOverlayElement("ResultSpeedMessage");
}

void TitleState::exit(void)
{
	mTitleOverlay->hide();
}

void TitleState::pause(void)
{
	std::cout << "TitleState pause\n";
}

void TitleState::resume(void)
{
	std::cout << "TitleState resume\n";
}

bool TitleState::frameStarted(GameManager* game, const FrameEvent& evt)
{
	char buf[50];

	switch (game->mGameClearState)
	{
	case 0:	//시작 전
	{
		sprintf(buf, " ");
		mGameStateMsg->setCaption(buf);
		setResultShow(false);
		break;
	}
	case 1:	//패배
	{
		sprintf(buf, "Defeat....");
		mGameStateMsg->setCaption(buf);
		setResultElements(game);
		setResultShow(true);
		break;
	}
	case 2:	//승리
	{
		sprintf(buf, "Win!! clear time : %f", game->mGameClearTime);
		mGameStateMsg->setCaption(buf);
		setResultElements(game);
		setResultShow(true);
		break;
	}
	}
	
	return true;
}

bool TitleState::frameEnded(GameManager* game, const FrameEvent& evt)
{
	return mContinue;
}

bool TitleState::keyPressed(GameManager* game, const OIS::KeyEvent &e)
{
	// Fill Here -------------------------------------------------------
	switch (e.key)
	{
	case OIS::KC_SPACE:
		game->changeState(PlayState::getInstance());
		break;
	case OIS::KC_ESCAPE:
		mContinue = false;
		break;
	}
	// -----------------------------------------------------------------

	return true;
}
