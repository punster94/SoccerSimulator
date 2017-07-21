#pragma once

#include "Enums.h"

class SOCCERSIMULATOR_API SteeringBehavior
{

public:

	SteeringBehavior(class APlayerBase* Player);

	~SteeringBehavior();

	FVector Calculate(float DeltaTime);

	void SetTargetLocation(FVector Target);

	FVector GetTargetLocation();

	void SetDecelerationType(EDecelerationType Deceleration);

	void SetTargetObject(UPrimitiveComponent* Object);

	void SetOtherTargetObject(UPrimitiveComponent* Object);

	void ArriveOn();
	
	void ArriveOff();

	void SeekOn();

	void SeekOff();

	void SeparationOn();

	void SeparationOff();

	void InterposeOn();

	void InterposeOff();

	void PursuitOn();

	void PursuitOff();

	bool PursuitIsOn();

private:

	FVector Seek(FVector Target);

	FVector Arrive(FVector Target, EDecelerationType Deceleration);

	FVector Pursue(UPrimitiveComponent* Object);

	FVector Separation(TArray<UPrimitiveComponent*> Neighbors);

	FVector Interpose(UPrimitiveComponent* ObjectA, UPrimitiveComponent* ObjectB);

	FVector TargetLocation;

	EDecelerationType DecelerationType;

	UPrimitiveComponent* TargetObject;

	UPrimitiveComponent* OtherTargetObject;

	float Delta;

	bool ShouldSeek;

	bool ShouldArrive;

	bool ShouldPursue;

	bool ShouldSeparate;

	bool ShouldInterpose;

	static const float DecelerationTweaker;

	class ASoccerBall* SoccerBall;

	class ASoccerField* SoccerField;

	class APlayerBase* PlayerBase;
};
