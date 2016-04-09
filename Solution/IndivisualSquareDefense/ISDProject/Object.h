#pragma once

#include "stdafx.h"
using namespace Ogre;

class CGameObject {
public:
	CGameObject();
	~CGameObject();

private:
	unsigned char		m_iId;			//UnitManager에서 객체 구분을 위한
	unsigned char		m_iType;		//유닛 정보(생산인지 업그레이드인지 마린인지 고스트인지)
	Entity*				m_pEntity;		//엔티티 정보
	SceneNode*			m_pNode;		//노드 정보
	Vector3				m_vMyPos;		//위치
};

//************************************************************************************
class CUnit : public CGameObject
{
public:
	CUnit();					//m_iType을 입력받으면 그에 맞는 메쉬 및 애니메이션 생성
	~CUnit();

	CUnit*		makeUnit();		//유닛 종류, 생성 위치
	void		KillUnit();		//유닛 제거
	void		move();			//마우스 클릭 위치, 경과시간
	void		attack();		//경과 시간, 위치로 이동하면서 매번 타겟이 있는지 검사
	bool		findTarget();	//공격범위 내 타겟 설정
	void		stop();			//현 위치에 정지, 이후 타겟 탐지
	
	void		updateInfo();	//업그레이드 유닛 정보 갱신

	//get & set Methods;

private:
	//애니메이션 정보가 추가로 필요하겠다.
	unsigned char		m_iCommand;		//현재 진행중인 명령어
	Vector3				m_vMovePos;		//이동하고자 하는 위치
	CUnit*				m_pTarget;		//공격하고자 하는 대상
	
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
	virtual void upgradeHealth(); //적 유닛 용
	
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
	virtual void upgradeHealth(); //적 유닛 용
};

class MarauderInfo : public UnitInfo{
public:
	MarauderInfo();
	~MarauderInfo();

	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //적 유닛 용
};

class GhostInfo : public UnitInfo{
public:
	GhostInfo();
	~GhostInfo();

	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //적 유닛 용
};

class EnemyInfo : public UnitInfo{
public:
	EnemyInfo();
	~EnemyInfo();

	virtual void upgradeDamage();
	virtual void upgradeAttackSpeed();
	virtual void upgradeHealth(); //적 유닛 용
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

	//동작함수들?
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
