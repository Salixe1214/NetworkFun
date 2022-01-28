// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "MyPauseBPBase.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKFUN_API UMyPauseBPBase : public UUserWidget
{
	GENERATED_BODY()

public:

    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
        class UButton* Quit;
    UPROPERTY(meta = (BindWidget))
        class UButton* Menu;

    UFUNCTION()
        void OnMenuClick();
    UFUNCTION()
        void OnQuitClick();
};
