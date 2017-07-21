#include "SoccerSimulator.h"
#include "FieldPlayer.h"
#include "StateMachine.h"
#include "FieldPlayerStates.h"
#include "SteeringBehavior.h"
#include "SoccerTeam.h"
#include "SoccerBall.h"
#include "SoccerField.h"

AFieldPlayer::AFieldPlayer() :
	APlayerBase()
{
	PlayerStateMachine = new StateMachine<AFieldPlayer>(*this);
}

AFieldPlayer::~AFieldPlayer()
{
	delete PlayerStateMachine;
}

void AFieldPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetTeam()->GetField()->Paused())
	{
		PlayerStateMachine->Update();
	}
}

void AFieldPlayer::SetTeam(ASoccerTeam* TeamReference)
{
	Super::SetTeam(TeamReference);

	PlayerStateMachine->SetCurrentState(*Wait::Instance());
	PlayerStateMachine->SetPreviousState(*Wait::Instance());
	PlayerStateMachine->SetGlobalState(GlobalPlayerState::Instance());

	PlayerStateMachine->GetCurrentState().Enter(*this);
	GetSteering()->SeparationOn();
}

StateMachine<AFieldPlayer>& AFieldPlayer::GetStateMachine()
{
	return *PlayerStateMachine;
}

void AFieldPlayer::MessageSupportAttacker()
{
	if (!PlayerStateMachine->IsInState(*SupportAttacker::Instance()))
	{
		GetSteering()->SetTargetLocation(GetTeam()->GetSupportSpot());

		PlayerStateMachine->ChangeState(*SupportAttacker::Instance());
	}
}

void AFieldPlayer::MessageGoHome()
{
	PlayerStateMachine->ChangeState(*ReturnToHomeRegion::Instance());
}

void AFieldPlayer::MessageReceiveBall(FVector Position)
{
	FVector ballVelocity = GetTeam()->GetBall()->GetObject()->GetPhysicsLinearVelocity();
	ballVelocity.Z = 0.0f;
	ballVelocity.Normalize();

	FVector tangent = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), ballVelocity);

	if (tangent.Y * (Position.Y - GetTeam()->GetField()->GetLocation().Y) > 0.0f)
	{
		tangent *= -1.0f;
	}

	FVector offset = GetTeam()->GetReceivingOffset();

	//FVector normalizedOffset = offset;
	//normalizedOffset.Normalize();

	//tangent += normalizedOffset;
	//tangent /= 2.0f;
	tangent *= offset.Size();

	GetSteering()->SetTargetLocation(Position + tangent);

	PlayerStateMachine->ChangeState(*ReceiveBall::Instance());
}

void AFieldPlayer::MessagePassToMe(AFieldPlayer& Receiver)
{
	if (GetTeam()->GetReceiver() == nullptr &&
		BallWithinKickingRange())
	{
		GetTeam()->GetBall()->Kick(Receiver.GetLocation() - GetLocation(), MaxPassingStrength);

		Receiver.MessageReceiveBall(Receiver.GetLocation());

		PlayerStateMachine->ChangeState(*Wait::Instance());

		FindSupport();
	}
}

void AFieldPlayer::MessageWait()
{
	PlayerStateMachine->ChangeState(*Wait::Instance());
}

void AFieldPlayer::FindSupport()
{
	if (GetTeam()->GetSupportingPlayer() == nullptr)
	{
		AFieldPlayer* bestSupport = GetTeam()->DetermineBestSupportingAttacker();

		if (bestSupport != nullptr)
		{
			GetTeam()->SetSupportingPlayer(bestSupport);

			bestSupport->MessageSupportAttacker();
		}
	}
}