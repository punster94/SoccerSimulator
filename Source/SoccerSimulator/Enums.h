#pragma once

UENUM(BlueprintType)
enum class EDecelerationType : uint8
{
	fast = 1,
	normal = 2,
	slow = 3,
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	red,
	blue,
};

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	normal,
	sweeper,
	midfield,
	forward,
};

UENUM(BlueprintType)
enum class ETeamAdvancementState : uint8
{
	advance,
	fallback,
};