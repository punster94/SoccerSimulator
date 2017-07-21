#include "SoccerSimulator.h"
#include "SoccerBall.h"
#include "PlayerBase.h"

ASoccerBall::ASoccerBall()
{
	PrimaryActorTick.bCanEverTick = true;

	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));	
	RootComponent = BallMesh;

	BallMesh->OnComponentHit.AddDynamic(this, &ASoccerBall::OnComponentHit);

	BallFriction = -500.0f;
	TimeUntilStale = 1.5f;
}

void ASoccerBall::BeginPlay()
{
	Super::BeginPlay();

	Owner = nullptr;
	LastPosition = BallMesh->GetComponentLocation();
	BallMesh->GetPhysicsVolume()->FluidFriction = 0.0f;
	//BallMesh->GetMaterial(0)->GetPhysicalMaterial()->Friction;

	TimeSinceLastKick = 0.0f;
}

void ASoccerBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLastPosition();

	FVector friction = BallMesh->GetPhysicsLinearVelocity();
	FVector velocity = friction;
	friction.Z = 0.0f;
	friction.Normalize();
	friction *= DeltaTime * BallFriction;

	if (velocity.Size() > friction.Size())
	{
		BallMesh->SetPhysicsLinearVelocity(friction, true);
	}
	else
	{
		FVector newVelocity = FVector::ZeroVector;
		newVelocity.Z = velocity.Z;
		BallMesh->SetPhysicsLinearVelocity(newVelocity);
	}

	TimeSinceLastKick += DeltaTime;
}

void ASoccerBall::Kick(FVector Direction, float force)
{
	FVector velocity = BallMesh->GetPhysicsLinearVelocity();
	BallMesh->SetPhysicsLinearVelocity(FVector((0.0f, 0.0f, velocity.Z)));

	FVector direction = Direction;
	direction.Z = 0.0f;
	direction.Normalize();

	BallMesh->AddImpulse(direction * force);

	TimeSinceLastKick = 0.0f;
}

UPrimitiveComponent* ASoccerBall::GetObject()
{
	return BallMesh;
}

FVector ASoccerBall::GetLocation()
{
	FVector location = GetActorLocation();
	location.Z = 0.0f;

	return location;
}

float ASoccerBall::GetRadius()
{
	return BallMesh->GetCollisionShape().GetSphereRadius() * BallMesh->GetComponentScale().X;
}

float ASoccerBall::TimeToCoverDistance(FVector From, FVector To, float Force)
{
	float speed = Force / BallMesh->GetMass();

	float term = speed * speed + 2.0f * FVector::Distance(From, To) * BallFriction;

	if (term <= 0.05f * speed * speed)
	{
		return -1.0f;
	}

	return (FMath::Sqrt(term) - speed) / BallFriction;
}

FVector ASoccerBall::FuturePosition(float Time)
{
	FVector currentVelocity = BallMesh->GetPhysicsLinearVelocity();
	currentVelocity.Z = 0.0f;

	FVector frictionComponent = currentVelocity;
	frictionComponent.Normalize();

	frictionComponent *= BallFriction * 0.5f * Time * Time;

	return GetLocation() + (currentVelocity * Time) + frictionComponent;
}

void ASoccerBall::Trap(APlayerBase* Player)
{
	FVector previousVelocity = BallMesh->GetPhysicsLinearVelocity();
	BallMesh->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, previousVelocity.Z));

	Owner = Player;
}

FVector ASoccerBall::GetLastPosition()
{
	return LastPosition;
}

bool ASoccerBall::Stale()
{
	return TimeSinceLastKick >= TimeUntilStale;
}

void ASoccerBall::PlaceAtPosition(FVector Position)
{
	BallMesh->SetWorldLocation(Position);

	BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void ASoccerBall::OnComponentHit(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalVector, const FHitResult& Hit)
{
	
}

void ASoccerBall::UpdateLastPosition()
{
	LastPosition = BallMesh->GetComponentLocation();
}

void ASoccerBall::UpdateHeading()
{
	FVector currentDirection = BallMesh->GetPhysicsLinearVelocity();
	currentDirection.Z = 0.0f;

	if (currentDirection.Size() > 1.0f)
	{
		FRotator velocityRotation = currentDirection.Rotation();
		velocityRotation.Pitch = 0.0f;

		SetActorRotation(velocityRotation);
	}
}