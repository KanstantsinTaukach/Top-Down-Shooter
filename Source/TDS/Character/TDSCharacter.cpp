// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "../Components/TDSStaminaComponent.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Components/TDSHealthComponent_Character.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "../Weapons/TDS_ProjectileDefault.h"
#include "../Game/TDSGameInstance.h"
#include "../Pickups/TDSWeaponPickup.h"
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

	CharacterHealthComponent = CreateDefaultSubobject<UTDSHealthComponent_Character>("HealthComponent");
	if (CharacterHealthComponent)
	{
		CharacterHealthComponent->OnDeath.AddDynamic(this, &ATDSCharacter::OnCharacterDeath);
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
	check(InventoryComponent);
	check(CharacterHealthComponent);
	
	StaminaComponent->OnStaminaEmpty.AddUObject(this, &ATDSCharacter::OnStaminaEmpty);
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

	if (PickupWeaponToDestroy != nullptr)
	{
		IsActorReadyToDropWeapon = PickupWeaponToDestroy->GetIsPlayerInOverlap();
	}
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

	PlayerInputComponent->BindAction(TEXT("SwitchNextWeapon"), IE_Pressed, this, &ATDSCharacter::TrySwitchNextWeapon);
	PlayerInputComponent->BindAction(TEXT("SwitchPreviousWeapon"), IE_Pressed, this, &ATDSCharacter::TrySwitchPreviousWeapon);

	PlayerInputComponent->BindAction(TEXT("DropWeapon"), IE_Pressed, this, &ATDSCharacter::DropWeapon);

	PlayerInputComponent->BindAction(TEXT("ActivateSpecialAbility"), IE_Pressed, this, &ATDSCharacter::ActivateSpecialAbility);
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
	if (IsAlive)
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
						Displacement = FVector(0.0f, 0.0f, 120.0f);
						CurrentWeapon->ShouldReduceDispersion = true;
						break;
					case EMovementState::Walk_State:
						Displacement = FVector(0.0f, 0.0f, 80.0f);
						CurrentWeapon->ShouldReduceDispersion = false;
						break;
					case EMovementState::Run_State:
						Displacement = FVector(0.0f, 0.0f, 80.0f);
						CurrentWeapon->ShouldReduceDispersion = false;
						break;
					case EMovementState::Sprint_State: break;
					default: break;
					}

					CurrentWeapon->SetShootEndLocation(ResultHit.Location + Displacement);
				}
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

void ATDSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
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
			if (!MyWeaponInfo.WeaponClass)
			{
				return;
			}

			FVector SpawnLocation = FVector(0);
			FRotator SpawnRotation = FRotator(0);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			auto* MyWeapon = Cast<ATDS_WeaponDefault>(GetWorld()->SpawnActor(MyWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
			if (!MyWeapon)
			{
				return;
			}

			FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
			CurrentWeapon = MyWeapon;
			CurrentWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));

			MyWeapon->SetWeaponInfo(MyWeaponInfo);
			MyWeapon->UpdateStateWeapon(MovementState);
			MyWeapon->SetAdditionWeaponInfo(WeaponAdditionalInfo);

			CurrentIndexWeapon = NewCurrentIndexWeapon;

			MyWeapon->OnWeaponReloadStart.AddDynamic(this, &ATDSCharacter::WeaponReloadStart);
			MyWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATDSCharacter::WeaponReloadEnd);
			MyWeapon->OnWeaponFire.AddDynamic(this, &ATDSCharacter::WeaponFire);

			if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckWeaponCanReload())
			{
				CurrentWeapon->InitReload();
			}

			const auto MyWeaponAdditionInfo = MyWeapon->GetAdditionWeaponInfo();
			if (InventoryComponent && MyWeaponAdditionInfo.Round > 0)
			{
				InventoryComponent->OnWeaponAmmoAvailable.Broadcast(MyWeaponInfo.WeaponType);
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
	if (CurrentWeapon && CurrentWeapon->GetWeaponRound() < CurrentWeapon->GetWeaponInfo().MaxRound && CurrentWeapon->CheckWeaponCanReload())
	{
		CurrentWeapon->InitReload();
	}
}

void ATDSCharacter::WeaponReloadStart(UAnimMontage* AnimMontage)
{
	WeaponReloadStart_BP(AnimMontage);
}

void ATDSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* AnimMontage)
{
	//in BP
}

void ATDSCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoToSubtract)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->GetWeaponInfo().WeaponType, AmmoToSubtract);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->GetAdditionWeaponInfo());
	}

	WeaponReloadEnd_BP(bIsSuccess);
}

void ATDSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//in BP
}

void ATDSCharacter::WeaponFire(UAnimMontage* AnimMontage)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->GetAdditionWeaponInfo());
	}

	WeaponFire_BP(AnimMontage);
}

void ATDSCharacter::WeaponFire_BP_Implementation(UAnimMontage* AnimMontage)
{
	//in BP
}

void ATDSCharacter::StartSwitchWeapon(FWeaponSlot PickupWeaponSlot, ATDSWeaponPickup* PickupActor)
{
	WeaponSlotToSwitch = PickupWeaponSlot;
	PickupWeaponToDestroy = PickupActor;

	if (!InventoryComponent->IsWeaponExistsInInventory())
	{
		InventoryComponent->SetIsNewPickupWeaponAllowed(true);
	}
}

void ATDSCharacter::EndSwitchWeapon()
{
	if (!InventoryComponent)
	{
		UE_LOG(TDSCharacterLog, Warning, TEXT("ATDSCharacter::EndSwitchWeapon: InventoryComponent is NULL"));

		return;
	}

	InventoryComponent->SetIsWeaponExistsInInventory(false);
	InventoryComponent->SetIsNewPickupWeaponAllowed(false);
}

void ATDSCharacter::DropWeapon()
{
	if (!GetWorld() || !CurrentWeapon || !IsActorReadyToDropWeapon || !InventoryComponent)
	{
		return;
	}

	if (CurrentWeapon->GetReloadState())
	{
		CurrentWeapon->CancelReload();
	}

	const auto Slots = InventoryComponent->GetWeaponSlots();
	const auto MyWeaponInfo = CurrentWeapon->GetWeaponInfo();
	FName WeaponName;
	for (auto Slot : Slots)
	{
		if (MyWeaponInfo.WeaponType == Slot.WeaponType)
		{
			WeaponName = Slot.NameItem;
			break;
		}
	}
		
	const int32 Index = InventoryComponent->GetWeaponIndexSlotByName(WeaponName);
	FDropItem MyDropItem;
	if (InventoryComponent->SwitchWeaponToInventory(WeaponSlotToSwitch, Index, MyDropItem))
	{
		if (PickupWeaponToDestroy)
		{
			PickupWeaponToDestroy->PickupSuccess();
			PickupWeaponToDestroy = nullptr;
		}

		FTransform Transform;
		Transform.SetLocation(GetActorLocation() + FVector(0.0f, 100.0f, -45.0f));
		Transform.SetRotation(FQuat::Identity);
		Transform.SetScale3D(FVector(1.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		SpawnParams.Owner = nullptr;

		ATDSWeaponPickup* DropWeapon = GetWorld()->SpawnActor<ATDSWeaponPickup>(ATDSWeaponPickup::StaticClass(), Transform, SpawnParams);

		if (DropWeapon)
		{
			DropWeapon->InitPickup(MyDropItem);
		}

		if (WeaponSlotToSwitch.AdditionalInfo.Round > 0)
		{
			InventoryComponent->OnWeaponAmmoAvailable.Broadcast(WeaponSlotToSwitch.WeaponType);
		}
	}
}


// in one func
void ATDSCharacter::TrySwitchNextWeapon()
{
	if (InventoryComponent->GetWeaponSlots().Num() > 1)
	{
		//we have more than one weapon to switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->GetAdditionWeaponInfo();
			if (CurrentWeapon->GetReloadState())
			{
				CurrentWeapon->CancelReload();
			}
		}

		if (InventoryComponent)
		{
			//need timer to switch with anim
			//now we don't have success switch if we have one weapon
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true))
			{

			}
		}
	}
}

// in one func
void ATDSCharacter::TrySwitchPreviousWeapon()
{
	if (InventoryComponent->GetWeaponSlots().Num() > 1)
	{
		//we have more than one weapon to switch
		int8 OldIndex = CurrentIndexWeapon;
		FAdditionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->GetAdditionWeaponInfo();
			if (CurrentWeapon->GetReloadState())
			{
				CurrentWeapon->CancelReload();
			}
		}

		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false))
			{

			}
		}
	}
}

float ATDSCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CharacterHealthComponent || !IsAlive || Damage <= 0.0) return 0.0f;

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		ATDS_ProjectileDefault* TargetProjectile = Cast<ATDS_ProjectileDefault>(DamageCauser);
		if (TargetProjectile)
		{
			FHitResult Hit;
			Hit.Actor = this;
			Hit.ImpactPoint = this->GetActorLocation();

			UTypes::AddEffectBySurfaceType(TargetProjectile->GetProjectileSettings().Effect, GetSurfaceType(), Hit);
		}
	}

	CharacterHealthComponent->RemoveFromCurrentHealth(Damage);

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ATDSCharacter::OnCharacterDeath()
{
	if (!GetWorld() || !IsAlive || !GetMesh() || !GetMesh()->GetAnimInstance()) return;

	float AnimationTime = 0.0f;
	if (DeathAnimations.Num() > 0)
	{
		int32 RandomNumber = FMath::RandRange(0, DeathAnimations.Num() - 1);
		if (UAnimMontage* DeathMontage = DeathAnimations[RandomNumber] )
		{
			AnimationTime = DeathMontage->GetPlayLength();
			GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
		}
	}

	IsAlive = false;

	AttackCharEvent(false);

	GetCursorToWorld()->SetVisibility(false);

	if (StaminaComponent)
	{
		StaminaComponent->StopStaminaRegeneration();
	}
	
	GetWorld()->GetTimerManager().SetTimer(DeathDelayTimerHandle, this, &ATDSCharacter::HandleDelayedDeathEvents, 3.0f, false);
}

void ATDSCharacter::OnCharacterDeath_BP_Implementation()
{
	//in BP
}

void ATDSCharacter::HandleDelayedDeathEvents()
{
	if (DeathDelayTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DeathDelayTimerHandle);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->UnPossess();
		PC->SetShowMouseCursor(false);
	}

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	OnCharacterDeath_BP();
}

void ATDSCharacter::EnableRagdoll()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
	}
}

EPhysicalSurface ATDSCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = ITDSInterfaceGameActor::GetSurfaceType();

	if (CharacterHealthComponent)
	{
		if (CharacterHealthComponent->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* MyMaterial = GetMesh()->GetMaterial(0);
				if (MyMaterial)
				{
					Result = MyMaterial->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
	}
	
	return Result;
}

TArray<UTDSStateEffect*> ATDSCharacter::GetAllCurrentEffects()
{
	return StateEffects;
}

void ATDSCharacter::RemoveEffect(UTDSStateEffect* EffectToRemove)
{
	StateEffects.Remove(EffectToRemove);
}

void ATDSCharacter::AddEffect(UTDSStateEffect* EffectToAdd)
{
	StateEffects.Add(EffectToAdd);
}

void ATDSCharacter::ActivateSpecialAbility()
{
	if (AbilityEffect && IsAlive)
	{
		UTDSStateEffect* NewEffect = NewObject<UTDSStateEffect>(this, AbilityEffect);
		if (NewEffect)
		{
			FHitResult Hit;
			Hit.Actor = this;
			Hit.ImpactPoint = this->GetActorLocation();
			Hit.ImpactNormal = FVector::UpVector;

			NewEffect->InitObject(this, Hit);
		}
	}
}
