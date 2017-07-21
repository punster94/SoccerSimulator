#include "SoccerSimulator.h"
#include "SoccerField.h"

ASoccerField::ASoccerField()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SupportArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SupportArea"));
	PlayArea = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayArea"));
	RegionDefinitionArea = CreateDefaultSubobject<UBoxComponent>(TEXT("RegionDefinitionArea"));
	SoccerBallComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("SoccerBall"));
	BlueGoalComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("BlueGoal"));
	RedGoalComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("RedGoal"));
	BlueTeamComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("BlueTeam"));
	RedTeamComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("RedTeam"));

	RootComponent = DefaultComponent;

	SupportArea->SetupAttachment(RootComponent);
	PlayArea->SetupAttachment(RootComponent);
	RegionDefinitionArea->SetupAttachment(RootComponent);
	SoccerBallComponent->SetupAttachment(RootComponent);
	BlueGoalComponent->SetupAttachment(RootComponent);
	RedGoalComponent->SetupAttachment(RootComponent);
	BlueTeamComponent->SetupAttachment(RootComponent);
	RedTeamComponent->SetupAttachment(RootComponent);

	bPaused = false;
	bGameOn = false;
	bGoalKeeperHasBall = false;
}

void ASoccerField::BeginPlay()
{
	Super::BeginPlay();

	InitialPosition = GetPlayAreaLocation();
	InitialBounds = PlayAreaScaledBounds();
	InitialUnscaledBounds = PlayArea->GetUnscaledBoxExtent();

	MaximumPlayAreaDiagonal = CalculateXYDiagonal(InitialUnscaledBounds);
	CurrentPlayAreaDiagonalRatio = 1.0f;

	SoccerBall = Cast<ASoccerBall>(SoccerBallComponent->GetChildActor());

	BlueGoal = Cast<AGoal>(BlueGoalComponent->GetChildActor());
	RedGoal = Cast<AGoal>(RedGoalComponent->GetChildActor());

	BlueTeam = Cast<ASoccerTeam>(BlueTeamComponent->GetChildActor());
	RedTeam = Cast<ASoccerTeam>(RedTeamComponent->GetChildActor());

	BlueTeam->SetOpponentTeam(*RedTeam, *BlueGoal, *RedGoal, *SoccerBall, *this);
	RedTeam->SetOpponentTeam(*BlueTeam, *RedGoal, *BlueGoal, *SoccerBall, *this);

	BlueTeam->SetUpTeamReferences();
	RedTeam->SetUpTeamReferences();
}

void ASoccerField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayArea();
}

bool ASoccerField::Paused()
{
	return bPaused;
}

bool ASoccerField::GameOn()
{
	return bGameOn;
}

bool ASoccerField::GoalKeeperHasBall()
{
	return bGoalKeeperHasBall;
}

FVector ASoccerField::SupportAreaScaledBounds()
{
	return SupportArea->GetScaledBoxExtent();
}

FVector ASoccerField::PlayAreaScaledBounds()
{
	return PlayArea->GetScaledBoxExtent();
}

FVector ASoccerField::RegionScaledBounds()
{
	return RegionDefinitionArea->GetScaledBoxExtent();
}

FVector ASoccerField::GetLocation()
{
	FVector location = RegionDefinitionArea->GetComponentLocation();
	location.Z = 0.0f;

	return location;
}

FVector ASoccerField::GetPlayAreaLocation()
{
	FVector location = PlayArea->GetComponentLocation();
	location.Z = 0.0f;

	return location;
}

void ASoccerField::SetGameOn(bool bOn)
{
	bGameOn = bOn;
}

FVector ASoccerField::GetPositionOfRegionID(int Region)
{
	float regionX = RegionDefinitionArea->GetScaledBoxExtent().X * 2.0f / XRegions;
	float regionY = RegionDefinitionArea->GetScaledBoxExtent().Y * 2.0f / YRegions;

	FVector position = GetLocation();

	int XIndex = Region % XRegions;
	int YIndex = Region / XRegions;

	position.X += (regionX * XIndex) - RegionDefinitionArea->GetScaledBoxExtent().X + (regionX / 2.0f);
	position.Y += (regionY * YIndex) - RegionDefinitionArea->GetScaledBoxExtent().Y + (regionY / 2.0f);

	return position;
}

ASoccerBall* ASoccerField::GetBall()
{
	return SoccerBall;
}

bool ASoccerField::IsPositionInPlayArea(FVector Position)
{
	FVector area = RegionDefinitionArea->GetScaledBoxExtent();
	FVector location = RegionDefinitionArea->GetComponentLocation();

	if (Position.X > (location.X + area.X) || Position.X < (location.X - area.X))
	{
		return false;
	}

	return (Position.Y <= (location.Y + area.Y) && Position.Y >= (location.Y - area.Y));
}

void ASoccerField::SetGoalKeeperHasBall(bool HasBall)
{
	bGoalKeeperHasBall = HasBall;
}

float ASoccerField::CurrentDiagonalRatio()
{
	return CurrentPlayAreaDiagonalRatio;
}

float ASoccerField::CalculateXYDiagonal(FVector Extent)
{
	return FMath::Sqrt(Extent.X * Extent.X + Extent.Y * Extent.Y);
}

void ASoccerField::UpdatePlayArea()
{
	FVector ballLocation = SoccerBall->GetLocation();
	FVector playAreaLocation = GetPlayAreaLocation();

	playAreaLocation.Y = ballLocation.Y;
	playAreaLocation = (playAreaLocation + InitialPosition) / 2.0f;

	float yOffsetFromInitial = FMath::Abs(InitialPosition.Y - playAreaLocation.Y);
	float yOffsetRatio = yOffsetFromInitial / InitialBounds.Y;
	yOffsetRatio = 1 - yOffsetRatio;

	FVector newBounds = InitialUnscaledBounds;
	newBounds.Y *= yOffsetRatio;

	//newBounds = (newBounds + InitialUnscaledBounds) / 2.0f;

	CurrentPlayAreaDiagonalRatio = CalculateXYDiagonal(newBounds) / MaximumPlayAreaDiagonal;

	PlayArea->SetBoxExtent(newBounds, false);
	PlayArea->SetWorldLocation(playAreaLocation, false, nullptr, ETeleportType::TeleportPhysics);
}