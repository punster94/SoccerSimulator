#pragma once

#include "Enums.h"
#include "GameFramework/Actor.h"
#include "PlayerBase.generated.h"

UCLASS()
class SOCCERSIMULATOR_API APlayerBase : public AActor
{
	GENERATED_BODY()
	
public:	

	APlayerBase();

	virtual ~APlayerBase();

	UPROPERTY(EditAnywhere)
	float MaximumSpeed;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DirectionComponent;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* NeighborhoodComponent;

	UPROPERTY(EditAnywhere)
	int32 StartingRegion;

	UPROPERTY(EditAnywhere)
	int32 AttackingRegion;

public:	

	UPROPERTY(EditAnywhere)
	float MaxShootingStrength;

	UPROPERTY(EditAnywhere)
	float ChancePlayerAttemptsPotShot;

	UPROPERTY(EditAnywhere)
	float MaxPassingStrength;

	UPROPERTY(EditAnywhere)
	float MinPassingDistance;

	UPROPERTY(EditAnywhere)
	float DirectionChangeDribbleStrength;

	UPROPERTY(EditAnywhere)
	float MaxDribbleStrength;

	UPROPERTY(EditAnywhere)
	float KickingAccuracy;

	UPROPERTY(EditAnywhere)
	float PassThreatRadius;

	UPROPERTY(EditAnywhere)
	float ChanceOfUsingArriveTypeReceiveBehavior;

	UPROPERTY(EditAnywhere)
	float ReceivingRadius;

	UPROPERTY(EditAnywhere)
	float ComfortRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentStamina;

	UPROPERTY(EditAnywhere)
	float StaminaRegenerationRate;

	UPROPERTY(EditAnywhere)
	float MinimumSpeed;

	UPROPERTY(EditAnywhere)
	float CurrentSpeed;

	UPROPERTY(EditAnywhere)
	float MinimumSpeedThreshold;

	UPROPERTY(EditAnywhere)
	float MaximumSpeedThreshold;

	UPROPERTY(EditAnywhere)
	float StaminaRegenerationThreshold;

	UPROPERTY(EditAnywhere)
	float StaminaCost;

	UPROPERTY(EditAnywhere)
	float Tackleability;

	UPROPERTY(EditAnywhere)
	float RecoveryTime;

	UPROPERTY(EditAnywhere)
	float MovementChangeAlpha;

	virtual void Tick(float DeltaTime) override;

	bool IsThreatened();

	bool BallWithinKeeperRange();

	bool BallWithinKickingRange();

	bool BallWithinReceivingRange();

	bool InHomeRegion();

	bool IsClosestTeamMemberToBall();

	bool IsClosestPlayerOnPitchToBall();

	bool IsClosestNonGoalieOnTeamToBall();

	bool AtTarget();

	bool IsControllingPlayer();

	bool IsAheadOfAttacker();

	bool IsReadyForNextKick();

	bool InHotRegion();

	void ReturnHome();

	FVector GetVelocity();

	FVector GetLocation();

	FVector GetHeading();

	float GetRadius();

	TArray<UPrimitiveComponent*> GetNeighbors();

	UFUNCTION(BlueprintCallable, Category = "Player")
	UPrimitiveComponent* GetObject();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Seek(FVector Position);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Arrive(FVector Position, EDecelerationType Deceleration);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Pursue(UPrimitiveComponent* Object);

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Separate();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void Interpose(UPrimitiveComponent* ObjectA, UPrimitiveComponent* ObjectB);

	class SteeringBehavior* GetSteering();

	void SetHomeRegion(int Region);

	void SetInitialHomeRegion();

	void SetAttackRegion();

	virtual void SetTeam(class ASoccerTeam* TeamReference);

	class ASoccerTeam* GetTeam();

	void ResetVelocity();

	void TrackBall();

	void TrackTarget();

	FVector AddNoiseToKick(FVector BallLocation, FVector BallTarget);

	bool PositionInFrontOfPlayer(FVector Position);

	void BeginRecoveryTimer();

	bool RecoveryComplete();

	void TargetHome();

	void SetSlice(float Top, float Bottom);

	bool BallInSlice();

	FVector GetHomeLocation();

	virtual float ReachSpeed();

	bool BehindBall();

protected:

	UFUNCTION()
	void NeighborhoodOverlap(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void NeighborhoodEndOverlap(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

private:

	void Steer(float DeltaTime);

	void SetKickReady();

	void UpdateStamina(float DeltaTime);

	void UpdateSpeed();

	enum class EStaminaState
	{
		Normal,
		Regenerating,
	};

	float SliceTop;

	float SliceBottom;

	EStaminaState CurrentStaminaState;

	FTimerHandle KickTimer;

	FTimerHandle RecoveryTimer;
	
	FVector HomeRegion;

	FVector OffsetFromTeam;

	TArray<UPrimitiveComponent*> Neighbors;

	class ASoccerTeam* Team;

	class SteeringBehavior* Steering;
};
