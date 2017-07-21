#include "SoccerSimulator.h"
#include "SoccerTeam.h"
#include "PlayerBase.h"
#include "GoalKeeper.h"
#include "FieldPlayer.h"
#include "Goal.h"
#include "SoccerBall.h"
#include "SoccerField.h"
#include "StateMachine.h"
#include "TeamStates.h"
#include "SteeringBehavior.h"
#include "FieldPlayerStates.h"
#include "GoalKeeperStates.h"

ASoccerTeam::ASoccerTeam()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GoalKeeper = CreateDefaultSubobject<UChildActorComponent>(TEXT("Goal Keeper"));

	FieldPlayer1 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 1"));
	FieldPlayer2 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 2"));
	FieldPlayer3 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 3"));
	FieldPlayer4 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 4"));
	FieldPlayer5 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 5"));
	FieldPlayer6 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 6"));
	FieldPlayer7 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 7"));
	FieldPlayer8 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 8"));
	FieldPlayer9 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 9"));
	FieldPlayer10 = CreateDefaultSubobject<UChildActorComponent>(TEXT("Field Player 10"));

	RootComponent = DefaultComponent;
	GoalKeeper->SetupAttachment(RootComponent);
	FieldPlayer1->SetupAttachment(RootComponent);
	FieldPlayer2->SetupAttachment(RootComponent);
	FieldPlayer3->SetupAttachment(RootComponent);
	FieldPlayer4->SetupAttachment(RootComponent);
	FieldPlayer5->SetupAttachment(RootComponent);
	FieldPlayer6->SetupAttachment(RootComponent);
	FieldPlayer7->SetupAttachment(RootComponent);
	FieldPlayer8->SetupAttachment(RootComponent);
	FieldPlayer9->SetupAttachment(RootComponent);
	FieldPlayer10->SetupAttachment(RootComponent);

	XSupportDots = 100;
	YSupportDots = 50;
	AttemptsToFindValidStrike = 5;
	TimeBetweenSupportSpotCalculations = 1.0f;
	CanPassStrength = 2.0f;
	CanScoreFromPositionStrength = 1.0f;
	OptimalDistanceFromControllingPlayer = 200.0f;
	DistanceFromControllingPlayerStrength = 2.0f;
	KickingRadius = 100.0f;
	SecondsPerKick = 1.0f;
	InterceptionRangeScalingFactor = 0.3f;
	SeparationDampening = 20.0f;
	AheadOfAttackerStrength = 1.0f;
	MaxTimeForPass = 0.3f;
	ControlLossDistance = 400.0f;
	AdvancementSpeed = 1.0f;
	MaximumAdvancementDistance = 2000.0f;
	DribbleDistance = 3000.0f;

	DisplaySupportSpots = false;

	PassAngleDot = 0.7f;

	AdvancementState = ETeamAdvancementState::fallback;
}

ASoccerTeam::~ASoccerTeam()
{
	delete TeamStateMachine;
}

void ASoccerTeam::BeginPlay()
{
	Super::BeginPlay();

	Location = GetActorLocation();
	Location.Z = 0.0f;
	InitialLocation = Location;
}

void ASoccerTeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Field->Paused())
	{
		CalculateClosestPlayerToBall();

		TeamStateMachine->Update();

		UpdateAdvancement(DeltaTime);
	}
}

bool ASoccerTeam::CanShoot(FVector From, float Power, FVector& ShotTarget)
{
	int32 attempts = AttemptsToFindValidStrike;

	ShotTarget = OpponentGoal->GetLocation() + OpponentGoal->Facing() * OpponentGoal->ScoringArea->GetScaledBoxExtent().Y;

	while (--attempts)
	{
		ShotTarget.Y = FMath::RandRange(OpponentGoal->Left() + Ball->GetRadius(), OpponentGoal->Right() - Ball->GetRadius());

		float time = Ball->TimeToCoverDistance(Ball->GetLocation(), ShotTarget, Power);

		if (time >= 0.0f)
		{
			if (IsPassSafeFromAllOpponents(Ball->GetLocation(), ShotTarget, nullptr, Power))
			{
				return true;
			}
		}
	}

	return false;
}

void ASoccerTeam::RequestPass(APlayerBase& Player)
{
	if (IsPassSafeFromAllOpponents(ControllingPlayer->GetLocation(), Player.GetLocation(), &Player, Player.MaxPassingStrength) &&
		SSCalculator->LatestScoreForPosition(ControllingPlayer->GetLocation()) > SSCalculator->LatestScoreForPosition(Player.GetLocation()))
	{
		AFieldPlayer* controller = Cast<AFieldPlayer>(ControllingPlayer);
		AFieldPlayer* receiver = Cast<AFieldPlayer>(&Player);

		if (controller != nullptr &&
			receiver != nullptr)
		{
			controller->MessagePassToMe(*receiver);
		}
	}
}

FVector ASoccerTeam::GetSupportSpot()
{
	return SSCalculator->GetBestSupportingPosition();
}

bool ASoccerTeam::InControl()
{
	return ControllingPlayer != nullptr;
}

APlayerBase* ASoccerTeam::GetReceiver()
{
	return Receiver;
}

void ASoccerTeam::SetReceiver(APlayerBase* Player)
{
	Receiver = Player;
}

APlayerBase* ASoccerTeam::GetSupportingPlayer()
{
	return SupportingPlayer;
}

void ASoccerTeam::SetSupportingPlayer(APlayerBase* Player)
{
	SupportingPlayer = Player;
}

APlayerBase* ASoccerTeam::GetControllingPlayer()
{
	return ControllingPlayer;
}

void ASoccerTeam::SetControllingPlayer(APlayerBase* Player)
{
	ControllingPlayer = Player;

	OpponentTeam->ControllingPlayer = nullptr;

	if (SupportingPlayer == Player)
	{
		SupportingPlayer = nullptr;
	}
}

APlayerBase* ASoccerTeam::GetPlayerClosestToBall()
{
	return PlayerClosestToBall;
}

void ASoccerTeam::SetPlayerClosestToBall(APlayerBase* Player)
{
	PlayerClosestToBall = Player;
}

APlayerBase* ASoccerTeam::GetClosestPlayerBehindBall()
{
	return ClosestPlayerBehindBall;
}

APlayerBase* ASoccerTeam::GetClosestNonGoalieToBall()
{
	return ClosestNonGoalieToBall;
}

bool ASoccerTeam::IsPassSafeFromAllOpponents(FVector From, FVector To, APlayerBase* ReceivingPlayer, float PassingForce)
{
	for (APlayerBase* opponent : OpponentTeam->FieldPlayers)
	{
		if (!IsPassSafeFromOpponent(From, To, ReceivingPlayer, opponent, PassingForce))
		{
			return false;
		}
	}

	APlayerBase* opponent = Cast<APlayerBase>(OpponentTeam->GoalKeeper->GetChildActor());

	return IsPassSafeFromOpponent(From, To, ReceivingPlayer, opponent, PassingForce);
}

bool ASoccerTeam::IsPassSafeFromOpponent(FVector From, FVector To, APlayerBase* Receiving, APlayerBase* Opponent, float PassingForce)
{
	FVector toTarget = To - From;
	toTarget.Normalize();

	FVector localPositionOfOpponent = Opponent->GetLocation() - From;
	localPositionOfOpponent.Normalize();

	float dot = FVector::DotProduct(toTarget, localPositionOfOpponent);

	if (dot < 0.0f)
	{
		return true;
	}

	if (FVector::DistSquared(From, To) < FVector::DistSquared(Opponent->GetLocation(), From))
	{
		if (Receiving)
		{
			return FVector::DistSquared(To, Opponent->GetLocation()) > FVector::DistSquared(To, Receiving->GetLocation());
		}
		else
		{
			return true;
		}
	}

	float timeForBall = Ball->TimeToCoverDistance(From, To, PassingForce);
	float reach = Opponent->CurrentSpeed * timeForBall + Opponent->GetRadius() + Ball->GetRadius();

	return FVector::Dist(Opponent->GetLocation(), To) >= reach;
}

bool ASoccerTeam::IsOpponentWithinRadius(FVector Center, float Radius)
{
	for (APlayerBase* player : OpponentTeam->Players)
	{
		if (FVector::Dist(Center, player->GetLocation()) < Radius)
		{
			return true;
		}
	}

	return false;
}

void ASoccerTeam::SetOpponentTeam(const ASoccerTeam& OtherTeam, const AGoal& OurGoal, const AGoal& OtherGoal, const ASoccerBall& SoccerBall, const ASoccerField& SoccerField)
{
	TeamStateMachine = new StateMachine<ASoccerTeam>(*this);
	TeamStateMachine->SetCurrentState(*PrepareForKickoff::Instance());
	TeamStateMachine->SetPreviousState(*PrepareForKickoff::Instance());
	TeamStateMachine->SetGlobalState(nullptr);

	OpponentTeam = &const_cast<ASoccerTeam&>(OtherTeam);
	Goal = &const_cast<AGoal&>(OurGoal);
	OpponentGoal = &const_cast<AGoal&>(OtherGoal);
	Ball = &const_cast<ASoccerBall&>(SoccerBall);
	Field = &const_cast<ASoccerField&>(SoccerField);

	Goal->SetTeam(const_cast<ASoccerTeam&>(*this));

	SSCalculator = new SupportSpotCalculator(XSupportDots, YSupportDots, *this);
	GetWorld()->GetTimerManager().SetTimer(SupportSpotTimer, this, &ASoccerTeam::DetermineSupportSpot, TimeBetweenSupportSpotCalculations, true);
}

void ASoccerTeam::UpdateTargetsOfWaitingPlayers()
{
	for (AFieldPlayer* player : FieldPlayers)
	{
		if (player->GetStateMachine().IsInState(*Wait::Instance()) ||
			player->GetStateMachine().IsInState(*ReturnToHomeRegion::Instance()))
		{
			player->ReturnHome();
		}
	}
}

void ASoccerTeam::ReturnAllFieldPlayersToHome()
{
	for (APlayerBase* player : Players)
	{
		player->ReturnHome();
	}

	for (AFieldPlayer* player : FieldPlayers)
	{
		player->GetStateMachine().ChangeState(*ReturnToHomeRegion::Instance());
	}

	//Goalie->GetStateMachine().ChangeState(*ReturnHome::Instance());
}

void ASoccerTeam::SetAllPlayersToAttackMode()
{
	for (APlayerBase* player : Players)
	{
		player->SetAttackRegion();
	}
}

void ASoccerTeam::SetAllPlayersToDefenseMode()
{
	for (APlayerBase* player : Players)
	{
		player->SetInitialHomeRegion();
	}
}

bool ASoccerTeam::AllPlayersAtHome()
{
	for (APlayerBase* player : Players)
	{
		if (!player->InHomeRegion())
		{
			return false;
		}
	}

	return true;
}

FVector ASoccerTeam::GetReceivingOffset()
{
	return ReceivingOffset;
}

ASoccerTeam* ASoccerTeam::GetOpponentTeam()
{
	return OpponentTeam;
}

ASoccerField* ASoccerTeam::GetField()
{
	return Field;
}

StateMachine<ASoccerTeam>& ASoccerTeam::GetStateMachine()
{
	return *TeamStateMachine;
}

ASoccerTeam::SupportSpotCalculator::SupportSpotCalculator(int32 XSpots, int32 YSpots, const ASoccerTeam& Team) :
	XCount(XSpots), YCount(YSpots), mTeam(&const_cast<ASoccerTeam&>(Team)), BestSupportingSpot(nullptr)
{
	FVector bounds = mTeam->Field->SupportAreaScaledBounds();

	float sliceX = bounds.X * 2.0f / XSpots;
	float sliceY = bounds.Y * 2.0f / YSpots;

	FVector fieldLocation = mTeam->Field->GetLocation();

	float left = fieldLocation.X - bounds.X + sliceX / 2.0f;
	float right = fieldLocation.X + bounds.X - sliceX / 2.0f;
	float top = fieldLocation.Y - bounds.Y + sliceY / 2.0f;

	for (int x = 0; x < (XSpots / 2); ++x)
	{
		for (int y = 0; y < YSpots; ++y)
		{
			//if (mTeam->Team == ETeam::red)
			{
				Spots.push_back(SupportSpot(FVector(left + x * sliceX, top + y * sliceY, 0.0f), 0.0f));
			}
			//else
			{
				Spots.push_back(SupportSpot(FVector(right - x * sliceX, top + y * sliceY, 0.0f), 0.0f));
			}
		}
	}

	for (SupportSpot& spot : Spots)
	{
		SpotMap.Add(spot.Position) = &spot;
	}
}

void ASoccerTeam::DetermineSupportSpot()
{
	if (ControllingPlayer != nullptr)
	{
		SSCalculator->DetermineBestSupportingPosition();

		SSCalculator->DrawBestSpot();
	}
}

void ASoccerTeam::SetUpTeamReferences()
{
	Goalie = Cast<AGoalKeeper>(GoalKeeper->GetChildActor());
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer1->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer2->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer3->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer4->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer5->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer6->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer7->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer8->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer9->GetChildActor()));
	FieldPlayers.Add(Cast<AFieldPlayer>(FieldPlayer10->GetChildActor()));

	Players.Add(Cast<APlayerBase>(GoalKeeper->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer1->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer2->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer3->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer4->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer5->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer6->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer7->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer8->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer9->GetChildActor()));
	Players.Add(Cast<APlayerBase>(FieldPlayer10->GetChildActor()));

	for (APlayerBase* player : Players)
	{
		player->SetTeam(this);
	}

	TeamStateMachine->GetCurrentState().Enter(*this);
}

ASoccerBall* ASoccerTeam::GetBall()
{
	return Ball;
}

AGoal* ASoccerTeam::GetGoal()
{
	return Goal;
}

AFieldPlayer* ASoccerTeam::DetermineBestSupportingAttacker()
{
	float closestSoFar = Field->RegionScaledBounds().X * 4.0f;

	if (Team == ETeam::red)
	{
		int x = 5;
		int y = -5;

		x += y;
		closestSoFar += x;
		closestSoFar += 100.0f;
	}

	AFieldPlayer* support = nullptr;

	for (AFieldPlayer* player : FieldPlayers)
	{
		if (player != ControllingPlayer)
		{
			float distance = FVector::Dist(player->GetLocation(), GetSupportSpot());

			if (distance < closestSoFar)
			{
				closestSoFar = distance;
				support = player;
			}
		}
	}

	return support;
}

float ASoccerTeam::GetKickRadius()
{
	return KickingRadius;
}

float ASoccerTeam::GetSecondsPerKick()
{
	return SecondsPerKick;
}

float ASoccerTeam::GetInterceptionRangeScalingFactor()
{
	return InterceptionRangeScalingFactor;
}

float ASoccerTeam::GetSeparationDampening()
{
	return SeparationDampening;
}

float ASoccerTeam::GetDribbleDistance()
{
	return DribbleDistance;
}

bool ASoccerTeam::FindPass(APlayerBase& Passer, APlayerBase*& Receiving, FVector& PassTarget, float Power, float Distance)
{
	float closestToGoal = Field->RegionScaledBounds().X * 4.0f;

	FVector target;

	for (APlayerBase* player : Players)
	{
		float dot = FVector::DotProduct((Ball->GetLocation() - player->GetLocation()), (Ball->GetLocation() - Passer.GetLocation()));

		if (player != &Passer &&
			player != Goalie &&
			FVector::Dist(Passer.GetLocation(), player->GetLocation()) > Distance &&
			dot < PassAngleDot)// &&
			//player->IsAheadOfAttacker())
		{
			if (GetBestPassToReceiver(Passer, *player, target, Power))
			{
				float distance = FMath::Abs(target.X - OpponentTeam->GetGoal()->GetLocation().X);

				if (distance < closestToGoal)
				{
					closestToGoal = distance;

					Receiving = player;

					PassTarget = target;
				}
			}
		}
	}

	return Receiving != nullptr;
}

bool ASoccerTeam::IsPlayerThreatened(APlayerBase& Player)
{
	for (APlayerBase* opponent : OpponentTeam->Players)
	{
		if (Player.PositionInFrontOfPlayer(opponent->GetLocation()) &&
			FVector::Dist(Player.GetLocation(), opponent->GetLocation()) < Player.ComfortRadius)
		{
			return true;
		}
	}

	return false;
}

bool ASoccerTeam::GetBestPassToReceiver(APlayerBase& Passer, APlayerBase& Receiving, FVector& PassTarget, float Power)
{
	float time = Ball->TimeToCoverDistance(Ball->GetLocation(), Receiving.GetLocation(), Power);

	if (time <= 0 ||
		(time >= MaxTimeForPass &&
		 &Passer != Goalie))
	{
		return false;
	}

	float interceptionRange = time * Receiving.CurrentSpeed * InterceptionRangeScalingFactor;

	FVector toReceiver = Receiving.GetLocation() - Passer.GetLocation();
	toReceiver.Normalize();

	FVector tangent = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), toReceiver);
	tangent *= interceptionRange;

	std::vector<FVector> passes;

	passes.push_back(Receiving.GetLocation() + tangent);
	passes.push_back(Receiving.GetLocation());
	passes.push_back(Receiving.GetLocation() - tangent);

	float closestSoFar = Field->RegionScaledBounds().X * 4.0f;
	bool result = false;

	for (FVector& pass : passes)
	{
		float distance = FMath::Abs(pass.X - OpponentTeam->GetGoal()->GetLocation().X);

		if (distance < closestSoFar &&
			Field->IsPositionInPlayArea(pass) &&
			IsPassSafeFromAllOpponents(Ball->GetLocation(), pass, &Receiving, Power))
		{
			closestSoFar = distance;
			PassTarget = pass;
			result = true;
		}
	}

	return result;
}

bool ASoccerTeam::ControllingPlayerTooFarFromBall()
{
	if (ControllingPlayer == nullptr)
	{
		return true;
	}

	return FVector::Dist(ControllingPlayer->GetLocation(), Ball->GetLocation()) > ControlLossDistance;
}

ETeam ASoccerTeam::GetTeam()
{
	return Team;
}

FVector ASoccerTeam::GetLocation()
{
	FVector current = Location;
	current.Z = 0.0f;

	return current;
}

int ASoccerTeam::NumberOfPlayersOfType(EPlayerRole Type, int& number, AFieldPlayer* Player)
{
	int amount = 0;

	for (AFieldPlayer* player : FieldPlayers)
	{
		if (player == Player)
		{
			number = amount;
		}

		if (player->PlayerRole == Type)
		{
			++amount;
		}
	}

	return amount;
}

FVector ASoccerTeam::CalculatePlayerHomeRegion(class AFieldPlayer* Player)
{
	FVector regionPosition = FVector::ZeroVector;

	int playerNumber;
	int numberOfType = NumberOfPlayersOfType(Player->PlayerRole, playerNumber, Player);

	FVector bounds = Field->PlayAreaScaledBounds() * 2.0f;
	FVector fieldLocation = Field->GetPlayAreaLocation();

	float sliceY = bounds.Y / numberOfType;

	regionPosition.Y = fieldLocation.Y - (bounds.Y * 0.5f);
	regionPosition.Y += (sliceY * playerNumber) + (sliceY * 0.5f);

	float sqDistanceFromMiddle = TeamRoleDistances[Player->PlayerRole] * Field->CurrentDiagonalRatio();
	sqDistanceFromMiddle *= sqDistanceFromMiddle;

	float sqYDistance = Location.Y - regionPosition.Y;
	sqYDistance *= sqYDistance;

	regionPosition.X = sqDistanceFromMiddle - sqYDistance;
	regionPosition.X = FMath::Sqrt(regionPosition.X);

	if (Team == ETeam::blue)
	{
		regionPosition.X = Location.X + regionPosition.X;
	}
	else
	{
		regionPosition.X = Location.X - regionPosition.X;
		//GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Red, FString::Printf(TEXT("Start for type %d number %d: %f, %f"), (int)Player->PlayerRole, playerNumber, regionPosition.X, regionPosition.Y));
	}

	if (FMath::IsNaN(regionPosition.X))
	{
		regionPosition.X = Location.X;
	}

	return regionPosition;
}

void ASoccerTeam::Advance()
{
	AdvancementState = ETeamAdvancementState::advance;
}

void ASoccerTeam::FallBack()
{
	AdvancementState = ETeamAdvancementState::fallback;
}

bool ASoccerTeam::Losing()
{
	return Goal->GetScore() < OpponentGoal->GetScore();
}

bool ASoccerTeam::DrawDebugBoxes()
{
	return DisplaySupportSpots;
}

void ASoccerTeam::CalculateClosestPlayerToBall()
{
	float closestSoFar = FVector::Dist(Goalie->GetLocation(), Ball->GetLocation());
	float closestBehindBall = closestSoFar;
	float closestNonGoalie = closestSoFar;

	for (APlayerBase* player : Players)
	{
		if (player->RecoveryComplete())
		{
			float distance = FVector::Dist(player->GetLocation(), Ball->GetLocation());

			if (distance <= closestSoFar)
			{
				closestSoFar = distance;

				PlayerClosestToBall = player;

				if (player != Goalie)
				{
					closestNonGoalie = distance;

					ClosestNonGoalieToBall = player;
				}
			}

			if (distance <= closestBehindBall &&
				player->PositionInFrontOfPlayer(Ball->GetLocation()))
			{
				closestBehindBall = distance;

				ClosestPlayerBehindBall = player;
			}
		}
	}
}

void ASoccerTeam::UpdateAdvancement(float DeltaTime)
{
	float xOffset = FMath::Abs(Location.X - InitialLocation.X);

	switch (AdvancementState)
	{
	case ETeamAdvancementState::advance:
		xOffset += AdvancementSpeed * DeltaTime;
		
		break;

	case ETeamAdvancementState::fallback:
		xOffset -= AdvancementSpeed * DeltaTime;

		break;
	}

	xOffset = FMath::Clamp(xOffset, 0.0f, MaximumAdvancementDistance);

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Team Advancement: %f"), xOffset));

	if (Team == ETeam::blue)
	{
		Location.X = InitialLocation.X + xOffset;
	}
	else
	{
		Location.X = InitialLocation.X - xOffset;
	}

	Location.Y = Field->GetPlayAreaLocation().Y;
}

void ASoccerTeam::SupportSpotCalculator::DetermineBestSupportingPosition(bool ForceOperation)
{
	if (mTeam->ControllingPlayer == nullptr && !ForceOperation)
	{
		return;
	}

	BestSupportingSpot = nullptr;

	float bestSpotSoFar = -1.0f;

	for (SupportSpot& spot : Spots)
	{
		spot.Score = 1.0f;

		if (mTeam->ControllingPlayer != nullptr &&
			mTeam->IsPassSafeFromAllOpponents(mTeam->ControllingPlayer->GetLocation(),
			spot.Position, nullptr, mTeam->ControllingPlayer->MaxPassingStrength))
		{
			spot.Score += mTeam->CanPassStrength;
		}

		FVector shotDirection;

		//Test for whether the support spot 
		if (mTeam->ControllingPlayer != nullptr &&
			mTeam->CanShoot(spot.Position,
				mTeam->ControllingPlayer->MaxShootingStrength,
				shotDirection))
		{
			spot.Score += mTeam->CanScoreFromPositionStrength;
		}

		//Test for how far the pass would be
		if (mTeam->SupportingPlayer != nullptr &&
			mTeam->ControllingPlayer != nullptr)
		{
			float distance = FVector::Distance(mTeam->ControllingPlayer->GetLocation(), spot.Position);
			float distanceFromOptimal = FMath::Abs(mTeam->OptimalDistanceFromControllingPlayer - distance);

			if (distanceFromOptimal < mTeam->OptimalDistanceFromControllingPlayer)
			{
				spot.Score += mTeam->DistanceFromControllingPlayerStrength *
					((mTeam->OptimalDistanceFromControllingPlayer - distanceFromOptimal) /
						mTeam->OptimalDistanceFromControllingPlayer);
			}
		}

		if (mTeam->ControllingPlayer != nullptr)
		{
			if (mTeam->Team == ETeam::blue)
			{
				if (spot.Position.X > mTeam->ControllingPlayer->GetLocation().X)
				{
					spot.Score += mTeam->AheadOfAttackerStrength;
				}
			}
			else
			{
				if (spot.Position.X < mTeam->ControllingPlayer->GetLocation().X)
				{
					spot.Score += mTeam->AheadOfAttackerStrength;
				}
			}
		}

		//Set if best
		if (spot.Score > bestSpotSoFar)
		{
			bestSpotSoFar = spot.Score;
			BestSupportingSpot = &spot;
		}

		DebugSpot(spot, 5.0f);
	}
}

void ASoccerTeam::SupportSpotCalculator::DrawBestSpot()
{
	DebugSpot(*BestSupportingSpot, 6.0f, true);
	DebugSpot(*BestSupportingSpot, 5.0f, true);
	DebugSpot(*BestSupportingSpot, 4.0f, true);
	DebugSpot(*BestSupportingSpot, 3.0f, true);
}

FVector ASoccerTeam::SupportSpotCalculator::GetBestSupportingPosition()
{
	if (BestSupportingSpot == nullptr)
	{
		DetermineBestSupportingPosition(true);
	}

	return BestSupportingSpot->Position;
}

float ASoccerTeam::SupportSpotCalculator::LatestScoreForPosition(FVector Position)
{
	FVector bounds = mTeam->Field->SupportAreaScaledBounds();

	float sliceX = bounds.X * 2.0f / XCount;
	float sliceY = bounds.Y * 2.0f / YCount;

	FVector fieldLocation = mTeam->Field->GetLocation();

	float left = fieldLocation.X - bounds.X + sliceX / 2.0f;
	float right = fieldLocation.X + bounds.X - sliceX / 2.0f;
	float top = fieldLocation.Y - bounds.Y + sliceY / 2.0f;

	int32 xindex = (int32)((Position.X - left) / sliceX);
	int32 yindex = (int32)((Position.Y - top) / sliceY);

	FVector closestSpotPosition(xindex * sliceX + left, yindex * sliceY + top, 0.0f);

	SupportSpot** spotAddress = SpotMap.Find(closestSpotPosition);

	if (spotAddress != nullptr)
	{
		SupportSpot* spot = *spotAddress;

		return spot->Score;
	}
	
	return 0.0f;
}

void ASoccerTeam::SupportSpotCalculator::DebugSpot(SupportSpot& Spot, float Size, bool BestSpot)
{
	if (mTeam->DisplaySupportSpots)
	{
		FVector offset(0.0f, 0.0f, 20.0f);
		float size = Size * Spot.Score;
		FVector extent(size, size, size);

		if (mTeam->Team == ETeam::blue && !BestSpot)
		{
			DrawDebugBox(mTeam->GetWorld(), Spot.Position + offset, extent, FColor::Blue, false, mTeam->TimeBetweenSupportSpotCalculations);
		}
		else if (!BestSpot)
		{
			DrawDebugBox(mTeam->GetWorld(), Spot.Position + offset, extent, FColor::Red, false, mTeam->TimeBetweenSupportSpotCalculations);
		}
		else
		{
			DrawDebugBox(mTeam->GetWorld(), Spot.Position + offset, extent, FColor::Black, false, mTeam->TimeBetweenSupportSpotCalculations);
		}
	}
}