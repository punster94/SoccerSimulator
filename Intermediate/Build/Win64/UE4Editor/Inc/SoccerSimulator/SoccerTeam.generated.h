// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	C++ class header boilerplate exported from UnrealHeaderTool.
	This is automatically generated by the tools.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SOCCERSIMULATOR_SoccerTeam_generated_h
#error "SoccerTeam.generated.h already included, missing '#pragma once' in SoccerTeam.h"
#endif
#define SOCCERSIMULATOR_SoccerTeam_generated_h

#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_RPC_WRAPPERS
#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_RPC_WRAPPERS_NO_PURE_DECLS
#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_INCLASS_NO_PURE_DECLS \
	private: \
	static void StaticRegisterNativesASoccerTeam(); \
	friend SOCCERSIMULATOR_API class UClass* Z_Construct_UClass_ASoccerTeam(); \
	public: \
	DECLARE_CLASS(ASoccerTeam, AActor, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/SoccerSimulator"), NO_API) \
	DECLARE_SERIALIZER(ASoccerTeam) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_INCLASS \
	private: \
	static void StaticRegisterNativesASoccerTeam(); \
	friend SOCCERSIMULATOR_API class UClass* Z_Construct_UClass_ASoccerTeam(); \
	public: \
	DECLARE_CLASS(ASoccerTeam, AActor, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/SoccerSimulator"), NO_API) \
	DECLARE_SERIALIZER(ASoccerTeam) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ASoccerTeam(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ASoccerTeam) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASoccerTeam); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASoccerTeam); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ASoccerTeam(ASoccerTeam&&); \
	NO_API ASoccerTeam(const ASoccerTeam&); \
public:


#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ASoccerTeam(ASoccerTeam&&); \
	NO_API ASoccerTeam(const ASoccerTeam&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ASoccerTeam); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ASoccerTeam); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ASoccerTeam)


#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__DefaultComponent() { return STRUCT_OFFSET(ASoccerTeam, DefaultComponent); } \
	FORCEINLINE static uint32 __PPO__GoalKeeper() { return STRUCT_OFFSET(ASoccerTeam, GoalKeeper); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer1() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer1); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer2() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer2); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer3() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer3); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer4() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer4); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer5() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer5); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer6() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer6); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer7() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer7); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer8() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer8); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer9() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer9); } \
	FORCEINLINE static uint32 __PPO__FieldPlayer10() { return STRUCT_OFFSET(ASoccerTeam, FieldPlayer10); } \
	FORCEINLINE static uint32 __PPO__Team() { return STRUCT_OFFSET(ASoccerTeam, Team); } \
	FORCEINLINE static uint32 __PPO__XSupportDots() { return STRUCT_OFFSET(ASoccerTeam, XSupportDots); } \
	FORCEINLINE static uint32 __PPO__YSupportDots() { return STRUCT_OFFSET(ASoccerTeam, YSupportDots); } \
	FORCEINLINE static uint32 __PPO__AttemptsToFindValidStrike() { return STRUCT_OFFSET(ASoccerTeam, AttemptsToFindValidStrike); } \
	FORCEINLINE static uint32 __PPO__TimeBetweenSupportSpotCalculations() { return STRUCT_OFFSET(ASoccerTeam, TimeBetweenSupportSpotCalculations); } \
	FORCEINLINE static uint32 __PPO__CanPassStrength() { return STRUCT_OFFSET(ASoccerTeam, CanPassStrength); } \
	FORCEINLINE static uint32 __PPO__CanScoreFromPositionStrength() { return STRUCT_OFFSET(ASoccerTeam, CanScoreFromPositionStrength); } \
	FORCEINLINE static uint32 __PPO__OptimalDistanceFromControllingPlayer() { return STRUCT_OFFSET(ASoccerTeam, OptimalDistanceFromControllingPlayer); } \
	FORCEINLINE static uint32 __PPO__DistanceFromControllingPlayerStrength() { return STRUCT_OFFSET(ASoccerTeam, DistanceFromControllingPlayerStrength); } \
	FORCEINLINE static uint32 __PPO__KickingRadius() { return STRUCT_OFFSET(ASoccerTeam, KickingRadius); } \
	FORCEINLINE static uint32 __PPO__SecondsPerKick() { return STRUCT_OFFSET(ASoccerTeam, SecondsPerKick); } \
	FORCEINLINE static uint32 __PPO__InterceptionRangeScalingFactor() { return STRUCT_OFFSET(ASoccerTeam, InterceptionRangeScalingFactor); } \
	FORCEINLINE static uint32 __PPO__SeparationDampening() { return STRUCT_OFFSET(ASoccerTeam, SeparationDampening); } \
	FORCEINLINE static uint32 __PPO__AheadOfAttackerStrength() { return STRUCT_OFFSET(ASoccerTeam, AheadOfAttackerStrength); } \
	FORCEINLINE static uint32 __PPO__MaxTimeForPass() { return STRUCT_OFFSET(ASoccerTeam, MaxTimeForPass); } \
	FORCEINLINE static uint32 __PPO__ControlLossDistance() { return STRUCT_OFFSET(ASoccerTeam, ControlLossDistance); } \
	FORCEINLINE static uint32 __PPO__AdvancementSpeed() { return STRUCT_OFFSET(ASoccerTeam, AdvancementSpeed); } \
	FORCEINLINE static uint32 __PPO__FallBackSpeed() { return STRUCT_OFFSET(ASoccerTeam, FallBackSpeed); } \
	FORCEINLINE static uint32 __PPO__MaximumAdvancementDistance() { return STRUCT_OFFSET(ASoccerTeam, MaximumAdvancementDistance); } \
	FORCEINLINE static uint32 __PPO__DribbleDistance() { return STRUCT_OFFSET(ASoccerTeam, DribbleDistance); } \
	FORCEINLINE static uint32 __PPO__ReceivingOffset() { return STRUCT_OFFSET(ASoccerTeam, ReceivingOffset); } \
	FORCEINLINE static uint32 __PPO__TeamRoleDistances() { return STRUCT_OFFSET(ASoccerTeam, TeamRoleDistances); } \
	FORCEINLINE static uint32 __PPO__DisplaySupportSpots() { return STRUCT_OFFSET(ASoccerTeam, DisplaySupportSpots); } \
	FORCEINLINE static uint32 __PPO__PassAngleDot() { return STRUCT_OFFSET(ASoccerTeam, PassAngleDot); }


#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_9_PROLOG
#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_PRIVATE_PROPERTY_OFFSET \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_RPC_WRAPPERS \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_INCLASS \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_PRIVATE_PROPERTY_OFFSET \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_RPC_WRAPPERS_NO_PURE_DECLS \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_INCLASS_NO_PURE_DECLS \
	SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h_12_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID SoccerSimulator_Source_SoccerSimulator_SoccerTeam_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
