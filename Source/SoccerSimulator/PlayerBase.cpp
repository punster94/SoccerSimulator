#include "SoccerSimulator.h"
#include "PlayerBase.h"
#include "SteeringBehavior.h"
#include "SoccerTeam.h"
#include "SoccerField.h"
#include "GoalKeeper.h"
#include "FieldPlayer.h"

APlayerBase::APlayerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	DirectionComponent = CreateDefaultSubobject<UStaticMeshComponent>("Direction Component");
	NeighborhoodComponent = CreateDefaultSubobject<USphereComponent>("Neighborhood Component");

	RootComponent = CapsuleComponent;
	
	MeshComponent->SetupAttachment(RootComponent);
	DirectionComponent->SetupAttachment(MeshComponent);
	NeighborhoodComponent->SetupAttachment(RootComponent);

	Neighbors.Empty();

	NeighborhoodComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::NeighborhoodOverlap);
	NeighborhoodComponent->OnComponentEndOverlap.AddDynamic(this, &APlayerBase::NeighborhoodEndOverlap);
	
	MaximumSpeed = 1000.0f;
	MaxShootingStrength = 1800.0f;
	ChancePlayerAttemptsPotShot = 0.1f;
	MaxPassingStrength = 1500.0f;
	MinPassingDistance = 600.0f;
	DirectionChangeDribbleStrength = 1000.0f;
	MaxDribbleStrength = 1200.0f;
	KickingAccuracy = 0.95f;
	PassThreatRadius = 300.0f;
	ChanceOfUsingArriveTypeReceiveBehavior = 1.0f;
	ReceivingRadius = 850.0f;
	ComfortRadius = 600.0f;

	MaxStamina = 100.0f;
	CurrentStamina = 100.0f;
	StaminaRegenerationRate = 10.0f;
	MinimumSpeed = 300.0f;
	CurrentSpeed = 1000.0f;
	MinimumSpeedThreshold = 30.0f;
	MaximumSpeedThreshold = 80.0f;
	StaminaRegenerationThreshold = 55.0f;
	StaminaCost = 0.002f;

	Tackleability = 0.5f;
	RecoveryTime = 0.5f;

	MovementChangeAlpha = 0.2f;

	CurrentStaminaState = EStaminaState::Normal;

	Steering = new SteeringBehavior(this);
}

APlayerBase::~APlayerBase()
{
	delete Steering;
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStamina(DeltaTime);
	UpdateSpeed();
	Steer(DeltaTime);
}

bool APlayerBase::IsThreatened()
{
	return Team->IsPlayerThreatened(*this);
}

bool APlayerBase::BallWithinKeeperRange()
{
	return FVector::Dist(GetLocation(), GetTeam()->GetBall()->GetLocation()) < GetTeam()->Goalie->KeeperRadius;
}

bool APlayerBase::BallWithinKickingRange()
{
	return FVector::Dist(GetLocation(), GetTeam()->GetBall()->GetLocation()) < GetTeam()->GetKickRadius();
}

bool APlayerBase::BallWithinReceivingRange()
{
	FVector GoalToBall = GetTeam()->GetBall()->GetLocation() - GetTeam()->GetGoal()->GetLocation();
	FVector GoalToPlayer = GetLocation() - GetTeam()->GetGoal()->GetLocation();

	return FVector::Dist(GetLocation(), GetTeam()->GetBall()->GetLocation()) < ReceivingRadius &&
		GoalToBall.Size() > GoalToPlayer.Size();
}

bool APlayerBase::InHomeRegion()
{
	return FVector::Dist(GetLocation(), HomeRegion) <= GetRadius();
}

bool APlayerBase::IsClosestTeamMemberToBall()
{
	return Team->GetPlayerClosestToBall() == this;
}

bool APlayerBase::IsClosestPlayerOnPitchToBall()
{
	return IsClosestTeamMemberToBall() &&
		FVector::Dist(GetLocation(), GetTeam()->GetBall()->GetLocation()) <
		FVector::Dist(GetTeam()->GetOpponentTeam()->GetPlayerClosestToBall()->GetLocation(), GetTeam()->GetBall()->GetLocation());
}

bool APlayerBase::IsClosestNonGoalieOnTeamToBall()
{
	return Team->GetClosestNonGoalieToBall() == this;
}

bool APlayerBase::AtTarget()
{
	return FVector::Dist(GetLocation(), Steering->GetTargetLocation()) <= GetRadius();
}

bool APlayerBase::IsControllingPlayer()
{
	return Team->GetControllingPlayer() == this;
}

bool APlayerBase::IsAheadOfAttacker()
{
	if (Team->GetControllingPlayer() == nullptr)
	{
		return false;
	}

	return FMath::Abs(GetLocation().X - Team->GetOpponentTeam()->GetGoal()->GetLocation().X) <
		FMath::Abs(Team->GetControllingPlayer()->GetLocation().X - Team->GetOpponentTeam()->GetGoal()->GetLocation().X);
}

bool APlayerBase::IsReadyForNextKick()
{
	if (!GetWorldTimerManager().IsTimerActive(KickTimer))
	{
		GetWorldTimerManager().SetTimer(KickTimer, this, &APlayerBase::SetKickReady, GetTeam()->GetSecondsPerKick(), false);

		return true;
	}

	return false;
}

bool APlayerBase::InHotRegion()
{
	return FMath::Abs(GetLocation().Y - Team->GetField()->GetLocation().Y) <
		Team->GetField()->RegionScaledBounds().Y * 0.5f;
}

void APlayerBase::ReturnHome()
{
	Steering->SetTargetLocation(HomeRegion);
}

FVector APlayerBase::GetVelocity()
{
	FVector velocity = CapsuleComponent->GetPhysicsLinearVelocity();
	velocity.Z = 0.0f;

	return velocity;
}

FVector APlayerBase::GetLocation()
{
	FVector location = GetActorLocation();
	location.Z = 0.0f;
	
	return location;
}

FVector APlayerBase::GetHeading()
{
	FVector heading = GetActorForwardVector();
	heading.Normalize();

	return heading;
}

float APlayerBase::GetRadius()
{
	return CapsuleComponent->GetScaledCapsuleRadius();
}

TArray<UPrimitiveComponent*> APlayerBase::GetNeighbors()
{
	return Neighbors;
}

UPrimitiveComponent* APlayerBase::GetObject()
{
	return CapsuleComponent;
}

void APlayerBase::Seek(FVector Position)
{
	Steering->ArriveOff();
	Steering->PursuitOff();
	Steering->SeparationOff();
	Steering->InterposeOff();

	Steering->SeekOn();
	Steering->SetTargetLocation(Position);
}

void APlayerBase::Arrive(FVector Position, EDecelerationType Deceleration)
{
	Steering->SeekOff();
	Steering->PursuitOff();
	Steering->SeparationOff();
	Steering->InterposeOff();

	Steering->ArriveOn();
	Steering->SetTargetLocation(Position);
	Steering->SetDecelerationType(Deceleration);
}

void APlayerBase::Pursue(UPrimitiveComponent* Object)
{
	Steering->SeekOff();
	Steering->ArriveOff();
	Steering->SeparationOff();
	Steering->InterposeOff();

	Steering->PursuitOn();
	Steering->SetTargetObject(Object);
}

void APlayerBase::Separate()
{
	Steering->SeekOff();
	Steering->ArriveOff();
	Steering->PursuitOff();
	Steering->InterposeOff();

	Steering->SeparationOn();
}

void APlayerBase::Interpose(UPrimitiveComponent* ObjectA, UPrimitiveComponent* ObjectB)
{
	Steering->SeekOff();
	Steering->ArriveOff();
	Steering->PursuitOff();
	Steering->SeparationOff();

	Steering->InterposeOn();
	Steering->SetTargetObject(ObjectA);
	Steering->SetOtherTargetObject(ObjectB);
}

SteeringBehavior* APlayerBase::GetSteering()
{
	return Steering;
}

void APlayerBase::SetHomeRegion(int Region)
{
	AFieldPlayer* fieldPlayer = Cast<AFieldPlayer>(this);

	if (fieldPlayer != nullptr)
	{
		HomeRegion = Team->CalculatePlayerHomeRegion(fieldPlayer);
	}
	else
	{
		HomeRegion = Team->GetField()->GetPositionOfRegionID(Region);
	}

	OffsetFromTeam = HomeRegion - Team->GetLocation();
}

void APlayerBase::SetInitialHomeRegion()
{
	SetHomeRegion(StartingRegion);
}

void APlayerBase::SetAttackRegion()
{
	SetHomeRegion(AttackingRegion);
}

void APlayerBase::SetTeam(ASoccerTeam* TeamReference)
{
	Team = TeamReference;

	SetInitialHomeRegion();
}

ASoccerTeam* APlayerBase::GetTeam()
{
	return Team;
}

void APlayerBase::ResetVelocity()
{
	FVector targetVelocity = FVector::ZeroVector;
	targetVelocity.Z = CapsuleComponent->GetPhysicsLinearVelocity().Z;
	
	CapsuleComponent->SetPhysicsLinearVelocity(targetVelocity);
}

void APlayerBase::TrackBall()
{
	FVector toBall = Team->GetBall()->GetLocation() - GetLocation();
	FRotator rotation = toBall.Rotation();

	rotation.Pitch = 0.0f;

	SetActorRotation(rotation);
}

void APlayerBase::TrackTarget()
{
	FVector toTarget = Steering->GetTargetLocation() - GetLocation();
	FRotator rotation = toTarget.Rotation();

	rotation.Pitch = 0.0f;

	SetActorRotation(rotation);
}

FVector APlayerBase::AddNoiseToKick(FVector BallLocation, FVector BallTarget)
{
	float displacement = (180.0f - 180.0f * KickingAccuracy) * (FMath::FRand() - FMath::FRand());

	FVector toTarget = BallTarget - BallLocation;

	toTarget.RotateAngleAxis(displacement, FVector(0.0f, 0.0f, 1.0f));

	return toTarget + BallLocation;
}

bool APlayerBase::PositionInFrontOfPlayer(FVector Position)
{
	FVector toPosition = Position - GetLocation();

	return FVector::DotProduct(toPosition, GetHeading()) > 0.0f;
}

void APlayerBase::BeginRecoveryTimer()
{
	GetWorldTimerManager().SetTimer(RecoveryTimer, RecoveryTime, false);
}

bool APlayerBase::RecoveryComplete()
{
	return !GetWorldTimerManager().IsTimerActive(RecoveryTimer);
}

void APlayerBase::TargetHome()
{
	AFieldPlayer* fieldPlayer = Cast<AFieldPlayer>(this);

	if (fieldPlayer != nullptr)
	{
		//HomeRegion = Team->GetLocation() + OffsetFromTeam;
		HomeRegion = Team->CalculatePlayerHomeRegion(fieldPlayer);
	}

	Steering->SetTargetLocation(HomeRegion);
}

void APlayerBase::Steer(float DeltaTime)
{
	FVector destinationVelocity = Steering->Calculate(DeltaTime);
	
	if (destinationVelocity.Size() > CurrentSpeed)
	{
		destinationVelocity.Normalize();
		destinationVelocity *= CurrentSpeed;
	}

	destinationVelocity.Z = CapsuleComponent->GetPhysicsLinearVelocity().Z;

	destinationVelocity = FMath::Lerp(CapsuleComponent->GetPhysicsLinearVelocity(), destinationVelocity, MovementChangeAlpha);

	CapsuleComponent->SetPhysicsLinearVelocity(destinationVelocity);
}

void APlayerBase::SetKickReady()
{
}

void APlayerBase::UpdateStamina(float DeltaTime)
{
	float Cost = GetVelocity().Size() * DeltaTime * StaminaCost;
	float Recovery = StaminaRegenerationRate * DeltaTime;

	CurrentStamina = FMath::Clamp(CurrentStamina - Cost + Recovery, 0.0f, MaxStamina);

	if (CurrentStaminaState == EStaminaState::Normal &&
		CurrentStamina <= MinimumSpeedThreshold)
	{
		CurrentStaminaState = EStaminaState::Regenerating;
	}

	if (CurrentStaminaState == EStaminaState::Regenerating &&
		CurrentStamina >= StaminaRegenerationThreshold)
	{
		CurrentStaminaState = EStaminaState::Normal;
	}
}

void APlayerBase::UpdateSpeed()
{
	switch (CurrentStaminaState)
	{
	case EStaminaState::Normal:
		CurrentSpeed = MaximumSpeed;

		if (CurrentStamina <= MaximumSpeedThreshold)
		{
			//TODO this could be better
			CurrentSpeed *= CurrentStamina / MaxStamina;
			CurrentSpeed = FMath::Max(CurrentSpeed, MinimumSpeed);
		}

		break;

	case EStaminaState::Regenerating:
		CurrentSpeed = MinimumSpeed;

		break;
	}
}

void APlayerBase::NeighborhoodOverlap(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerBase* otherPlayer = Cast<APlayerBase>(OtherActor);

	if (otherPlayer != nullptr && otherPlayer->GetObject() == OtherComponent)
	{
		if (!Neighbors.Contains(otherPlayer->GetObject()))
		{
			Neighbors.Add(otherPlayer->GetObject());
		}

		if (!otherPlayer->Neighbors.Contains(GetObject()))
		{
			otherPlayer->Neighbors.Add(GetObject());
		}
	}
}

void APlayerBase::NeighborhoodEndOverlap(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	APlayerBase* otherPlayer = Cast<APlayerBase>(OtherActor);

	if (otherPlayer != nullptr && otherPlayer->GetObject() == OtherComponent)
	{
		Neighbors.Remove(OtherComponent);
		otherPlayer->Neighbors.Remove(GetObject());
	}
}