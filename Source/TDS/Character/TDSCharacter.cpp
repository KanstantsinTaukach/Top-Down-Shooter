// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "../Components/TDSStaminaComponent.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "../Game/TDSGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

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

	InventoryComponent = CreateDefaultSubobject<UTDSInventoryComponent>("InventoryComponent");
	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATDSCharacter::InitWeapon);
	}
}

void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	check(StaminaComponent);

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

	PlayerInputComponent->BindAction(TEXT("ReloadEvent"), IE_Released, this, &ATDSCharacter::TryReloadWeapon);

	PlayerInputComponent->BindAction(TEXT("SwitchNextWeapon"), IE_Pressed, this, &ATDSCharacter::TrySwitchWeapon);
	PlayerInputComponent->BindAction(TEXT("SwitchPreviousWeapon"), IE_Pressed, this, &ATDSCharacter::TrySwitchPreviousWeapon);
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

			if (CurrentWeapon)
			{
				FVector Displacement = FVector(0);
				switch (MovementState)
				{
				case EMovementState::Aim_State: 
					Displacement = FVector(0.0f, 0.0f, 160.0f); 
					CurrentWeapon->ShouldReduceDispersion = true; 
					break;
				case EMovementState::Walk_State: 
					Displacement = FVector(0.0f, 0.0f, 120.0f); 
					CurrentWeapon->ShouldReduceDispersion = false; 
					break;
				case EMovementState::Run_State: 
					Displacement = FVector(0.0f, 0.0f, 120.0f); 
					CurrentWeapon->ShouldReduceDispersion = false; 
					break;
				case EMovementState::Sprint_State: break;
				default: break;
				}

				CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
			}	
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

	//Weapon state update
	ATDS_WeaponDefault* MyWeapon = GetCurrentWeapon();
	if (MyWeapon)
	{
		MyWeapon->UpdateStateWeapon(MovementState);
	}
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

void ATDSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	auto GameInstance = Cast<UTDSGameInstance>(GetGameInstance());
	FWeaponInfo MyWeaponInfo;
	if (GameInstance)
	{
		if (GameInstance->GetWeaponInfoByName(IdWeaponName, MyWeaponInfo))
		{
			if (MyWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				auto* MyWeapon = Cast<ATDS_WeaponDefault>(GetWorld()->SpawnActor(MyWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));

				if (MyWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					CurrentWeapon = MyWeapon;
					CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));

					MyWeapon->WeaponSettings = MyWeaponInfo;
					MyWeapon->AdditionalWeaponInfo.Round = MyWeaponInfo.MaxRound;
										
					MyWeapon->UpdateStateWeapon(MovementState);




					// Need to remove
					if (InventoryComponent)
					{
						CurrentIndexWeapon = InventoryComponent->GetWeaponIndexSlotByName(IdWeaponName);
					}





					MyWeapon->OnWeaponReloadStart.AddDynamic(this, &ATDSCharacter::WeaponReloadStart);
					MyWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATDSCharacter::WeaponReloadEnd);
					MyWeapon->OnWeaponFire.AddDynamic(this, &ATDSCharacter::WeaponFire);
				}
			}
		}
		else
		{
			UE_LOG(TDSCharacterLog, Warning, TEXT("ATDSCharacter::InitWeapon - Weapon not found if table"));
		}
	}	
}

void ATDSCharacter::OnStaminaEmpty()
{
	OnStopSprinting();
}

void ATDSCharacter::OnStaminaChanged(float Stamina)
{
	
}

UDecalComponent* ATDSCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}

ATDS_WeaponDefault* ATDSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATDSCharacter::TryReloadWeapon()
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSettings.MaxRound)
	{
		CurrentWeapon->InitReload();
	}
}

void ATDSCharacter::WeaponReloadStart(UAnimMontage* AnimMontage)
{
	WeaponReloadStart_BP(AnimMontage);
}

void ATDSCharacter::WeaponReloadEnd(bool bIsSuccess)
{
	WeaponReloadEnd_BP(bIsSuccess);
}

void ATDSCharacter::WeaponFire(UAnimMontage* AnimMontage)
{
	WeaponFire_BP(AnimMontage);
}

void ATDSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* AnimMontage)
{
	//in BP
}

void ATDSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//in BP
}

void ATDSCharacter::WeaponFire_BP_Implementation(UAnimMontage* AnimMontage)
{
	//in BP
}






// in one func
void ATDSCharacter::TrySwitchWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		//we have more than one weapon to switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->GetReloadState())
			{
				CurrentWeapon->CancelReload();
			}
		}

		if (InventoryComponent)
		{
			//need tomer to switch with anim
			//now we don't have success switch if we have one weapon
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo))
			{

			}
		}
	}
}

// in one func
void ATDSCharacter::TrySwitchPreviousWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		//we have more than one weapon to switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AdditionalWeaponInfo;
			if (CurrentWeapon->GetReloadState())
			{
				CurrentWeapon->CancelReload();
			}
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo))
			{

			}
		}
	}
}