#pragma once

#include "defines.h"
#include "FrameAnimation.h"
#include "Pos.h"
#include "Config.h"
#include "GameMap.h"
#include "tsv.h"
#include "Logger.h"
#include "animation_database.h"

class Player 
{
public:
	enum ActionName : int 
	{
		Idle = 15,		
		Moving = 9,		
		Attack1 = 2,
		// Attack2 = 3,
		Caster1 = 3,
		// Caster2 = 5
	};
	
	Player(int roleID);
	Player();
	~Player();
	void OnUpdate(double dt);
	void ReloadFrames();
	void ResetDirAll(int dir);
	void ResetDir(int dir);
	void SetDir(int dir);
	
	void SetActionID(int state) { m_ActionID = state; };
	int GetActionID(){ return m_ActionID; }

	void OnDraw(SpriteRenderer * renderer, int px,int py);
	void SetPos(double x, double y);
	void SetCombatPos(double x, double y) {m_CombatPos.x=x;m_CombatPos.y=y;};
	Pos GetCombatPos(){ return m_CombatPos;};

	void SetBox();
	double GetX() { return m_Pos.x; }
	double GetY() { return m_Pos.y; }

	int GetBoxX() { return m_Pos.x/20; }
	int GetBoxY() { return m_Pos.y/20; }

	void SetX(double x) { m_Pos.x = x; }
	void SetY(double y) { m_Pos.y = y; }

	void TranslateX(double x) { m_Pos.x += x; }
	void TranslateY(double y) { m_Pos.y += y; }
	
	void PlayAction(int action,int dir);

	void MoveTo(GameMap* gameMapPtr, int param2, int param3);
	void SetVelocity(int velocity) { m_MoveVelocity = velocity; };

	bool IsMove() { return m_IsMove; }
	int GetId() { return m_ID; }
	void HandleMoveToCalled();
	void SaveFrame(int index);
	void ChangeRole();
	void ChangeWeapon();
	void ChangeAction();

	void SetNickName(std::wstring name) { m_NickName= name ;};
	
private:

	std::map<int,FrameAnimation> m_PlayerFrames;
	std::map<int,FrameAnimation> m_WeaponFrames;
	
	std::wstring m_NickName;
	int m_AnimationState;
	int m_Dir;
	Pos m_Pos;
	Pos m_CombatPos;
    BoxPos m_Box;
	std::list<Pos> m_MoveList;
    std::list<Pos> m_BackupMoveList;
    bool m_MoveToCalled;
	double m_UpdateDelta;
	bool m_IsMove;
	int m_ID;
	double m_MoveVelocity;

	int m_RoleID;				//current role
	int m_ActionID;				//current action
	int m_WeaponID;				//current weapon
	bool m_HasWeapon;

	utils::tsv m_ActionTSV;
	utils::tsv m_RoleTSV;
	utils::tsv m_WeaponTSV;
	utils::AnimDB m_AnimDB;
	

	void LogInfo()
	{
		Logger::Print("Role:%s Weapon:%s Action:%s\n",
		m_RoleTSV.query(m_RoleID)[1].c_str(),
		m_WeaponTSV.query(m_WeaponID)[1].c_str(),
		m_ActionTSV.query(m_ActionID)[1].c_str()
		);
	}
};

