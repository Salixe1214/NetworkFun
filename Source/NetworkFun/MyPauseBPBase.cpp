// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPauseBPBase.h"

void UMyPauseBPBase::NativeConstruct()
{
    Super::NativeConstruct();

    Quit->OnClicked.AddDynamic(this, &UMyPauseBPBase::OnQuitClick);
    Menu->OnClicked.AddDynamic(this, &UMyPauseBPBase::OnMenuClick);
}

void UMyPauseBPBase::OnMenuClick()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TEXT("Allo \U0001f604"));
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LobyMap")));
}

void UMyPauseBPBase::OnQuitClick()
{
    GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TEXT("Allo \U0001f604"));
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningLocalPlayer()->GetPlayerController(GetWorld()), EQuitPreference::Quit, false);
}
