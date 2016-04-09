#pragma once

#include "stdafx.h"
#include "Timer.h"
#include "Object.h"

using namespace Ogre;

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	void		Initialize();					//리소스 등록 및 그 외 초기화 작업
	void		handlingInput();				//입력 처리
	void		update();						//프레임에 따른 게임 로직 처리?
	
public:
	CGameObject*		m_pSelectedObject;		//피킹에 의해 현재 선택된 객체, 타입을 통해 구분
	unsigned char		m_iGameState;			//현재 게임 상태
	unsigned int		m_iUserMineral;
	
};

