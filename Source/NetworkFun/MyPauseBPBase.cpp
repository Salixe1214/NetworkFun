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
    UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("LobyMap")));
}

void UMyPauseBPBase::OnQuitClick()
{
    UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningLocalPlayer()->GetPlayerController(GetWorld()), EQuitPreference::Quit, false);
}
