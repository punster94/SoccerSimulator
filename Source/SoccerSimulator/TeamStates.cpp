#include "SoccerSimulator.h"
#include "SoccerTeam.h"
#include "TeamStates.h"
#include "StateMachine.h"
#include "SoccerField.h"

Attacking* Attacking::Instance()
{
	static Attacking instance;

	return &instance;
}

void Attacking::Enter(ASoccerTeam& team)
{
	team.SetAllPlayersToAttackMode();

	team.UpdateTargetsOfWaitingPlayers();

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("Entering Attack State"));
}

void Attacking::Execute(ASoccerTeam& team)
{
	if (!team.InControl())
	{
		team.GetStateMachine().ChangeState(*Defending::Instance());
	}
	else
	{
		team.Advance();
	}
}

void Attacking::Exit(ASoccerTeam& team)
{
	team.SetSupportingPlayer(nullptr);
}

Attacking::Attacking()
{
}

Defending* Defending::Instance()
{
	static Defending instance;

	return &instance;
}

void Defending::Enter(ASoccerTeam& team)
{
	team.SetAllPlayersToDefenseMode();

	team.UpdateTargetsOfWaitingPlayers();

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Entering Defense State"));
}

void Defending::Execute(ASoccerTeam& team)
{
	if (team.InControl())
	{
		team.GetStateMachine().ChangeState(*Attacking::Instance());
	}
	else
	{
		team.FallBack();
	}
}

void Defending::Exit(ASoccerTeam& team)
{
	team.SetSupportingPlayer(nullptr);
}

Defending::Defending()
{
}

PrepareForKickoff* PrepareForKickoff::Instance()
{
	static PrepareForKickoff instance;

	return &instance;
}

void PrepareForKickoff::Enter(ASoccerTeam& team)
{
	team.SetControllingPlayer(nullptr);
	team.SetSupportingPlayer(nullptr);
	team.SetReceiver(nullptr);
	team.SetPlayerClosestToBall(nullptr);

	team.SetAllPlayersToDefenseMode();
	team.ReturnAllFieldPlayersToHome();
	team.UpdateTargetsOfWaitingPlayers();

	team.FallBack();

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Entering PrepareForKickoff State"));
}

void PrepareForKickoff::Execute(ASoccerTeam& team)
{
	if (team.AllPlayersAtHome() && team.GetOpponentTeam()->AllPlayersAtHome())
	{
		team.GetStateMachine().ChangeState(*Defending::Instance());
	}
}

void PrepareForKickoff::Exit(ASoccerTeam& team)
{
	team.GetField()->SetGameOn(true);
}

PrepareForKickoff::PrepareForKickoff()
{
}