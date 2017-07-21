#pragma once

#include "State.h"
#include <string>

template <class T>
class StateMachine
{

public:

	StateMachine(const T& owner);

	virtual ~StateMachine();

	void SetCurrentState(State<T>& s);

	void SetPreviousState(State<T>& s);

	void SetGlobalState(State<T>* s);

	void Update() const;

	void ChangeState(State<T>& newState);

	void RevertToPreviousState();

	bool IsInState(const State<T>& s) const;

	State<T>& GetCurrentState() const;

	State<T>& GetPreviousState() const;

	State<T>& GetGlobalState() const;

private:

	T& Owner;

	State<T>* CurrentState;

	State<T>* PreviousState;

	State<T>* GlobalState;
};

#include "StateMachine.inl"