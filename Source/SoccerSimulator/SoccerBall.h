#pragma once

#include "GameFramework/Actor.h"
#include "SoccerBall.generated.h"

UCLASS()
class SOCCERSIMULATOR_API ASoccerBall : public AActor
{
	GENERATED_BODY()
	
public:	

	ASoccerBall();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Soccer Ball")
	void Kick(FVector Direction, float Force);

	UFUNCTION(BlueprintCallable, Category = "Soccer Ball")
	UPrimitiveComponent* GetObject();

	FVector GetLocation();

	float GetRadius();

	float TimeToCoverDistance(FVector From, FVector To, float Force);

	FVector FuturePosition(float Time);

	void Trap(class APlayerBase* Player);

	FVector GetLastPosition();

	bool Stale();

	UFUNCTION(BlueprintCallable, Category = "Soccer Ball")
	void PlaceAtPosition(FVector Position);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BallMesh;

	UPROPERTY(EditAnywhere)
	float TimeUntilStale;

protected:

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalVector, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float BallFriction;

private:

	void UpdateLastPosition();

	void UpdateHeading();

	class APlayerBase* Owner;

	FVector LastPosition;

	float TimeSinceLastKick;
};