#pragma once

#include "State.h"

class ASoccerTeam;

class Attacking : public State<ASoccerTeam>
{

public:

	static Attacking* Instance();

	virtual void Enter(ASoccerTeam& team) override;

	virtual void Execute(ASoccerTeam& team) override;

	virtual void Exit(ASoccerTeam& team) override;

private:

	Attacking();
};

class Defending : public State<ASoccerTeam>
{

public:

	static Defending* Instance();

	virtual void Enter(ASoccerTeam& team) override;

	virtual void Execute(ASoccerTeam& team) override;

	virtual void Exit(ASoccerTeam& team) override;

private:

	Defending();
};

class PrepareForKickoff : public State<ASoccerTeam>
{

public:

	static PrepareForKickoff* Instance();

	virtual void Enter(ASoccerTeam& team) override;

	virtual void Execute(ASoccerTeam& team) override;

	virtual void Exit(ASoccerTeam& team) override;

private:

	PrepareForKickoff();
};