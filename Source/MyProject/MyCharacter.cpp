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

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	UE_LOG(GameLog, Log, TEXT("Character Sucsessfully spawned."));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using MyCharacter."));


	
	
}


void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
	
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);

		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AMyCharacter::Dash);

	}
	
}
void AMyCharacter::Move(const FInputActionValue& Value)
{

	const FVector2D MovementValue = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Right, MovementValue.X);

		const FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Forward, MovementValue.Y);
	}
}

void AMyCharacter::Look(const FInputActionValue& Value) {

	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (Controller) {

		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AMyCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UE_LOG(GameLog, Log, TEXT("Character Landed"));
}

void AMyCharacter::Falling()
{
	Super::Falling();

	UE_LOG(GameLog, Log, TEXT("Character in air"));
}

void AMyCharacter::Dash()
{
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

	LastDashTime = CurrentTime;

	UE_LOG(GameLog, Log, TEXT("Dashed! (%s)"), *DashType);
}