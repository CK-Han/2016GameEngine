#pragma once

#define		CLIENT_WIDTH		1024.0f
#define		CLIENT_HEIGHT		768.0f

#define		OBJECT_TYPE_MARINE				0x0001
#define		OBJECT_TYPE_REAPER				0x0002
#define		OBJECT_TYPE_MARAUDER			0x0003

#define		COMMAND_ATTACK	0x0001	
#define		COMMAND_STOP	0x0002
#define		COMMAND_MOVE	0x0003


#define		WAIT_TIME		30
#define		STAGE_TIME		90

typedef enum {
	state_lobby = 1,
	state_wait,
	state_stage,
	state_defeat,
	state_win
};