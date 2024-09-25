// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "../Components/TDSStaminaComponent.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/TextRenderComponent.h"

DEFINE_LOG_CATEGORY_STATIC(TDSCharacterLog, All, All);

ATDSCharacter::ATDSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movementö
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	StaminaComponent = CreateDefaultSubobject<UTDSStaminaComponent>("StaminaComponent");
	StaminaTextComponent = CreateDefaultSubobject<UTextRenderComponent>("StaminaTextComponent");
	StaminaTextComponent->SetupAttachment(GetRootComponent());	
}

void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitWeapon();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	check(StaminaComponent);
	check(StaminaTextComponent);

	OnStaminaChanged(StaminaComponent->GetStamina());
	
	StaminaComponent->OnStaminaEmpty.AddUObject(this, &ATDSCharacter::OnStaminaEmpty);
	StaminaComponent->OnStaminaChanged.AddUObject(this, &ATDSCharacter::OnStaminaChanged);
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CurrentCursor)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}
	
	MovementTick(DeltaSeconds);
}

void ATDSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATDSCharacter::InputAxisX);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATDSCharacter::InputAxisY);
	PlayerInputComponent->BindAxis(TEXT("MouseWheel"), this, &ATDSCharacter::CameraSlide);
	PlayerInputComponent->BindAction(TEXT("Walk"), IE_Pressed, this, &ATDSCharacter::OnStartWalking);
	PlayerInputComponent->BindAction(TEXT("Walk"), IE_Released, this, &ATDSCharacter::OnStartRunning);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Pressed, this, &ATDSCharacter::OnStartAiming);
	PlayerInputComponent->BindAction(TEXT("Aim"), IE_Released, this, &ATDSCharacter::OnStartRunning);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ATDSCharacter::OnStartSprinting);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ATDSCharacter::OnStopSprinting);
	PlayerInputComponent->BindAction(TEXT("FireEvent"), IE_Pressed, this, &ATDSCharacter::InputAttackPressed);
	PlayerInputComponent->BindAction(TEXT("FireEvent"), IE_Released, this, &ATDSCharacter::InputAttackReleased);
}

void ATDSCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATDSCharacter::InputAxisY(float Value)
{
	AxisY = Value;
}

void ATDSCharacter::OnStartWalking()
{
	ChangeMovementState(EMovementState::Walk_State);
}

void ATDSCharacter::OnStartAiming()
{
	ChangeMovementState(EMovementState::Aim_State);
}

void ATDSCharacter::OnStartRunning()
{
	ChangeMovementState(EMovementState::Run_State);
}

void ATDSCharacter::OnStartSprinting()
{
	if ((GetMovementDirection() <= SprintAngleThreshold) && StaminaComponent->CanSprinting())
	{
		WantsToSprint = true;
		if (!GetVelocity().IsZero())
		{
			StaminaComponent->ChangeSpamina(WantsToSprint);
		}
		ChangeMovementState(EMovementState::Sprint_State);
	}
	else
	{
		OnStopSprinting();
	}	
}

void ATDSCharacter::OnStopSprinting()
{
	WantsToSprint = false;
	StaminaComponent->ChangeSpamina(WantsToSprint);
	ChangeMovementState(EMovementState::Run_State);
}

float ATDSCharacter::GetMovementDirection() const
{
	FVector VelocityVector = GetVelocity();
	VelocityVector.Z = 0;

	FVector ForwardVector = GetActorForwardVector();
	ForwardVector.Z = 0;

	if (VelocityVector.IsNearlyZero() || ForwardVector.IsNearlyZero())
	{
		return 0.0f;
	}
	
	VelocityVector.Normalize();
	ForwardVector.Normalize();

	float DotProduct = FVector::DotProduct(VelocityVector, ForwardVector);
	float AngleRadians = FMath::Acos(DotProduct);
	float AnlgeDegrees = FMath::RadiansToDegrees(AngleRadians);

	return AnlgeDegrees;
}

bool ATDSCharacter::IsSprinting() const
{
	return WantsToSprint && ((GetMovementDirection() <= SprintAngleThreshold)) && !GetVelocity().IsZero();
}

void ATDSCharacter::MovementTick(float DeltaSeconds)
{
	const FRotator Rotation = TopDownCameraComponent->GetComponentRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector DirectionForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector DirectionRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(DirectionForward, AxisX);
	AddMovementInput(DirectionRight, AxisY);

	if (MovementState == EMovementState::Sprint_State)
	{
		FVector myRotationVector = GetVelocity();
		FRotator myRotator = myRotationVector.ToOrientationRotator();
		SetActorRotation((FQuat(myRotator)));
	}
	else
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			FHitResult ResultHit;
			PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel1, false, ResultHit);

			float FindRatatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			SetActorRotation(FQuat(FRotator(0.0f, FindRatatorResultYaw, 0.0f)));
		}
	}
}

void ATDSCharacter::CharacterUpdate()
{
	float ResSpeed = 600.0f;
	switch (MovementState)
	{
	case EMovementState::Aim_State: ResSpeed = MovementInfo.AimSpeed; break;
	case EMovementState::Walk_State: ResSpeed = MovementInfo.WalkSpeed; break;
	case EMovementState::Run_State: ResSpeed = MovementInfo.RunSpeed; break;
	case EMovementState::Sprint_State: ResSpeed = MovementInfo.SprintSpeed; break;
	default: break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

void ATDSCharacter::ChangeMovementState(EMovementState InMovementState)
{
	MovementState = InMovementState;
	CharacterUpdate();
}

void ATDSCharacter::InputAttackPressed()
{
	AttackCharEvent(true);
}

void ATDSCharacter::InputAttackReleased()
{
	AttackCharEvent(false);
}

void ATDSCharacter::AttackCharEvent(bool bIsFiring)
{
	ATDS_WeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->SetWeaponStateFire(bIsFiring);
	}
	else
	{
		UE_LOG(TDSCharacterLog, Warning, TEXT("ATDSCharacter::AttackCharEvent - CurrentWeapon is NULL!"));
	}
}

void ATDSCharacter::CameraSlide(float Value)
{
	float DistanceToCamera = CameraBoom->TargetArmLength;
	if (Value == 0 || !GetWorld())
	{
		return;
	}
	if (Value < 0)
	{
		if (DistanceToCamera + CameraStep <= CameraHeightMax)
		{
			if (!DoSmoothScrolling)
			{
				CameraBoom->TargetArmLength = DistanceToCamera + CameraStep;
			}
			else
			{
				DoSlideUp = true;
				GetWorld()->GetTimerManager().SetTimer(CameraTimerHandle, this, &ATDSCharacter::CameraScroll, TimerStep, true);
			}
		}
	}
	else if (Value > 0)
	{
		if (DistanceToCamera - CameraStep >= CameraHeightMin)
		{
			if (!DoSmoothScrolling)
			{
				CameraBoom->TargetArmLength = DistanceToCamera - CameraStep;
			}
			else
			{
				DoSlideUp = false;
				GetWorld()->GetTimerManager().SetTimer(CameraTimerHandle, this, &ATDSCharacter::CameraScroll, TimerStep, true);
			}
		}
	}
}

void ATDSCharacter::CameraScroll()
{
	CurrentCameraDistance += SmoothCameraStep;
	float Distance = CameraBoom->TargetArmLength;
	if (DoSlideUp)
	{
		Distance += SmoothCameraStep;
	}
	else
	{
		Distance -= SmoothCameraStep;
	}
	CameraBoom->TargetArmLength = Distance;

	if (CurrentCameraDistance >= CameraStep)
	{
		CurrentCameraDistance = 0;
		GetWorld()->GetTimerManager().ClearTimer(CameraTimerHandle);
	}
}

void ATDSCharacter::InitWeapon()
{
	FVector SpawnLocation = FVector(0);
	FRotator SpawnRotation = FRotator(0);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	auto* MyWeapon = Cast<ATDS_WeaponDefault>(GetWorld()->SpawnActor(InitWeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
	
	if (MyWeapon)
	{
		FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
		CurrentWeapon = MyWeapon;
		CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
	}
}

void ATDSCharacter::OnStaminaEmpty()
{
	OnStopSprinting();
}

void ATDSCharacter::OnStaminaChanged(float Stamina)
{
	StaminaTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Stamina)));
}

UDecalComponent* ATDSCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}

ATDS_WeaponDefault* ATDSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}
