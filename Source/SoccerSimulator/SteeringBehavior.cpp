#include "SoccerSimulator.h"
#include "SteeringBehavior.h"
#include "PlayerBase.h"
#include "SoccerTeam.h"
#include "Enums.h"

SteeringBehavior::SteeringBehavior(APlayerBase* Player) :
	PlayerBase(Player), TargetLocation(FVector::ZeroVector),
	DecelerationType(EDecelerationType::normal), TargetObject(nullptr),
	ShouldSeek(false), ShouldArrive(false), ShouldPursue(false),
	ShouldSeparate(false), ShouldInterpose(false)
{
}

SteeringBehavior::~SteeringBehavior()
{
}

FVector SteeringBehavior::Calculate(float DeltaTime)
{
	Delta = DeltaTime;

	FVector result = FVector::ZeroVector;

	if (ShouldSeek)
	{
		result += Seek(TargetLocation);
	}

	if (ShouldArrive)
	{
		result += Arrive(TargetLocation, DecelerationType);
	}

	if (ShouldPursue)
	{
		result += Pursue(TargetObject);
	}

	if (ShouldSeparate)
	{
		result += Separation(PlayerBase->GetNeighbors());
	}

	if (ShouldInterpose)
	{
		result += Interpose(TargetObject, OtherTargetObject);
	}

	return result;
}

void SteeringBehavior::SetTargetLocation(FVector Target)
{
	TargetLocation = Target;
}

FVector SteeringBehavior::GetTargetLocation()
{
	return TargetLocation;
}

void SteeringBehavior::SetDecelerationType(EDecelerationType Deceleration)
{
	DecelerationType = Deceleration;
}

void SteeringBehavior::SetTargetObject(UPrimitiveComponent* Object)
{
	TargetObject = Object;
}

void SteeringBehavior::SetOtherTargetObject(UPrimitiveComponent* Object)
{
	OtherTargetObject = Object;
}

FVector SteeringBehavior::Seek(FVector Target)
{
	FVector desiredVelocity = Target - PlayerBase->GetLocation();
	desiredVelocity.Normalize();
	desiredVelocity *= PlayerBase->CurrentSpeed;

	FVector projectedPosition = PlayerBase->GetLocation() + (desiredVelocity * Delta);

	if (FVector::Dist(projectedPosition, Target) > FVector::Dist(PlayerBase->GetLocation(), Target))
	{
		return FVector::ZeroVector;
	}

	return desiredVelocity;
}

FVector SteeringBehavior::Arrive(FVector Target, EDecelerationType Deceleration)
{
	Target.Z = 0.0f;

	FVector toTarget = Target - PlayerBase->GetLocation();
	float distance = toTarget.Size();

	FVector desiredVelocity = FVector::ZeroVector;

	if (distance > 0.0f)
	{
		float speed = distance / ((uint8)Deceleration * DecelerationTweaker);
		speed = FMath::Min(speed, PlayerBase->CurrentSpeed);

		desiredVelocity = (toTarget * speed / distance);
	}

	return desiredVelocity;
}

FVector SteeringBehavior::Pursue(UPrimitiveComponent* Object)
{
	FVector targetLocation = Object->GetComponentLocation();
	targetLocation.Z = 0.0f;

	FVector toTarget = targetLocation - PlayerBase->GetLocation();

	FVector heading = PlayerBase->GetVelocity();
	heading.Normalize();

	FVector objectVelocity = Object->GetPhysicsLinearVelocity();
	objectVelocity.Z = 0.0f;

	FVector objectHeading = objectVelocity;
	objectHeading.Normalize();

	float relativeHeading = FVector::DotProduct(heading, objectHeading);

	if ((FVector::DotProduct(toTarget, heading) > 0.0f) &&
		(relativeHeading < -0.95)) // acos(0.95) = 18 degrees
	{
		return Seek(targetLocation);

		//return Arrive(targetLocation, EDecelerationType::fast);
	}

	float lookAheadTime = toTarget.Size() / (PlayerBase->CurrentSpeed + objectVelocity.Size());

	return Seek(targetLocation + objectVelocity * lookAheadTime);

	//return Arrive(targetLocation + objectVelocity * lookAheadTime, EDecelerationType::fast);
}

FVector SteeringBehavior::Separation(TArray<UPrimitiveComponent*> Neighbors)
{
	FVector result = FVector::ZeroVector;

	for (UPrimitiveComponent* neighbor : Neighbors)
	{
		FVector neighborLocation = neighbor->GetComponentLocation();
		neighborLocation.Z = 0.0f;

		FVector fromNeighbor = PlayerBase->GetLocation() - neighborLocation;
		float distance = fromNeighbor.Size();

		fromNeighbor.Normalize();

		result += fromNeighbor / distance;
	}

	if (result == FVector::ZeroVector)
	{
		return result;
	}

	result.Normalize();

	return result * PlayerBase->CurrentSpeed / PlayerBase->GetTeam()->GetSeparationDampening();
}

FVector SteeringBehavior::Interpose(UPrimitiveComponent* ObjectA, UPrimitiveComponent* ObjectB)
{
	FVector APosition = ObjectA->GetComponentLocation();
	APosition.Z = 0.0f;

	FVector BPosition = ObjectB->GetComponentLocation();
	BPosition.Z = 0.0f;

	FVector midpoint = (APosition + BPosition) / 2.0f;
	midpoint.Z = 0.0f;

	float timeToReachMidpoint = FVector::Dist(PlayerBase->GetLocation(), midpoint) / PlayerBase->CurrentSpeed;

	FVector AVelocity = ObjectA->GetPhysicsLinearVelocity();
	AVelocity.Z = 0.0f;

	FVector BVelocity = ObjectB->GetPhysicsLinearVelocity();
	BVelocity.Z = 0.0f;

	FVector futureA = APosition + (AVelocity * timeToReachMidpoint);
	FVector futureB = BPosition + (BVelocity * timeToReachMidpoint);

	midpoint = (futureA + futureB) / 2.0f;

	return Arrive(midpoint, EDecelerationType::fast);
}

void SteeringBehavior::ArriveOn()
{
	ShouldArrive = true;
}

void SteeringBehavior::ArriveOff()
{
	ShouldArrive = false;
}

void SteeringBehavior::SeekOn()
{
	ShouldSeek = true;
}

void SteeringBehavior::SeekOff()
{
	ShouldSeek = false;
}

void SteeringBehavior::SeparationOn()
{
	ShouldSeparate = true;
}

void SteeringBehavior::SeparationOff()
{
	ShouldSeparate = false;
}

void SteeringBehavior::InterposeOn()
{
	ShouldInterpose = true;
}

void SteeringBehavior::InterposeOff()
{
	ShouldInterpose = false;
}

void SteeringBehavior::PursuitOn()
{
	ShouldPursue = true;
}

void SteeringBehavior::PursuitOff()
{
	ShouldPursue = false;
}

bool SteeringBehavior::PursuitIsOn()
{
	return ShouldPursue;
}

const float SteeringBehavior::DecelerationTweaker = 0.3f;