// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	C++ class header boilerplate exported from UnrealHeaderTool.
	This is automatically generated by the tools.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef SOCCERSIMULATOR_FieldPlayer_generated_h
#error "FieldPlayer.generated.h already included, missing '#pragma once' in FieldPlayer.h"
#endif
#define SOCCERSIMULATOR_FieldPlayer_generated_h

#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_RPC_WRAPPERS
#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_RPC_WRAPPERS_NO_PURE_DECLS
#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_INCLASS_NO_PURE_DECLS \
	private: \
	static void StaticRegisterNativesAFieldPlayer(); \
	friend SOCCERSIMULATOR_API class UClass* Z_Construct_UClass_AFieldPlayer(); \
	public: \
	DECLARE_CLASS(AFieldPlayer, APlayerBase, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/SoccerSimulator"), NO_API) \
	DECLARE_SERIALIZER(AFieldPlayer) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_INCLASS \
	private: \
	static void StaticRegisterNativesAFieldPlayer(); \
	friend SOCCERSIMULATOR_API class UClass* Z_Construct_UClass_AFieldPlayer(); \
	public: \
	DECLARE_CLASS(AFieldPlayer, APlayerBase, COMPILED_IN_FLAGS(0), 0, TEXT("/Script/SoccerSimulator"), NO_API) \
	DECLARE_SERIALIZER(AFieldPlayer) \
	/** Indicates whether the class is compiled into the engine */ \
	enum {IsIntrinsic=COMPILED_IN_INTRINSIC};


#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API AFieldPlayer(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(AFieldPlayer) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AFieldPlayer); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AFieldPlayer); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AFieldPlayer(AFieldPlayer&&); \
	NO_API AFieldPlayer(const AFieldPlayer&); \
public:


#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API AFieldPlayer(AFieldPlayer&&); \
	NO_API AFieldPlayer(const AFieldPlayer&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, AFieldPlayer); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(AFieldPlayer); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(AFieldPlayer)


#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_PRIVATE_PROPERTY_OFFSET
#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_8_PROLOG
#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_PRIVATE_PROPERTY_OFFSET \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_RPC_WRAPPERS \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_INCLASS \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_PRIVATE_PROPERTY_OFFSET \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_RPC_WRAPPERS_NO_PURE_DECLS \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_INCLASS_NO_PURE_DECLS \
	SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h_11_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID SoccerSimulator_Source_SoccerSimulator_FieldPlayer_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS