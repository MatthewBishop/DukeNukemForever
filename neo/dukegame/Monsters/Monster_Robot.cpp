// Monster_Robot.cpp
//


#include "../gamelib/game_local.h"

#define ROBOT_FIRE_DISTANCE			500
#define ROBOT_FORCE_FIREDISTANCE		200

CLASS_DECLARATION(DnAI, DnRobot)
END_CLASS

/*
==============
DnRobot::state_Begin
==============
*/
stateResult_t DnRobot::state_Begin(stateParms_t* parms)
{
	Event_SetAnimation("idle", false);

	Event_SetState("state_Idle");

	troop_awake = declManager->FindSound("Robot_awake", false);
	fire_sound = declManager->FindSound("Robot_fire", false);
	death_sound = declManager->FindSound("Robot_die", false);

	return SRESULT_DONE;
}

/*
==============
DnRobot::state_BeginDeath
==============
*/
stateResult_t DnRobot::state_BeginDeath(stateParms_t* parms)
{
	Event_SetState("state_Killed");

	Event_StopMove();

	Event_SetAnimation("death", false);

	StartSoundShader(death_sound, SND_CHANNEL_ANY, 0, false, nullptr);

	return SRESULT_DONE;
}

/*
==============
DnRobot::state_Killed
==============
*/
stateResult_t DnRobot::state_Killed(stateParms_t* parms)
{
	return SRESULT_WAIT; // Were dead so do nothing.
}

/*
==============
DnRobot::state_ShootEnemy
==============
*/
stateResult_t DnRobot::state_ShootEnemy(stateParms_t* parms)
{
	// If we are firing, don't make any new decisions until its done.
	if ((animator.IsAnimating(gameLocal.time) || CurrentlyPlayingSound()) && GetCurrentAnimation() == "fire")
	{
		Event_TurnToward(target->GetOrigin());
		animator.RemoveOriginOffset(true);
		return SRESULT_WAIT;
	}

	float distToEnemy = 0.0f;
	distToEnemy = (target->GetOrigin() - GetOrigin()).Length();

	if (distToEnemy < ROBOT_FIRE_DISTANCE + 25 || AI_PAIN)
	{
		if (!isTargetVisible)
		{
			Event_SetState("state_ApproachingEnemy");
			return SRESULT_DONE;
		}

		Event_TurnToward(target->GetOrigin());
		Event_ResetAnimation();
		Event_SetAnimation("fire", false);
		StartSoundShader(fire_sound, SND_CHANNEL_ANY, 0, false, nullptr);

		idVec3 muzzleOrigin = GetOrigin() + idVec3(0, 0, 40);
		idVec3 muzzleDir = muzzleOrigin - (target->GetOrigin() + target->GetVisualOffset());

		muzzleDir.Normalize();
		Event_Hitscan("damage_liztoop", muzzleOrigin, -muzzleDir, 1, 1, 10);

		return SRESULT_WAIT;
	}
	else
	{
		Event_SetState("state_ApproachingEnemy");
		return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}

/*
==============
DnRobot::state_ApproachingEnemy
==============
*/
stateResult_t DnRobot::state_ApproachingEnemy(stateParms_t* parms)
{
	float distToEnemy = 0.0f;

	distToEnemy = (target->GetOrigin() - GetOrigin()).Length();

	if (distToEnemy > ROBOT_FIRE_DISTANCE || !isTargetVisible || (CurrentlyPlayingSound() && distToEnemy >= ROBOT_FORCE_FIREDISTANCE))
	{
		Event_UpdatePathToPosition(target->GetOrigin());
		Event_SetAnimation("walk", true);
	}
	else
	{
		Event_StopMove();
		Event_SetState("state_ShootEnemy");
		return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}

/*
==============
DnRobot::state_Idle
==============
*/
stateResult_t DnRobot::state_Idle(stateParms_t* parms)
{
	switch (parms->stage)
	{
		case ROBOT_IDLE_WAITINGTPLAYER:
			target = Event_FindNewTarget();

			if (target != nullptr)
			{
				targetLastSeenLocation = target->GetOrigin();
				isTargetVisible = true;

				StartSoundShader(troop_awake, SND_CHANNEL_ANY, 0, false, nullptr);
				parms->stage = ROBOT_IDLE_ROAR;
			}
			break;

		case ROBOT_IDLE_ROAR:
			Event_SetState("state_ApproachingEnemy");
			return SRESULT_DONE;
	}

	return SRESULT_WAIT;
}