#pragma once

#include "stdafx.h"
using namespace Ogre;

class CGameObject {
public:
	CGameObject();
	~CGameObject();

private:
	unsigned char		m_iId;			//UnitManager���� ��ü ������ ����
	unsigned char		m_iType;		//���� ����(�������� ���׷��̵����� �������� ��Ʈ����)
	Entity*				m_pEntity;		//��ƼƼ ����
	SceneNode*			m_pNode;		//��� ����
	Vector3				m_vMyPos;		//��ġ
};

//************************************************************************************
class CUnit : public CGameObject
{
public:
	CUnit();					//m_iType�� �Է¹����� �׿� �´� �޽� �� �ִϸ��̼� ����
	~CUnit();

	CUnit*		makeUnit();		//���� ����, ���� ��ġ
	void		KillUnit();		//���� ����
	void		move();			//���콺 Ŭ�� ��ġ, ����ð�
	void		attack();		//��� �ð�, ��ġ�� �̵��ϸ鼭 �Ź� Ÿ���� �ִ��� �˻�
	bool		findTarget();	//���ݹ��� �� Ÿ�� ����
	void		stop();			//�� ��ġ�� ����, ���� Ÿ�� Ž��
	
	void		updateInfo();	//���׷��̵� ���� ���� ����

	//get & set Methods;

private:
	//�ִϸ��̼� ������ �߰��� �ʿ��ϰڴ�.
	unsigned char		m_iCommand;		//���� �������� ��ɾ�
	Vector3				m_vMovePos;		//�̵��ϰ��� �ϴ� ��ġ
	CUnit*				m_pTarget;		//�����ϰ��� �ϴ� ���
	
	Vector3				m_vMyPos;
	unsigned int		m_iDamage;
	unsigned int		m_iHealth;
	unsigned char		m_iRange;
	float				m_fAttackSpeed;
};

//************************************************************************************
class UnitInfo {
public:
	UnitInfo();
	~UnitInfo();

protected:
	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //�� ���� ��
	
private:
	unsigned int		m_iDamage;
	unsigned int		m_iHealth;
	unsigned char		m_iRange;
	float				m_fAttackSpeed;
};

class MarineInfo : public UnitInfo{
public:
	MarineInfo();
	~MarineInfo();
	
	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //�� ���� ��
};

class MarauderInfo : public UnitInfo{
public:
	MarauderInfo();
	~MarauderInfo();

	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //�� ���� ��
};

class GhostInfo : public UnitInfo{
public:
	GhostInfo();
	~GhostInfo();

	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //�� ���� ��
};

class EnemyInfo : public UnitInfo{
public:
	EnemyInfo();
	~EnemyInfo();

	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //�� ���� ��
};


//************************************************************************************
class UnitManager {
public:
	std::vector<CUnit*>			m_myUnitVector;
	std::vector<CUnit*>			m_EnemyVector;

	MarineInfo*					m_pMarineInfo;
	MarauderInfo*				m_pMarauderInfo;
	GhostInfo*					m_pGhostInfo;
	EnemyInfo*					m_pEnemyInfo;

	//�����Լ���?
};


//************************************************************************************
class MakeBuilding : public CGameObject{
public:
	MakeBuilding();
	~MakeBuilding();

private:
	unsigned int		m_iMarineFee;
	unsigned int		m_iMarauderFee;
	unsigned int		m_iGhostFee;
};

class UpgradeBuilding : public CGameObject{
public:
	UpgradeBuilding();
	~UpgradeBuilding();

private:
	unsigned int		m_iNextDamageFee;
	unsigned int		m_iNextSpeedFee;
};
