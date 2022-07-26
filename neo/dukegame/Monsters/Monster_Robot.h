// Monster_Robot.h
//

#pragma once

enum ROBOT_IDLE_STATE
{
	ROBOT_IDLE_WAITINGTPLAYER = 0,
	ROBOT_IDLE_ROAR
};

//
// DnLiztroop
//
class DnRobot : public DnAI
{
	CLASS_PROTOTYPE(DnRobot);
public:
	stateResult_t				state_Begin(stateParms_t* parms);
	stateResult_t				state_Idle(stateParms_t* parms);
	stateResult_t				state_ApproachingEnemy(stateParms_t* parms);
	stateResult_t				state_ShootEnemy(stateParms_t* parms);
	stateResult_t				state_BeginDeath(stateParms_t* parms);
	stateResult_t				state_Killed(stateParms_t* parms);
private:
	const idSoundShader* troop_awake;
	const idSoundShader* fire_sound;
	const idSoundShader* death_sound;
};