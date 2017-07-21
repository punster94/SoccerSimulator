#include "SoccerSimulator.h"
#include "GoalKeeper.h"
#include "GoalKeeperStates.h"
#include "SteeringBehavior.h"
#include "SoccerTeam.h"
#include "SoccerBall.h"
#include "SoccerField.h"
#include "FieldPlayer.h"
#include "TeamStates.h"

GlobalKeeperState* GlobalKeeperState::Instance()
{
	static GlobalKeeperState instance;

	return &instance;
}

void GlobalKeeperState::Enter(AGoalKeeper& keeper)
{
}

void GlobalKeeperState::Execute(AGoalKeeper& keeper)
{
	if (!keeper.GetTeam()->GetField()->GameOn() &&
		&keeper.GetStateMachine().GetCurrentState() != ReturnHome::Instance())
	{
		keeper.GetStateMachine().ChangeState(*ReturnHome::Instance());
	}
}

void GlobalKeeperState::Exit(AGoalKeeper& keeper)
{
}

TendGoal* TendGoal::Instance()
{
	static TendGoal instance;

	return &instance;
}

void TendGoal::Enter(AGoalKeeper& keeper)
{
	keeper.GetSteering()->ArriveOn();
	keeper.SetRearInterposeTargetPosition();
}

void TendGoal::Execute(AGoalKeeper& keeper)
{
	keeper.SetRearInterposeTargetPosition();

	if (keeper.BallWithinKeeperRange())
	{
		keeper.GetTeam()->GetBall()->Trap(&keeper);
		keeper.GetTeam()->SetControllingPlayer(&keeper);
		keeper.GetTeam()->GetField()->SetGoalKeeperHasBall(true);
		keeper.GetStateMachine().ChangeState(*PutBallBackInPlay::Instance());

		return;
	}

	if (keeper.BallWithinRangeForIntercept() &&
		keeper.IsClosestTeamMemberToBall())
	{
		keeper.GetStateMachine().ChangeState(*InterceptBall::Instance());
	}
}

void TendGoal::Exit(AGoalKeeper& keeper)
{
	keeper.GetSteering()->ArriveOff();
}

ReturnHome* ReturnHome::Instance()
{
	static ReturnHome instance;

	return &instance;
}

void ReturnHome::Enter(AGoalKeeper& keeper)
{
	keeper.GetSteering()->ArriveOn();
}

void ReturnHome::Execute(AGoalKeeper& keeper)
{
	keeper.ReturnHome();

	if (keeper.InHomeRegion() &&
		keeper.GetTeam()->GetField()->GameOn())
	{
		keeper.GetStateMachine().ChangeState(*TendGoal::Instance());
	}
}

void ReturnHome::Exit(AGoalKeeper& keeper)
{
	keeper.GetSteering()->ArriveOff();
}

PutBallBackInPlay* PutBallBackInPlay::Instance()
{
	static PutBallBackInPlay instance;

	return &instance;
}

void PutBallBackInPlay::Enter(AGoalKeeper& keeper)
{
	keeper.GetTeam()->SetControllingPlayer(&keeper);

	keeper.GetTeam()->GetOpponentTeam()->ReturnAllFieldPlayersToHome();
	keeper.GetTeam()->ReturnAllFieldPlayersToHome();
}

void PutBallBackInPlay::Execute(AGoalKeeper& keeper)
{
	APlayerBase* receiver = nullptr;
	FVector ballTarget;

	if (keeper.GetTeam()->FindPass(keeper, receiver, ballTarget,
		keeper.MaxPassingStrength,
		keeper.MinPassDistance))
	{
		FVector toTarget = ballTarget - keeper.GetTeam()->GetBall()->GetLocation();
		toTarget.Normalize();

		keeper.GetTeam()->GetBall()->Kick(toTarget, keeper.MaxPassingStrength);

		keeper.GetTeam()->GetField()->SetGoalKeeperHasBall(false);

		AFieldPlayer* receiving = Cast<AFieldPlayer>(receiver);

		if (receiving != nullptr)
		{
			receiving->MessageReceiveBall(ballTarget);
		}

		keeper.GetStateMachine().ChangeState(*TendGoal::Instance());

		return;
	}

	keeper.ResetVelocity();
}

void PutBallBackInPlay::Exit(AGoalKeeper& keeper)
{
}

InterceptBall* InterceptBall::Instance()
{
	static InterceptBall instance;

	return &instance;
}

void InterceptBall::Enter(AGoalKeeper& keeper)
{
	keeper.GetSteering()->PursuitOn();
	keeper.GetSteering()->SetTargetObject(keeper.GetTeam()->GetBall()->GetObject());
}

void InterceptBall::Execute(AGoalKeeper& keeper)
{
	if ((keeper.TooFarFromGoalMouth() &&
		!keeper.IsClosestPlayerOnPitchToBall()) ||
		keeper.OutsideInterceptionRange())
	{
		keeper.GetStateMachine().ChangeState(*ReturnHome::Instance());

		return;
	}

	if (keeper.BallWithinKeeperRange())
	{
		if (keeper.Tackleability < FMath::FRand())
		{
			keeper.GetTeam()->GetBall()->Trap(&keeper);
			keeper.GetTeam()->SetControllingPlayer(&keeper);
			keeper.GetTeam()->GetField()->SetGoalKeeperHasBall(true);
			keeper.GetStateMachine().ChangeState(*PutBallBackInPlay::Instance());
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Recovering From Missed Tackle: %f"), keeper.Tackleability));
			keeper.GetStateMachine().ChangeState(*Fall::Instance());
		}

		return;
	}
}

void InterceptBall::Exit(AGoalKeeper& keeper)
{
	keeper.GetSteering()->PursuitOff();
}

Fall* Fall::Instance()
{
	static Fall instance;

	return &instance;
}

void Fall::Enter(AGoalKeeper& keeper)
{
	keeper.BeginRecoveryTimer();
}

void Fall::Execute(AGoalKeeper& keeper)
{
	if (keeper.RecoveryComplete())
	{
		keeper.GetStateMachine().ChangeState(*TendGoal::Instance());
	}
	else
	{
		keeper.ResetVelocity();
	}
}

void Fall::Exit(AGoalKeeper& keeper)
{
}