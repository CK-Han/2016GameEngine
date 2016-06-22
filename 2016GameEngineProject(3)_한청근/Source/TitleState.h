#pragma once

#include "GameState.h"

class TitleState : public GameState
{
public:
  // Fill Here --------------------------------------------------------------------------------------------
	void enter();
	void exit();

	void pause();
	void resume();

	bool frameStarted(GameManager* game, const Ogre::FrameEvent& evt);
	bool frameEnded(GameManager* game, const Ogre::FrameEvent& evt);

	bool mouseMoved(GameManager* game, const OIS::MouseEvent& e) { return true; }
	bool mousePressed(GameManager* game, const OIS::MouseEvent& e, OIS::MouseButtonID id) { return true; }
	bool mouseReleased(GameManager* game, const OIS::MouseEvent& e, OIS::MouseButtonID id) { return true; }

	bool keyPressed(GameManager* game, const OIS::KeyEvent &e);
	bool keyReleased(GameManager* game, const OIS::KeyEvent &e) { return true; }
	// -------------------------------------------------------------------------------------------------------

	void setResultElements(GameManager* game)
	{
		char buf[50];
		sprintf(buf, "Created Marine : %d", game->mGameClearMarine);
		mResultMarineNum->setCaption(buf);
		sprintf(buf, "Created Reaper : %d", game->mGameClearReaper);
		mResultReaperNum->setCaption(buf);
		sprintf(buf, "Created Marauder : %d", game->mGameClearMarauder);
		mResultMarauderNum->setCaption(buf);
		sprintf(buf, "Upgrade Damage : %d", game->mGameClearDamageUp);
		mResultDamageUpNum->setCaption(buf);
		sprintf(buf, "Upgrade Speed : %d", game->mGameClearSpeedUp);
		mResultSpeedUpNum->setCaption(buf);
	}

	void setResultShow(bool isShow)
	{
		if (true == isShow)
		{
			mResultMarineNum->show();
			mResultReaperNum->show();
			mResultMarauderNum->show();
			mResultDamageUpNum->show();
			mResultSpeedUpNum->show();
		}
		else
		{
			mResultMarineNum->hide();
			mResultReaperNum->hide();
			mResultMarauderNum->hide();
			mResultDamageUpNum->hide();
			mResultSpeedUpNum->hide();
		}
	}

  static TitleState* getInstance() { return &mTitleState; }

private:
  static TitleState mTitleState;

  bool mContinue;

  Ogre::Overlay* mTitleOverlay;
  Ogre::OverlayElement* mStartMsg;
  Ogre::OverlayElement* mGameStateMsg;
  Ogre::OverlayElement* mResultMarineNum;
  Ogre::OverlayElement* mResultReaperNum;
  Ogre::OverlayElement* mResultMarauderNum;
  Ogre::OverlayElement* mResultDamageUpNum;
  Ogre::OverlayElement* mResultSpeedUpNum;
};
