#include "SoccerSimulator.h"
#include "GoalKeeper.h"
#include "GoalKeeperStates.h"
#include "StateMachine.h"
#include "SteeringBehavior.h"
#include "SoccerTeam.h"
#include "SoccerBall.h"
#include "SoccerField.h"
#include "FieldPlayer.h"
#include "Goal.h"

AGoalKeeper::AGoalKeeper() :
	APlayerBase()
{
	KeeperStateMachine = new StateMachine<AGoalKeeper>(*this);

	InterposeTarget = CreateDefaultSubobject<USphereComponent>("InterposeTarget");
	
	InterposeTarget->SetupAttachment(RootComponent);
	InterposeTarget->SetSphereRadius(0.0f);
	InterposeTarget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InterceptRange = 600.0f;
	MinPassDistance = 1800.0f;
	MaxInterceptRange = 600.0f;
	KeeperRadius = 100.0f;
}

AGoalKeeper::~AGoalKeeper()
{
	delete KeeperStateMachine;
}

void AGoalKeeper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetTeam()->GetField()->Paused())
	{
		KeeperStateMachine->Update();

		if (!GetTeam()->GetField()->GoalKeeperHasBall())
		{
			TrackBall();
			LookAt = GetHeading();
		}
	}
}

void AGoalKeeper::SetTeam(ASoccerTeam* TeamReference)
{
	Super::SetTeam(TeamReference);

	KeeperStateMachine->SetCurrentState(*ReturnHome::Instance());
	KeeperStateMachine->SetPreviousState(*ReturnHome::Instance());
	KeeperStateMachine->SetGlobalState(GlobalKeeperState::Instance());

	KeeperStateMachine->GetCurrentState().Enter(*this);
	GetSteering()->SetTargetObject(TeamReference->GetBall()->GetObject());
}

StateMachine<AGoalKeeper>& AGoalKeeper::GetStateMachine()
{
	return *KeeperStateMachine;
}

void AGoalKeeper::SetRearInterposeTargetPosition()
{
	FVector position = GetTeam()->GetGoal()->GetLocation();

	float goalWidth = GetTeam()->GetGoal()->ScoringArea->GetScaledBoxExtent().X;

	position.Y = GetTeam()->GetField()->GetLocation().Y;
	position.Y += goalWidth * (GetTeam()->GetBall()->GetLocation().Y - GetTeam()->GetField()->GetLocation().Y) / (GetTeam()->GetField()->RegionScaledBounds().Y * 2.0f);

	FVector toBall = GetTeam()->GetBall()->GetLocation() - position;
	toBall.Normalize();
	toBall *= goalWidth;

	position += toBall;

	GetSteering()->SetTargetLocation(position);
	InterposeTarget->SetWorldLocation(position);

	//GetSteering()->SetOtherTargetObject(InterposeTarget);
	//GetSteering()->SetTargetObject(InterposeTarget);

	if (GetTeam()->DrawDebugBoxes())
	{
		FVector extent(20.0f, 20.0f, 20.0f);
		FVector offset(0.0f, 0.0f, 500.0f);
		DrawDebugBox(GetWorld(), position + offset, extent, FColor::Green, false, 1.0f);
	}
}

bool AGoalKeeper::BallWithinRangeForIntercept()
{
	return FVector::Dist(GetTeam()->GetGoal()->GetLocation(), GetTeam()->GetBall()->GetLocation()) <=
		InterceptRange;
}

bool AGoalKeeper::TooFarFromGoalMouth()
{
	return FVector::Dist(GetLocation(), GetTeam()->GetGoal()->GetLocation()) > InterceptRange;
}

bool AGoalKeeper::OutsideInterceptionRange()
{
	return FVector::Dist(GetLocation(), GetTeam()->GetGoal()->GetLocation()) > MaxInterceptRange;
}

float AGoalKeeper::ReachSpeed()
{
	return 0.0f;
}