#pragma once



#include "CoreMinimal.h"
#include "GameFramework/Character.h"


UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Walking     UMETA(DisplayName = "Walking"),
	Running     UMETA(DisplayName = "Running"),
	Jumping     UMETA(DisplayName = "Jumping"),
	Falling     UMETA(DisplayName = "Falling"),
	Dashing     UMETA(DisplayName = "Dashing"),
	Recovering  UMETA(DisplayName = "Recovering")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None        UMETA(DisplayName = "None"),
	Attacking   UMETA(DisplayName = "Attacking"),
	Blocking    UMETA(DisplayName = "Blocking"),
	Parrying    UMETA(DisplayName = "Parrying"),
	Casting     UMETA(DisplayName = "Casting"),
	Stunned     UMETA(DisplayName = "Stunned")
};

#include "MyCharacter.generated.h"




class UInputMappingContext;
class UInputAction;
class UInputComponent;
class USpringArmComponent;
class UCameraComponent;



UCLASS()
class MYPROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AMyCharacter();
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StopMoving(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
	
	float LastDashTime = 0.0f;
	void Dash();
	void StopDashing();

	void SetMovementState(EMovementState NewState);
	void SetActionState(EActionState NewState);
	void ResolveMovementState(bool bforce = false);
	bool bIsMoveInputPressed = false;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext>InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> ThirdPersonCamera;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashForce_ground = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashForce_air = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DashCooldown = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EMovementState CurrentMovementState = EMovementState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	EActionState CurrentActionState = EActionState::None;

	FTimerHandle DashTimerHandle;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	


public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
