#define CLIENT_DESCRIPTION "ISD_PROJECT"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "GameManager.h"
#include "TitleState.h"
#include "PlayState.h"

using namespace Ogre;

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
	int main(int argc, char *argv[])
#endif
	{
#ifdef _DEBUG
		AllocConsole();
#endif
		// Fill Here ---------------------------------------------------
		GameManager game;
		try
		{
			game.init();
			game.changeState(TitleState::getInstance());
			game.go();
		}
		// --------------------------------------------------------------
		catch (Ogre::Exception& e)
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "An exception has occured: " <<
				e.getFullDescription().c_str() << std::endl;
#endif
		}


#ifdef _DEBUG
		FreeConsole();
#endif
		return 0;
	}

#ifdef __cplusplus
}
#endif