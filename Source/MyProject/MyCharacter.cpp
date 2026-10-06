// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "InputActionValue.h"
#include "InputMappingContext.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Get the player controller for this character
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get the enhanced input local player subsystem and add a new input mapping context to it
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
			UE_LOG(LogTemp, Warning, TEXT("IMC ADDED!"));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("CHAR SPAWNED!"));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using MyCharacter."));


	
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// --- New Code Start --- 
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

		// --- New Code Start --- 

		// Bind Jump Actions
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);
	}
	// --- New Code End --- 
}
void AMyCharacter::Move(const FInputActionValue& Value)
{
	// --- New Code Start --- 

	// 2D Vector of movement values returned from the input action
	const FVector2D MovementValue = Value.Get<FVector2D>();

	if (Controller)
	{
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);
	}
}