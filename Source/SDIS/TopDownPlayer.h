// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TopDownPlayer.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
UCLASS()
class SDIS_API ATopDownPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATopDownPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	class ATopDownPlayerController* PlayerController;

#pragma region Player Component
	UPROPERTY(EditAnywhere)
		class UBoxComponent* PlayerBoxComponent;

	UPROPERTY(EditAnywhere)
		class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* CameraComponent;
#pragma endregion 
	
#pragma region Inputs 
	UPROPERTY(EditDefaultsOnly, Category="Input/Movement");
		UInputMappingContext* InputContext;	// Error here, but it's working, probably a RiderPlugin error

	// World movement mapping with the ZQSD / WASD keys
	UPROPERTY(EditDefaultsOnly, Category="Input/Movement/Keyboard");
		UInputAction* KeyboardMovementInputAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Input/Movement/Keyboard");
		UInputAction* ShiftKeyboardMovementInputAction;

	// World movement mapping with the mouse
	UPROPERTY(EditDefaultsOnly, Category="Input/Movement/Mouse");
		UInputAction* MouseMovementInputAction;
	
	// World movement mapping with the mouse
	UPROPERTY(EditDefaultsOnly, Category="Input/Movement/Mouse");
		UInputAction* TriggerMouseMovementInputAction;

	// World camera zoom with the scrollwheel
	UPROPERTY(EditDefaultsOnly, Category="Input/Camera/Zoom/Scrollwheel");
		UInputAction* ScrollwheelCameraZoomInputAction;

	// World camera zoom mapping with the AE / QE keys
	UPROPERTY(EditDefaultsOnly, Category="Input/Camera/Zoom/Keyboard");
		UInputAction* KeyboardCameraZoomInputAction;

	// World camera rotation mapping with the scrollwheel
	UPROPERTY(EditDefaultsOnly, Category="Input/Camera/Rotation");
		UInputAction* ScrollwheelCameraRotationInputAction;

	// World camera rotation mapping 
	UPROPERTY(EditDefaultsOnly, Category="Input/Camera/Rotation");
		UInputAction* MouseCameraRotationInputAction;

	bool isRotating = false;
	bool isShiftHold = false;
	bool isGrabbing = false;

	FVector2D InitialPosition = FVector2D(0,0);
	FVector2D Direction = FVector2D(0,0);
	float Distance = 0.0f;
	
	void HandleMovement(const FInputActionValue& Value);
	void CameraZoomAction(const FInputActionValue& Value);
	void CameraRotationAction(const FInputActionValue& Value);
	void TriggerCameraRotationAction(const FInputActionValue& Value);
	bool IsControlsConfigured();
	void TriggerShiftDown(const FInputActionValue& Value);
	void GrabMovement(const FInputActionValue& Value);
	void TriggerGrabMovement(const FInputActionValue& Value);
	void BindActions(UEnhancedInputComponent* PlayerEnhancedInput);
	FVector2D GetMousePosition();
#pragma endregion 
};
