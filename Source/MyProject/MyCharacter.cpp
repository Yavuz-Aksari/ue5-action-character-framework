#include "MyCharacter.h"
#include "MyProject.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"




AMyCharacter::AMyCharacter()
{

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent); 
	SpringArm->TargetArmLength = 400.0f; 
	SpringArm->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->AirControl = 0.2f;

	PrimaryActorTick.bCanEverTick = true;

}


void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//input mapping context added to priority 0 so it is the foundation
			Subsystem->AddMappingContext(InputMapping, 0);
			UE_LOG(GameLog, Log, TEXT("IMC succsesfully added."));
			return;
		}
		UE_LOG(GameLog, Error, TEXT("InputMapping is null: assign IMC in Blueprint."));
	}
}


void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
	
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyCharacter::StopMoving);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AMyCharacter::StopMoving);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AMyCharacter::Dash);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCharacter::Attack);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &AMyCharacter::Parry);



	}
	
}

void AMyCharacter::ResolveMovementState(bool bForce)
{
	if (!bForce && CurrentMovementState == EMovementState::Dashing)
	{
		return;
	}

	if (GetCharacterMovement()->IsFalling())
	{
		SetMovementState(EMovementState::Falling);
	}
	else if (bIsMoveInputPressed)
	{
		SetMovementState(EMovementState::Walking);
	}
	else
	{
		SetMovementState(EMovementState::Idle);
		
	}
}

bool AMyCharacter::CanChangeMovementState(EMovementState NewState)
{
	switch (NewState)
	{
	case EMovementState::Dashing:
		if (CurrentMovementState == EMovementState::Falling)
		{
			UE_LOG(GameLog, Warning, TEXT("Dash	denied (Reason: Falling)"));
			return false;
		}
		break;

	default:
		break;
	}

	return true;
}

bool AMyCharacter::CanChangeActionState(EActionState NewState)
{
	switch (NewState)
	{
	case EActionState::Attacking:
		if (CurrentMovementState == EMovementState::Falling)
		{
			UE_LOG(GameLog, Warning, TEXT("Attack denied (Reason: Falling)"));
			return false;
		}
		if (CurrentMovementState == EMovementState::Dashing)
		{
			UE_LOG(GameLog, Warning, TEXT("Attack denied (Reason: Dashing)"));
			return false;
		}
		break;

	case EActionState::Parrying:
		if (CurrentMovementState == EMovementState::Dashing)
		{
			UE_LOG(GameLog, Warning, TEXT("Parry denied (Reason: Dashing)"));
			return false;
		}
		break;

	default:
		break;
	}

	return true;
}

void AMyCharacter::Move(const FInputActionValue& Value)
{

	const FVector2D MovementValue = Value.Get<FVector2D>();

	if (MovementValue.IsNearlyZero())
	{
		return;
	}

	bIsMoveInputPressed = true;

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Right, MovementValue.X);

		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, MovementValue.Y);
	}
	ResolveMovementState(false);
}

void AMyCharacter::StopMoving(const FInputActionValue& Value)
{
	if (CurrentMovementState == EMovementState::Dashing)
	{
		return;
	}
	
	bIsMoveInputPressed = false;

	
	ResolveMovementState(false);
}

void AMyCharacter::Look(const FInputActionValue& Value) {

	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (Controller) {

		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AMyCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	ResolveMovementState(false);
}

void AMyCharacter::Dash()
{
	if (!CanChangeMovementState(EMovementState::Dashing))
	{
		return;
	}
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < LastDashTime + DashCooldown){
	
		UE_LOG(GameLog, Log, TEXT("Dash still on cooldown: %f"), (LastDashTime + DashCooldown) - CurrentTime);
		return;
	}

	FVector DashDirection = GetCharacterMovement()->GetLastInputVector();
	FString DashType = GetCharacterMovement()->IsFalling() ? TEXT("Air") : TEXT("Ground");
	float AppliedForce = GetCharacterMovement()->IsFalling() ? DashForce_air : DashForce_ground;

	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}

	FVector LaunchVelocity = DashDirection * AppliedForce;
	LaunchCharacter(LaunchVelocity, true, true);
	SetMovementState(EMovementState::Dashing);
	GetWorld()->GetTimerManager().SetTimer(DashTimerHandle, this, &AMyCharacter::StopDashing, 0.2f, false);

	LastDashTime = CurrentTime;

}

void AMyCharacter::SetMovementState(EMovementState NewState)
{
	//Don't bother changing the state if its already in the same state.
	if (CurrentMovementState == NewState)
	{
		return;
	}

	if (!CanChangeMovementState(NewState))
	{
		return;
	}
	
	FString OldStateName = UEnum::GetValueAsString(CurrentMovementState);
	FString NewStateName = UEnum::GetValueAsString(NewState);

	UE_LOG(GameLog, Log, TEXT("State Changed: %s -> %s"), *OldStateName, *NewStateName);

	CurrentMovementState = NewState;
}

void AMyCharacter::SetActionState(EActionState NewState)
{
	//Don't bother changing the state if its already in the same state.
	if (CurrentActionState == NewState)
	{
		return;
	}

	if (!CanChangeActionState(NewState))
	{
		return;
	}

	FString OldStateName = UEnum::GetValueAsString(CurrentActionState);
	FString NewStateName = UEnum::GetValueAsString(NewState);

	UE_LOG(GameLog, Log, TEXT("State Changed: %s -> %s"), *OldStateName, *NewStateName);

	CurrentActionState = NewState;
}

void AMyCharacter::StopDashing()
{
	ResolveMovementState(true);
}

void AMyCharacter::Attack()
{
	if (!CanChangeActionState(EActionState::Attacking))
	{
		return; 
	}

	
	SetActionState(EActionState::Attacking);

	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AMyCharacter::StopAttacking, 0.5f, false);
}

void AMyCharacter::StopAttacking()
{
	SetActionState(EActionState::None);
}

void AMyCharacter::Parry()
{
	if (!CanChangeActionState(EActionState::Parrying))
	{
		return;
	}

	SetActionState(EActionState::Parrying);

	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &AMyCharacter::StopParrying, 0.3f, false);
}

void AMyCharacter::StopParrying()
{
	SetActionState(EActionState::None);
}