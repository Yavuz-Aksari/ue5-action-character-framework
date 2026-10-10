// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	// Sets default values for this character's properties
	AMyCharacter();
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	
	void Look(const FInputActionValue& Value);
	
	virtual void Landed(const FHitResult& Hit) override;
	virtual void Falling() override;

	float LastDashTime = 0.0f;
	void Dash();

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

	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
