#pragma once
#include "stdafx.h"

class CGameTimer
{
public:
	CGameTimer();				//evt.timeSinceLastFrame 활용
	~CGameTimer();

	void enterWaitTime();		//정비 시간 진입
	void enterStageTime();		//스테이지 시간 진입
	
	void Tick();				//시간 확인

public:
	__int64			m_nCurrentTime;						// Current Performance Counter
	__int64			m_nLastTime;						// Performance Counter last frame
	//define 값을 이용한 정비 및 스테이지 시간 활용
};

