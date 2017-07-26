#pragma once

#include "State.h"

class AFieldPlayer;

class GlobalPlayerState : public State<AFieldPlayer>
{

public:

	static GlobalPlayerState* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	GlobalPlayerState() {};
};

class Wait : public State<AFieldPlayer>
{

public:

	static Wait* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	Wait() {};
};

class ChaseBall : public State<AFieldPlayer>
{

public:

	static ChaseBall* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	ChaseBall() {};
};

class ReturnToHomeRegion : public State<AFieldPlayer>
{

public:

	static ReturnToHomeRegion* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	ReturnToHomeRegion() {};
};

class KickBall : public State<AFieldPlayer>
{

public:

	static KickBall* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	KickBall() {};
};

class Dribble : public State<AFieldPlayer>
{

public:

	static Dribble* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	Dribble() {};

	float RotationDirection(FVector V1, FVector V2);
};

class SupportAttacker : public State<AFieldPlayer>
{

public:

	static SupportAttacker* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	SupportAttacker() {};
};

class ReceiveBall : public State<AFieldPlayer>
{

public:

	static ReceiveBall* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	ReceiveBall() {};
};

class BlockBall : public State<AFieldPlayer>
{

public:

	static BlockBall* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	BlockBall() {};
};

class Recover : public State<AFieldPlayer>
{

public:

	static Recover* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	Recover() {};
};

class FindAggressivePosition : public State<AFieldPlayer>
{

public:

	static FindAggressivePosition* Instance();

	virtual void Enter(AFieldPlayer& player) override;

	virtual void Execute(AFieldPlayer& player) override;

	virtual void Exit(AFieldPlayer& player) override;

private:

	FindAggressivePosition() {};
};