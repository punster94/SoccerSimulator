#pragma once

#include <vector>
#include "Enums.h"
#include "StateMachine.h"
#include "GameFramework/Actor.h"
#include "SoccerTeam.generated.h"

UCLASS()
class SOCCERSIMULATOR_API ASoccerTeam : public AActor
{
	GENERATED_BODY()
	
public:	

	ASoccerTeam();

	virtual ~ASoccerTeam();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	bool CanShoot(FVector From, float Power, FVector& ShotTarget);

	void RequestPass(class APlayerBase& Player);

	FVector GetSupportSpot();

	bool InControl();

	class APlayerBase* GetReceiver();

	void SetReceiver(class APlayerBase* Player);

	class APlayerBase* GetSupportingPlayer();

	void SetSupportingPlayer(class APlayerBase* Player);

	class APlayerBase* GetControllingPlayer();

	void SetControllingPlayer(class APlayerBase* Player);

	class APlayerBase* GetPlayerClosestToBall();

	void SetPlayerClosestToBall(class APlayerBase* Player);

	class APlayerBase* GetClosestPlayerBehindBall();

	class APlayerBase* GetClosestNonGoalieToBall();

	bool IsPassSafeFromAllOpponents(FVector From, FVector To, class APlayerBase* ReceivingPlayer, float PassingForce);

	bool IsPassSafeFromOpponent(FVector From, FVector To, class APlayerBase* Receiving, class APlayerBase* Opponent, float PassingForce);

	bool IsOpponentWithinRadius(FVector Center, float Radius);

	void SetOpponentTeam(const ASoccerTeam& OtherTeam, const class AGoal& OurGoal, const class AGoal& OtherGoal, const class ASoccerBall& SoccerBall, const class ASoccerField& SoccerField);

	void UpdateTargetsOfWaitingPlayers();

	void ReturnAllFieldPlayersToHome();

	void SetAllPlayersToAttackMode();

	void SetAllPlayersToDefenseMode();

	bool AllPlayersAtHome();

	FVector GetReceivingOffset();

	ASoccerTeam* GetOpponentTeam();

	ASoccerField* GetField();

	StateMachine<ASoccerTeam>& GetStateMachine();

	void DetermineSupportSpot();

	void SetUpTeamReferences();

	class ASoccerBall* GetBall();

	class AGoal* GetGoal();

	class AFieldPlayer* DetermineBestSupportingAttacker();

	float GetKickRadius();

	float GetSecondsPerKick();

	float GetInterceptionRangeScalingFactor();

	float GetSeparationDampening();

	float GetDribbleDistance();

	bool FindPass(class APlayerBase& Passer, class APlayerBase*& Receiving, FVector& PassTarget, float Power, float Distance);

	bool IsPlayerThreatened(class APlayerBase& Player);

	bool GetBestPassToReceiver(class APlayerBase& Passer, class APlayerBase& Receiving, FVector& PassTarget, float Power);

	bool ControllingPlayerTooFarFromBall();

	ETeam GetTeam();

	class AGoalKeeper* Goalie;

	FVector GetLocation();

	int NumberOfPlayersOfType(EPlayerRole Type, int& number, class AFieldPlayer* Player = nullptr);

	FVector CalculatePlayerHomeRegion(class AFieldPlayer* Player);

	void Advance();

	void FallBack();

	bool Losing();

	bool DrawDebugBoxes();

protected:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* DefaultComponent;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* GoalKeeper;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer1;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer2;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer3;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer4;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer5;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer6;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer7;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer8;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer9;

	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* FieldPlayer10;

	UPROPERTY(EditAnywhere)
	ETeam Team;

	UPROPERTY(EditAnywhere)
	int32 XSupportDots;

	UPROPERTY(EditAnywhere)
	int32 YSupportDots;

	UPROPERTY(EditAnywhere)
	int32 AttemptsToFindValidStrike;

	UPROPERTY(EditAnywhere)
	float TimeBetweenSupportSpotCalculations;

	UPROPERTY(EditAnywhere)
	float CanPassStrength;

	UPROPERTY(EditAnywhere)
	float CanScoreFromPositionStrength;

	UPROPERTY(EditAnywhere)
	float OptimalDistanceFromControllingPlayer;

	UPROPERTY(EditAnywhere)
	float DistanceFromControllingPlayerStrength;

	UPROPERTY(EditAnywhere)
	float KickingRadius;

	UPROPERTY(EditAnywhere)
	float SecondsPerKick;

	UPROPERTY(EditAnywhere)
	float InterceptionRangeScalingFactor;

	UPROPERTY(EditAnywhere)
	float SeparationDampening;

	UPROPERTY(EditAnywhere)
	float AheadOfAttackerStrength;

	UPROPERTY(EditAnywhere)
	float MaxTimeForPass;

	UPROPERTY(EditAnywhere)
	float ControlLossDistance;

	UPROPERTY(EditAnywhere)
	float AdvancementSpeed;

	UPROPERTY(EditAnywhere)
	float MaximumAdvancementDistance;

	UPROPERTY(EditAnywhere)
	float DribbleDistance;

	UPROPERTY(EditAnywhere)
	FVector ReceivingOffset;

	UPROPERTY(EditAnywhere)
	TMap<EPlayerRole, float> TeamRoleDistances;

	UPROPERTY(BlueprintReadWrite)
	bool DisplaySupportSpots;

	UPROPERTY(EditAnywhere)
	float PassAngleDot;

	TArray<class AFieldPlayer*> FieldPlayers;
	TArray<class APlayerBase*> Players;

private:

	void CalculateClosestPlayerToBall();

	void UpdateAdvancement(float DeltaTime);

	class APlayerBase* Receiver;

	class APlayerBase* SupportingPlayer;

	class APlayerBase* ControllingPlayer;

	class APlayerBase* PlayerClosestToBall;

	class APlayerBase* ClosestPlayerBehindBall;

	class APlayerBase* ClosestNonGoalieToBall;

	FVector Location;

	FVector InitialLocation;

	ETeamAdvancementState AdvancementState;

	ASoccerTeam* OpponentTeam;

	class AGoal* Goal;

	class AGoal* OpponentGoal;

	class ASoccerBall* Ball;

	class ASoccerField* Field;

	StateMachine<ASoccerTeam>* TeamStateMachine;

	FTimerHandle SupportSpotTimer;

	class SupportSpotCalculator
	{

	public:

		SupportSpotCalculator(int32 XSpots, int32 YSpots, const ASoccerTeam& Team);

		void DetermineBestSupportingPosition(bool ForceOperatior = false);

		void DrawBestSpot();

		FVector GetBestSupportingPosition();

		float LatestScoreForPosition(FVector Position);

	private:

		struct SupportSpot
		{
			FVector Position;

			float Score;

			SupportSpot(FVector P, float S) :
				Position(P), Score(S)
			{};
		};

		void DebugSpot(SupportSpot& Spot, float Size, bool BestSpot = false);

		int32 XCount;

		int32 YCount;

		ASoccerTeam* mTeam;

		SupportSpot* BestSupportingSpot;

		std::vector<SupportSpot> Spots;

		TMap<FVector, SupportSpot*> SpotMap;
	};

	SupportSpotCalculator* SSCalculator;
};
