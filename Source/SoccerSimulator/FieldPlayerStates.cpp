#include "SoccerSimulator.h"
#include "FieldPlayer.h"
#include "StateMachine.h"
#include "FieldPlayerStates.h"
#include "SoccerBall.h"
#include "Goal.h"
#include "SoccerField.h"
#include "SoccerTeam.h"
#include "SteeringBehavior.h"
#include "GoalKeeper.h"
#include "Enums.h"

GlobalPlayerState* GlobalPlayerState::Instance()
{
	static GlobalPlayerState instance;

	return &instance;
}

void GlobalPlayerState::Enter(AFieldPlayer& player)
{
}

void GlobalPlayerState::Execute(AFieldPlayer& player)
{
	//if (player.BallWithinReceivingRange() &&
	//	player.IsControllingPlayer())
	//{
	//	player.SetSpeedWithBall();
	//}
	//else
	//{
	//	player.SetSpeedWithoutBall();
	//}
}

void GlobalPlayerState::Exit(AFieldPlayer& player)
{
}

Wait* Wait::Instance()
{
	static Wait instance;

	return &instance;
}

void Wait::Enter(AFieldPlayer& player)
{
	if (!player.GetTeam()->GetField()->GameOn())
	{
		player.ReturnHome();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Entering Wait State"));
}

void Wait::Execute(AFieldPlayer& player)
{
	player.TargetHome();

	// Ensure you are home first
	if (!player.AtTarget() &&
		(!player.IsClosestNonGoalieOnTeamToBall() ||
		!player.GetTeam()->GetField()->GameOn()))
	{
		player.GetSteering()->ArriveOn();

		player.TrackTarget();

		return;
	}
	else
	{
		player.GetSteering()->ArriveOff();
		player.ResetVelocity();
		player.TrackBall();
	}

	// If you are closest and the controller is far but there wasn't a pass
	if (player.GetTeam()->GetField()->GameOn() &&
		player.IsClosestNonGoalieOnTeamToBall() &&
		player.GetTeam()->ControllingPlayerTooFarFromBall() &&
		player.GetTeam()->GetReceiver() == nullptr &&
		!player.GetTeam()->GetField()->GoalKeeperHasBall())
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		AFieldPlayer* controller = Cast<AFieldPlayer>(player.GetTeam()->GetControllingPlayer());

		if (controller != nullptr)
		{
			controller->GetStateMachine().ChangeState(*Wait::Instance());
			player.GetTeam()->SetControllingPlayer(&player);
		}

		return;
	}

	// If you're in control and ahead of controller, ask to be passed to
	if (player.GetTeam()->InControl() &&
		!player.IsControllingPlayer())// &&
		//player.IsAheadOfAttacker())
	{
		player.GetTeam()->RequestPass(player);

		return;
	}

	// If you're the closest behind the ball but the ball hasn't been picked up by receiver
	if (player.GetTeam()->InControl() &&
		player.GetTeam()->GetReceiver() != nullptr &&
		player.GetTeam()->GetBall()->Stale() &&
		!player.GetTeam()->GetField()->GoalKeeperHasBall())
	{
		APlayerBase* behind = player.GetTeam()->GetClosestPlayerBehindBall();

		if (behind == &player ||
			(!behind->BehindBall() &&
			player.GetTeam()->GetClosestNonGoalieToBall() == &player))
		{
			player.GetStateMachine().ChangeState(*ReceiveBall::Instance());
			return;
		}
	}

	if (player.GetTeam()->GetField()->GameOn())
	{
		switch (player.PlayerRole)
		{

		case EPlayerRole::sweeper:
			player.GetStateMachine().ChangeState(*BlockBall::Instance());

			break;

		case EPlayerRole::forward:
			player.GetStateMachine().ChangeState(*FindAggressivePosition::Instance());

			break;
		}
	}
}

void Wait::Exit(AFieldPlayer& player)
{
}

ChaseBall* ChaseBall::Instance()
{
	static ChaseBall instance;

	return &instance;
}

void ChaseBall::Enter(AFieldPlayer& player)
{
	//player.GetSteering()->PursuitOn();
	//player.GetSteering()->SetTargetObject(player.GetTeam()->GetBall()->GetObject());
	player.GetSteering()->SeekOn();
}

void ChaseBall::Execute(AFieldPlayer& player)
{
	if (player.BallWithinKickingRange())
	{
		player.GetStateMachine().ChangeState(*KickBall::Instance());

		return;
	}

	if (player.IsClosestTeamMemberToBall())
	{
		// From before switching to pursuit over arrive
		player.GetSteering()->SetTargetLocation(player.GetTeam()->GetBall()->GetLocation());

		player.TrackTarget();

		return;
	}

	player.GetStateMachine().ChangeState(*ReturnToHomeRegion::Instance());
}

void ChaseBall::Exit(AFieldPlayer& player)
{
	//player.GetSteering()->PursuitOff();
	player.GetSteering()->SeekOff();
}

ReturnToHomeRegion* ReturnToHomeRegion::Instance()
{
	static ReturnToHomeRegion instance;

	return &instance;
}

void ReturnToHomeRegion::Enter(AFieldPlayer& player)
{
	player.GetSteering()->ArriveOn();

	if (!player.InHomeRegion())
	{
		player.ReturnHome();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, TEXT("Entering ReturnToHomeRegion State"));
}

void ReturnToHomeRegion::Execute(AFieldPlayer& player)
{
	if (player.GetTeam()->GetField()->GameOn() &&
		player.IsClosestTeamMemberToBall() &&
		player.GetTeam()->GetReceiver() == nullptr &&
		!player.GetTeam()->GetField()->GoalKeeperHasBall())
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		return;
	}

	if (player.GetTeam()->GetField()->GameOn() &&
		player.InHomeRegion())
	{
		player.GetSteering()->SetTargetLocation(player.GetLocation());
		player.GetStateMachine().ChangeState(*Wait::Instance());
	}
	else if (!player.GetTeam()->GetField()->GameOn() &&
		player.AtTarget())
	{
		if (player.GetTeam()->Losing())
		{
			player.PlayerRole = player.LosingRole;
		}
		else
		{
			player.PlayerRole = player.WinningRole;
		}

		player.GetStateMachine().ChangeState(*Wait::Instance());
	}
	else
	{
		player.TrackTarget();
	}
}

void ReturnToHomeRegion::Exit(AFieldPlayer& player)
{
	player.GetSteering()->ArriveOff();
}

KickBall* KickBall::Instance()
{
	static KickBall instance;

	return &instance;
}

void KickBall::Enter(AFieldPlayer& player)
{
	if (!player.IsReadyForNextKick())
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		return;
	}

	if (player.GetTeam()->GetControllingPlayer() != &player &&
		player.IsThreatened() &&
		player.Tackleability < FMath::FRand())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Recovering From Missed Tackle: %f"), player.Tackleability));
		player.GetStateMachine().ChangeState(*Recover::Instance());

		return;
	}

	player.GetTeam()->SetControllingPlayer(&player);
}

void KickBall::Execute(AFieldPlayer& player)
{
	FVector toBall = player.GetTeam()->GetBall()->GetLocation() - player.GetLocation();
	toBall.Normalize();

	float dot = FVector::DotProduct(player.GetHeading(), toBall);

	if (player.GetTeam()->GetReceiver() != nullptr ||
		player.GetTeam()->GetField()->GoalKeeperHasBall() ||
		dot < 0.0f)
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		return;
	}

	FVector ballTarget;

	float power = player.MaxShootingStrength * dot;

	if (player.GetTeam()->CanShoot(player.GetTeam()->GetBall()->GetLocation(), power, ballTarget))// ||
		//FMath::FRand() < player.ChancePlayerAttemptsPotShot)
	{
		ballTarget = player.AddNoiseToKick(player.GetTeam()->GetBall()->GetLocation(), ballTarget);
		FVector kickDirection = ballTarget - player.GetTeam()->GetBall()->GetLocation();

		player.GetTeam()->GetBall()->Kick(kickDirection, power);

		player.GetStateMachine().ChangeState(*Wait::Instance());

		player.FindSupport();

		return;
	}

	APlayerBase* receiver = nullptr;

	power = player.MaxPassingStrength * dot;

	if (player.IsThreatened() &&
		player.GetTeam()->FindPass(player, receiver, ballTarget, power, player.MinPassingDistance))
	{
		ballTarget += -1.0f * player.GetTeam()->GetReceivingOffset();
		ballTarget = player.AddNoiseToKick(player.GetTeam()->GetBall()->GetLocation(), ballTarget);

		FVector kickDirection = ballTarget - player.GetTeam()->GetBall()->GetLocation();

		player.GetTeam()->GetBall()->Kick(kickDirection, power);

		AFieldPlayer* receivingPlayer = Cast<AFieldPlayer>(receiver);
		
		if (receivingPlayer != nullptr)
		{
			receivingPlayer->MessageReceiveBall(ballTarget);
		}

		player.GetStateMachine().ChangeState(*Wait::Instance());

		player.FindSupport();
	}
	else
	{
		player.FindSupport();

		player.GetStateMachine().ChangeState(*Dribble::Instance());
	}
}

void KickBall::Exit(AFieldPlayer& player)
{
}

Dribble* Dribble::Instance()
{
	static Dribble instance;

	return &instance;
}

void Dribble::Enter(AFieldPlayer& player)
{
	player.GetTeam()->SetControllingPlayer(&player);
}

void Dribble::Execute(AFieldPlayer& player)
{
	float dot = FVector::DotProduct(player.GetTeam()->GetGoal()->Facing(), player.GetHeading());

	if (dot < 0)
	{
		FVector direction = player.GetHeading();

		float angle = -45.0f * RotationDirection(player.GetTeam()->GetGoal()->Facing(), player.GetHeading());

		direction = direction.RotateAngleAxis(angle, FVector(0.0f, 0.0f, 1.0f));

		if (!player.GetTeam()->GetField()->IsPositionInPlayArea(player.GetTeam()->GetBall()->GetLocation() + direction * 100.0f))
		{
			angle *= -1.0f;
			direction = direction.RotateAngleAxis(angle * 2.0f, FVector(0.0f, 0.0f, 1.0f));
		}

		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString::Printf(TEXT("Changing Direction With Dribble: %f %f %f"), direction.X, direction.Y, direction.Z));
		player.GetTeam()->GetBall()->Kick(direction, player.DirectionChangeDribbleStrength);
	}
	else
	{
		FVector direction = player.GetTeam()->GetOpponentTeam()->GetGoal()->GetLocation() - player.GetTeam()->GetBall()->GetLocation();

		if (direction.Size() > player.GetTeam()->GetDribbleDistance())
		{
			if (player.InHotRegion())
			{
				direction.Y *= -1.0f; // Opposite Direction 
			}
			else
			{
				direction.Y = 0.0f; // Straight Forward
			}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Dribbling Forward"));
		player.GetTeam()->GetBall()->Kick(direction, player.MaxDribbleStrength);
	}

	player.GetStateMachine().ChangeState(*ChaseBall::Instance());
}

void Dribble::Exit(AFieldPlayer& player)
{
}

float Dribble::RotationDirection(FVector V1, FVector V2)
{
	if (V1.Y * V2.X > V1.X * V2.Y)
	{
		return -1.0f;
	}

	return 1.0f;
}

SupportAttacker* SupportAttacker::Instance()
{
	static SupportAttacker instance;

	return &instance;
}

void SupportAttacker::Enter(AFieldPlayer& player)
{
	player.GetSteering()->ArriveOn();

	player.GetSteering()->SetTargetLocation(player.GetTeam()->GetSupportSpot());
}

void SupportAttacker::Execute(AFieldPlayer& player)
{
	if (!player.GetTeam()->InControl())
	{
		player.GetStateMachine().ChangeState(*ReturnToHomeRegion::Instance());

		return;
	}

	//if (player.GetTeam()->GetSupportingPlayer() != &player)
	//{
	//	player.GetStateMachine().ChangeState(*Wait::Instance());

	//	return;
	//}

	if (player.GetTeam()->GetSupportSpot() != player.GetSteering()->GetTargetLocation())
	{
		player.GetSteering()->SetTargetLocation(player.GetTeam()->GetSupportSpot());

		player.GetSteering()->ArriveOn();
	}

	FVector target;

	if (player.GetTeam()->CanShoot(player.GetLocation(), player.MaxShootingStrength, target))
	{
		player.GetTeam()->RequestPass(player);
	}

	if (player.AtTarget())
	{
		player.GetSteering()->ArriveOff();

		player.TrackBall();

		player.ResetVelocity();

		if (!player.IsThreatened())
		{
			player.GetTeam()->RequestPass(player);
		}

		return;
	}

	player.TrackTarget();

	/* NEW */
	if (player.IsClosestTeamMemberToBall())
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());
	}
}

void SupportAttacker::Exit(AFieldPlayer& player)
{
	player.GetTeam()->SetSupportingPlayer(nullptr);

	player.GetSteering()->ArriveOff();
}

ReceiveBall* ReceiveBall::Instance()
{
	static ReceiveBall instance;

	return &instance;
}

void ReceiveBall::Enter(AFieldPlayer& player)
{
	APlayerBase* receiver = dynamic_cast<APlayerBase*>(&player);
	player.GetTeam()->SetReceiver(receiver);

	player.GetTeam()->SetControllingPlayer(&player);

	if ((player.InHotRegion() ||
		FMath::FRand() < player.ChanceOfUsingArriveTypeReceiveBehavior) &&
		!player.GetTeam()->IsOpponentWithinRadius(player.GetLocation(), player.PassThreatRadius))
	{
		player.GetSteering()->ArriveOn();
	}
	else
	{
		player.GetSteering()->PursuitOn();
		player.GetSteering()->SetTargetObject(player.GetTeam()->GetBall()->GetObject());
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Entering ReceiveBall State"));
}

void ReceiveBall::Execute(AFieldPlayer& player)
{
	//TODO prevent player from running into ball if they are not behind the ball
	if (player.BallWithinReceivingRange() ||
		!player.GetTeam()->InControl())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, TEXT("Receiving Pass"));
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		return;
	}

	if (player.GetSteering()->PursuitIsOn())
	{
		player.GetSteering()->SetTargetLocation(player.GetTeam()->GetBall()->GetLocation());
	}

	if (player.AtTarget())
	{
		player.GetSteering()->ArriveOff();
		player.GetSteering()->PursuitOff();
		player.TrackBall();
		player.ResetVelocity();
	}
	else
	{
		player.TrackTarget();
	}

	if (player.GetTeam()->GetControllingPlayer() != &player)
	{
		player.GetStateMachine().ChangeState(*ReturnToHomeRegion::Instance());
	}
}

void ReceiveBall::Exit(AFieldPlayer& player)
{
	player.GetSteering()->ArriveOff();
	player.GetSteering()->PursuitOff();

	player.GetTeam()->SetReceiver(nullptr);
}

BlockBall* BlockBall::Instance()
{
	static BlockBall instance;

	return &instance;
}

void BlockBall::Enter(AFieldPlayer& player)
{
	player.GetSteering()->InterposeOn();
	player.GetSteering()->SetTargetObject(player.GetTeam()->Goalie->GetObject());
	player.GetSteering()->SetOtherTargetObject(player.GetTeam()->GetBall()->GetObject());
}

void BlockBall::Execute(AFieldPlayer& player)
{
	//if (player.GetTeam()->InControl())
	//{
	//	player.GetStateMachine().ChangeState(*Wait::Instance());

	//	return;
	//}

	if (player.IsClosestTeamMemberToBall())
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		return;
	}

	if (player.BallInSlice())
	{
		player.GetSteering()->SetOtherTargetObject(player.GetTeam()->GetBall()->GetObject());
	}
	else
	{
		player.SetInterposeLocation(player.GetHomeLocation());
	}
}

void BlockBall::Exit(AFieldPlayer& player)
{
	player.GetSteering()->InterposeOff();
}

Recover* Recover::Instance()
{
	static Recover instance;

	return &instance;
}

void Recover::Enter(AFieldPlayer& player)
{
	player.BeginRecoveryTimer();
}

void Recover::Execute(AFieldPlayer& player)
{
	if (player.RecoveryComplete())
	{
		player.GetStateMachine().ChangeState(*Wait::Instance());
	}
	else
	{
		player.ResetVelocity();
	}
}

void Recover::Exit(AFieldPlayer& player)
{
}

FindAggressivePosition* FindAggressivePosition::Instance()
{
	static FindAggressivePosition instance;

	return &instance;
}

void FindAggressivePosition::Enter(AFieldPlayer& player)
{
	player.GetSteering()->ArriveOn();

	if (!player.GetTeam()->InControl())
	{
		player.GetStateMachine().ChangeState(*Wait::Instance());
	}
}

void FindAggressivePosition::Execute(AFieldPlayer& player)
{
	// If the forward is the closest and also behind the ball, take it
	if (player.GetTeam()->GetClosestNonGoalieToBall() == &player &&
		player.GetTeam()->GetClosestPlayerBehindBall() == &player)
	{
		player.GetStateMachine().ChangeState(*ChaseBall::Instance());

		return;
	}

	// If the team is defending, go home
	if (!player.GetTeam()->InControl())
	{
		player.GetStateMachine().ChangeState(*Wait::Instance());

		return;
	}

	FVector aggressivePosition = player.GetHomeLocation();
	aggressivePosition.X = player.GetTeam()->GetBall()->GetLocation().X;

	player.GetSteering()->SetTargetLocation(aggressivePosition);
}

void FindAggressivePosition::Exit(AFieldPlayer& player)
{
	player.GetSteering()->ArriveOff();
}