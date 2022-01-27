// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkFunGameMode.h"
#include "NetworkFunCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetworkFunGameMode::ANetworkFunGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
