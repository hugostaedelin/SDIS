// Fill out your copyright notice in the Description page of Project Settings.
#include "TopDownPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "TopDownPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ATopDownPlayer::ATopDownPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerBoxComponent"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(PlayerBoxComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ATopDownPlayer::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ATopDownPlayerController>(GetController());
	AddControllerPitchInput(22.5f);
}

// Called every frame
void ATopDownPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isGrabbing)
	{
		AddActorLocalOffset(FVector(Direction.Y*10, Direction.X*10, 0), true);
	}

}

// Called to bind functionality to input
void ATopDownPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController()) 
	{
		if (UEnhancedInputLocalPlayerSubsystem* LocalPlayerInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!IsControlsConfigured())
			{
				UE_LOG(LogTemp, Error, TEXT("Controls not configured in the blueprints"));
				return;
			}

			LocalPlayerInputSubsystem->AddMappingContext(InputContext, 0);

			if (UEnhancedInputComponent* PlayerEnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				BindActions(PlayerEnhancedInput);
			}
		}
	}
	
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATopDownPlayer::HandleMovement(const FInputActionValue& Value)
{
	float Delta = GetWorld()->GetDeltaSeconds();
	if (isShiftHold)
	{
		AddActorLocalOffset(FVector(Value[0]*Delta*2000, Value[1]*Delta*2000, 0), true);
	} else
	{
		AddActorLocalOffset(FVector(Value[0]*Delta*1000, Value[1]*Delta*1000, 0), true);		
	}
}

void ATopDownPlayer::CameraZoomAction(const FInputActionValue& Value)
{
	float newZoom = SpringArmComponent->TargetArmLength + Value.Get<float>() * -100;
	if (newZoom > 3000 || newZoom < 500)
	{
		return;
	}
	SpringArmComponent->TargetArmLength = newZoom;
}

void ATopDownPlayer::CameraRotationAction(const FInputActionValue& value)
{
	if (isRotating)
	{
		AddControllerPitchInput(value[1] * - 1);
		AddControllerYawInput(value[0]);

		FRotator controllerRotation = GetController()->GetControlRotation();
		FRotator newRotation = FRotator(0, controllerRotation.Yaw, 0);

		PlayerBoxComponent->SetRelativeRotation(newRotation);
	}

}

void ATopDownPlayer::TriggerCameraRotationAction(const FInputActionValue& Value)
{
	isRotating = Value.Get<bool>();
}

bool ATopDownPlayer::IsControlsConfigured()
{
	return InputContext &&  KeyboardMovementInputAction && ShiftKeyboardMovementInputAction && MouseMovementInputAction && ScrollwheelCameraZoomInputAction && KeyboardCameraZoomInputAction && ScrollwheelCameraRotationInputAction && MouseCameraRotationInputAction;
}

void ATopDownPlayer::TriggerShiftDown(const FInputActionValue& Value)
{
	isShiftHold = Value.Get<bool>();
}

void ATopDownPlayer::GrabMovement(const FInputActionValue& Value)
{
	if (isGrabbing)
	{
		FVector2D mousePosition = GetMousePosition();
		FVector2D direction = mousePosition - InitialPosition;
		Distance = FVector2D::Distance(mousePosition, InitialPosition);
		Distance = Distance > 20 ? 20 : Distance;
		Direction = (direction*FVector2D(1, -1));
		Direction.Normalize();
	}
}

void ATopDownPlayer::TriggerGrabMovement(const FInputActionValue& Value)
{
	isGrabbing = Value.Get<bool>();
	if (isGrabbing)
	{
		InitialPosition = GetMousePosition();
	} else
	{
		Direction = FVector2D::Zero();
	}
}

void ATopDownPlayer::BindActions(UEnhancedInputComponent* PlayerEnhancedInput)
{
	PlayerEnhancedInput->BindAction(KeyboardMovementInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::HandleMovement);
	PlayerEnhancedInput->BindAction(ShiftKeyboardMovementInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::TriggerShiftDown);
	
	PlayerEnhancedInput->BindAction(MouseMovementInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::GrabMovement);
	PlayerEnhancedInput->BindAction(TriggerMouseMovementInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::TriggerGrabMovement);
				
	PlayerEnhancedInput->BindAction(KeyboardCameraZoomInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::CameraZoomAction);
	PlayerEnhancedInput->BindAction(ScrollwheelCameraZoomInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::CameraZoomAction);
				
	PlayerEnhancedInput->BindAction(ScrollwheelCameraRotationInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::TriggerCameraRotationAction);
	PlayerEnhancedInput->BindAction(MouseMovementInputAction, ETriggerEvent::Triggered, this, &ATopDownPlayer::CameraRotationAction);
}

FVector2D ATopDownPlayer::GetMousePosition()
{
	float x;
	float y;
	PlayerController->GetMousePosition(x, y);
	return FVector2D(x, y);
}