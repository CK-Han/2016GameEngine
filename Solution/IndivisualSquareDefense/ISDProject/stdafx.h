#pragma once

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <Ogre.h>
#include <OIS/OIS.h>

#define		MARINE			0x0001
#define		MARAUDER		0x0002
#define		GHOST			0x0003

#define		COMMAND_ATTACK	0x0001	
#define		COMMAND_STOP	0x0002
#define		COMMAND_MOVE	0x0003

#include <vector>

#define		WAIT_TIME		30
#define		STAGE_TIME		90

typedef enum {
	state_lobby = 1,
	state_wait,
	state_stage,
	state_defeat,
	state_win
};