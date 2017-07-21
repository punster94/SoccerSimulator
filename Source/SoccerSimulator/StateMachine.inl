template <typename T>
StateMachine<T>::StateMachine(const T& owner) :
	Owner(const_cast<T&>(owner)), CurrentState(nullptr), PreviousState(nullptr), GlobalState(nullptr)
{
}

template <typename T>
StateMachine<T>::~StateMachine()
{
}

template <typename T>
void StateMachine<T>::SetCurrentState(State<T>& s)
{
	CurrentState = &s;
}

template <typename T>
void StateMachine<T>::SetPreviousState(State<T>& s)
{
	PreviousState = &s;
}

template <typename T>
void StateMachine<T>::SetGlobalState(State<T>* s)
{
	GlobalState = s;
}

template <typename T>
void StateMachine<T>::Update() const
{
	if (GlobalState != nullptr)
	{
		GlobalState->Execute(Owner);
	}

	if (CurrentState != nullptr)
	{
		CurrentState->Execute(Owner);
	}
}

template <typename T>
void StateMachine<T>::ChangeState(State<T>& newState)
{
	PreviousState = CurrentState;

	CurrentState->Exit(Owner);

	CurrentState = &newState;

	CurrentState->Enter(Owner);
}

template <typename T>
void StateMachine<T>::RevertToPreviousState()
{
	ChangeState(*PreviousState);
}

template <typename T>
bool StateMachine<T>::IsInState(const State<T>& s) const
{
	return CurrentState == &s;
}

template <typename T>
State<T>& StateMachine<T>::GetCurrentState() const
{
	return *CurrentState;
}

template <typename T>
State<T>& StateMachine<T>::GetPreviousState() const
{
	return *PreviousState;
}

template <typename T>
State<T>& StateMachine<T>::GetGlobalState() const
{
	return *GlobalState;
}