#pragma once

#include "GameFramework/Actor.h"
#include "Goal.generated.h"

UCLASS()
class SOCCERSIMULATOR_API AGoal : public AActor
{
	GENERATED_BODY()
	
public:	

	AGoal();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ScoringArea;

	FVector GetLocation();

	UFUNCTION(BlueprintCallable)
	FVector Facing();

	float Left();

	float Right();

	void SetTeam(class ASoccerTeam& AssociatedTeam);

	int32 GetScore();

protected:

	UFUNCTION()
	void ScoreAreaOverlapped(UPrimitiveComponent* ThisComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintReadWrite)
	int32 GoalsScored;

	UPROPERTY(EditAnywhere)
	FVector ResetBallOffset;

private:

	void ResetGame();

	class ASoccerTeam* Team;
};
