// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkFunCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ANetworkFunCharacter

ANetworkFunCharacter::ANetworkFunCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANetworkFunCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	acc = 2.0f;


	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &ANetworkFunCharacter::Pause);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANetworkFunCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetworkFunCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANetworkFunCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANetworkFunCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ANetworkFunCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ANetworkFunCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANetworkFunCharacter::OnResetVR);
}

void ANetworkFunCharacter::CreateMenu() 
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Black, TEXT("Allo \U0001f604"));
	if (PauseMenuUIClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Black, TEXT("Allo2 \U0001f604"));
		if (!PauseMenuWidget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Black, TEXT("Allo3 \U0001f604"));
			PauseMenuWidget = CreateWidget<UMyPauseBPBase>(GetWorld()->GetGameInstance(), PauseMenuUIClass);
			if (!PauseMenuWidget)
			{
				return;
			}

			PauseMenuWidget->AddToViewport();
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void ANetworkFunCharacter::OnResetVR()
{
	// If NetworkFun is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in NetworkFun.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ANetworkFunCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ANetworkFunCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ANetworkFunCharacter::Pause()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TEXT("Allo \U0001f604"));
	if (PauseMenuWidget)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, TEXT("Allo9 \U0001f604"));
		APlayerController* PC = Cast<APlayerController>(GetController());
		switch (PauseMenuWidget->GetVisibility())
		{
		case ESlateVisibility::Hidden:
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;
			}
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
			break;
		default:
			if (PC)
			{
				PC->bShowMouseCursor = false;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;
			}
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ANetworkFunCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANetworkFunCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANetworkFunCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void ANetworkFunCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value * acc);
	}
}

void ANetworkFunCharacter::StartSprint()
{
	FString TheFloatStr = FString::SanitizeFloat(acc);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	TheFloatStr = FString::SanitizeFloat(GetCharacterMovement()->MaxWalkSpeed);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, *TheFloatStr);
	UWorld* wworld = GEngine->GameViewport->GetWorld();
	TheFloatStr = FString::SanitizeFloat(wworld->URL.Port);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Black, *TheFloatStr);

	GetCharacterMovement()->MaxWalkSpeed *= acc;
	GetCharacterMovement()->JumpZVelocity *= acc;

	TheFloatStr = FString::SanitizeFloat(GetCharacterMovement()->MaxWalkSpeed);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Orange, *TheFloatStr);
}

void ANetworkFunCharacter::StopSprint()
{
	FString TheFloatStr = FString::SanitizeFloat(acc);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
	TheFloatStr = FString::SanitizeFloat(GetCharacterMovement()->MaxWalkSpeed);
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, *TheFloatStr);

	GetCharacterMovement()->MaxWalkSpeed /= acc;
	GetCharacterMovement()->JumpZVelocity /= acc;

	TheFloatStr = FString::SanitizeFloat(GetCharacterMovement()->GetMaxSpeed());
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Orange, *TheFloatStr);
}
