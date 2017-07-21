#pragma once

#include "State.h"

class AGoalKeeper;

class GlobalKeeperState : public State<AGoalKeeper>
{

public:

	static GlobalKeeperState* Instance();

	virtual void Enter(AGoalKeeper& keeper) override;

	virtual void Execute(AGoalKeeper& keeper) override;

	virtual void Exit(AGoalKeeper& keeper) override;

private:

	GlobalKeeperState() {};
};

class TendGoal : public State<AGoalKeeper>
{

public:

	static TendGoal* Instance();

	virtual void Enter(AGoalKeeper& keeper) override;

	virtual void Execute(AGoalKeeper& keeper) override;

	virtual void Exit(AGoalKeeper& keeper) override;

private:

	TendGoal() {};
};

class ReturnHome : public State<AGoalKeeper>
{

public:

	static ReturnHome* Instance();

	virtual void Enter(AGoalKeeper& keeper) override;

	virtual void Execute(AGoalKeeper& keeper) override;

	virtual void Exit(AGoalKeeper& keeper) override;

private:

	ReturnHome() {};
};

class PutBallBackInPlay : public State<AGoalKeeper>
{

public:

	static PutBallBackInPlay* Instance();

	virtual void Enter(AGoalKeeper& keeper) override;

	virtual void Execute(AGoalKeeper& keeper) override;

	virtual void Exit(AGoalKeeper& keeper) override;

private:

	PutBallBackInPlay() {};
};

class InterceptBall : public State<AGoalKeeper>
{

public:

	static InterceptBall* Instance();

	virtual void Enter(AGoalKeeper& keeper) override;

	virtual void Execute(AGoalKeeper& keeper) override;

	virtual void Exit(AGoalKeeper& keeper) override;

private:

	InterceptBall() {};
};

class Fall : public State<AGoalKeeper>
{

public:

	static Fall* Instance();

	virtual void Enter(AGoalKeeper& keeper) override;

	virtual void Execute(AGoalKeeper& keeper) override;

	virtual void Exit(AGoalKeeper& keeper) override;

private:

	Fall() {};
};