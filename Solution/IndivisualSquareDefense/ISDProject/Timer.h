#pragma once
#include "stdafx.h"

class CGameTimer
{
public:
	CGameTimer();				//evt.timeSinceLastFrame Ȱ��
	~CGameTimer();

	void enterWaitTime();		//���� �ð� ����
	void enterStageTime();		//�������� �ð� ����
	
	void Tick();				//�ð� Ȯ��

public:
	__int64			m_nCurrentTime;						// Current Performance Counter
	__int64			m_nLastTime;						// Performance Counter last frame
	//define ���� �̿��� ���� �� �������� �ð� Ȱ��
};

