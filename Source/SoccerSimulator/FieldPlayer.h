#pragma once

#include "PlayerBase.h"
#include "Enums.h"
#include "StateMachine.h"
#include "FieldPlayer.generated.h"

UCLASS()
class SOCCERSIMULATOR_API AFieldPlayer : public APlayerBase
{
	GENERATED_BODY()

public:

	AFieldPlayer();

	virtual ~AFieldPlayer();

	virtual void Tick(float DeltaTime) override;

	virtual void SetTeam(class ASoccerTeam* TeamReference) override;

	StateMachine<AFieldPlayer>& GetStateMachine();

	void MessageSupportAttacker();

	void MessageGoHome();

	void MessageReceiveBall(FVector Position);

	void MessagePassToMe(AFieldPlayer& Receiver);

	void MessageWait();

	void FindSupport();

	void SetInterposeLocation(FVector Position);

	UPROPERTY(EditAnywhere)
	EPlayerRole PlayerRole;

	UPROPERTY(EditAnywhere)
	EPlayerRole WinningRole;

	UPROPERTY(EditAnywhere)
	EPlayerRole LosingRole;

private:

	StateMachine<AFieldPlayer>* PlayerStateMachine;

	USphereComponent* InterposeTarget;
};
