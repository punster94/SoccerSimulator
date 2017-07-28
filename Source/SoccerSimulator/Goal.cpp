#include "SoccerSimulator.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "SoccerTeam.h"
#include "SoccerField.h"
#include "TeamStates.h"
#include "Enums.h"

AGoal::AGoal()
{
	PrimaryActorTick.bCanEverTick = true;

	ScoringArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Scoring Area"));

	RootComponent = ScoringArea;

	ScoringArea->OnComponentBeginOverlap.AddDynamic(this, &AGoal::ScoreAreaOverlapped);
}

void AGoal::BeginPlay()
{
	Super::BeginPlay();

	GoalsScored = 0;
}

void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AGoal::GetLocation()
{
	FVector location = GetActorLocation();
	location.Z = 0.0f;

	return location;
}

FVector AGoal::Facing()
{
	FVector facing = ScoringArea->GetRightVector();
	facing.Normalize();

	return facing;
}

float AGoal::Left()
{
	return ScoringArea->GetComponentLocation().Y - ScoringArea->GetScaledBoxExtent().X;
}

float AGoal::Right()
{
	return ScoringArea->GetComponentLocation().Y + ScoringArea->GetScaledBoxExtent().X;
}

void AGoal::SetTeam(ASoccerTeam& AssociatedTeam)
{
	Team = &AssociatedTeam;
}

int32 AGoal::GetScore()
{
	return GoalsScored;
}

void AGoal::ScoreAreaOverlapped(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASoccerBall* ball = Cast<ASoccerBall>(OtherActor);

	if (ball != nullptr)
	{
		Team->GetOpponentTeam()->GetGoal()->GoalsScored++;

		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, "Goal Scored");

		ResetGame();

		Team->GetField()->OnGoal(ball->GetLocation());
	}
}

void AGoal::ResetGame()
{
	FVector fieldLocation = Team->GetField()->GetLocation();

	Team->GetBall()->PlaceAtPosition(ResetBallOffset + fieldLocation);

	Team->GetStateMachine().ChangeState(*PrepareForKickoff::Instance());
	Team->GetOpponentTeam()->GetStateMachine().ChangeState(*PrepareForKickoff::Instance());

	Team->GetField()->SetGameOn(false);
}
