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

	void		Initialize();					//���ҽ� ��� �� �� �� �ʱ�ȭ �۾�
	void		handlingInput();				//�Է� ó��
	void		update();						//�����ӿ� ���� ���� ���� ó��?
	
public:
	CGameObject*		m_pSelectedObject;		//��ŷ�� ���� ���� ���õ� ��ü, Ÿ���� ���� ����
	unsigned char		m_iGameState;			//���� ���� ����
	unsigned int		m_iUserMineral;
	
};

