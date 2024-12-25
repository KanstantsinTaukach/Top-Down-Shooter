// Top Down Shooter. All Rights Reserved

#include "TDS_WeaponDefault.h"
#include "TDS_ProjectileDefault.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Particles/ParticleSystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(TDSWeaponDefaultLog, All, All);

ATDS_WeaponDefault::ATDS_WeaponDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName("NoCollision");
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName("NoCollision");
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>("ShootLocation");
	ShootLocation->SetupAttachment(GetRootComponent());
}

void ATDS_WeaponDefault::BeginPlay()
{
	Super::BeginPlay();

	check(SceneComponent);
	check(SkeletalMeshWeapon);
	check(StaticMeshWeapon);
	check(ShootLocation);

	WeaponInit();
}

void ATDS_WeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	ClipDropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void ATDS_WeaponDefault::FireTick(float DeltaTime)
{
	if (GetWeaponRound() > 0)
	{
		if (WeaponFiring)
		{
			if (FireTimer < 0.0f)
			{
				if (!WeaponReloading)
				{
					Fire();
					UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::FireTick - %d bullets lost"), GetWeaponRound());
				}
			}
			else
			{
				FireTimer -= DeltaTime;
			}
		}
	}
	else
	{
		if (!WeaponReloading)
		{
			UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::FireTick - %d bullets lost, RELOAD WEAPON"), GetWeaponRound());
			InitReload();
		}
	}
}

void ATDS_WeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::InitReload()
{
	if (!WeaponReloading)
	{
		WeaponReloading = true;

		ReloadTimer = WeaponSettings.ReloadTime;

		UAnimMontage* AnimToPlay = nullptr;
		if (WeaponAiming)
		{
			AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharReloadAim;
		}
		else
		{
			AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharReload;
		}

		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSettings.SoundReloadWeapon, ShootLocation->GetComponentLocation());

		OnWeaponReloadStart.Broadcast(AnimToPlay);

		UAnimMontage* AnimWeaponToPlay = nullptr;
		if (WeaponAiming)
		{
			AnimWeaponToPlay = WeaponSettings.AnimWeaponInfo.AnimWeaponReloadAim;
		}
		else
		{
			AnimWeaponToPlay = WeaponSettings.AnimWeaponInfo.AnimWeaponReload;
		}

		if (WeaponSettings.AnimWeaponInfo.AnimWeaponReload && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
		{
			SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponToPlay);
		}

		if (WeaponSettings.ClipDropMesh.DropMesh)
		{
			if (WeaponSettings.ClipDropMesh.DropMesh)
			{
				DropClipFlag = true;
				DropClipTimer = WeaponSettings.ClipDropMesh.DropMeshTime;
			}
		}
	}
}

void ATDS_WeaponDefault::FinishReload()
{
	WeaponReloading = false;
	WeaponInfo.Round = WeaponSettings.MaxRound;

	OnWeaponReloadEnd.Broadcast();
}


void ATDS_WeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
			{
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			}
			else
			{
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;
			}
		}

		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else if (CurrentDispersion > CurrentDispersionMax)
		{
			CurrentDispersion = CurrentDispersionMax;
		}
	}
	if (ShowDebug)
	{
		UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::DispersionTick - Dispersion: MAX = %f, MIN = %f, Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
	}
}

void ATDS_WeaponDefault::ClipDropTick(float DeltaTime)
{
	if (DropClipFlag)
	{
		if (DropClipTimer < 0.0f)
		{
			DropClipFlag = false;
			InitDropMesh(WeaponSettings.ClipDropMesh.DropMesh, //
				WeaponSettings.ClipDropMesh.DropMeshOffset, //
				WeaponSettings.ClipDropMesh.DropMeshImpulseDir, //
				WeaponSettings.ClipDropMesh.DropMeshLifeTime, //
				WeaponSettings.ClipDropMesh.ImpulseRandomDispersion, //
				WeaponSettings.ClipDropMesh.PowerImpulse, //
				WeaponSettings.ClipDropMesh.CustomMass);
		}
		else
		{
			DropClipTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellFlag)
	{
		if (DropShellTimer < 0.0f)
		{
			DropShellFlag = false;
			InitDropMesh(WeaponSettings.ShellBullets.DropMesh, //
				WeaponSettings.ShellBullets.DropMeshOffset, //
				WeaponSettings.ShellBullets.DropMeshImpulseDir, //
				WeaponSettings.ShellBullets.DropMeshLifeTime, //
				WeaponSettings.ShellBullets.ImpulseRandomDispersion, //
				WeaponSettings.ShellBullets.PowerImpulse, //
				WeaponSettings.ShellBullets.CustomMass);
		}
		else
		{
			DropShellTimer -= DeltaTime;
		}
	}
}

void ATDS_WeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent(true);
	}

	UpdateStateWeapon(EMovementState::Run_State);
}

void ATDS_WeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f;
	}
}

bool ATDS_WeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo ATDS_WeaponDefault::GetProjectile()
{
	return WeaponSettings.ProjectileSettings;
}

void ATDS_WeaponDefault::Fire()
{
	if (!GetWorld())
	{
		return;
	}

	UAnimMontage* AnimToPlay = nullptr;
	if (WeaponAiming)
	{
		AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharFireAim;
	}
	else
	{
		AnimToPlay = WeaponSettings.AnimWeaponInfo.AnimCharFire;
	}

	if (WeaponSettings.AnimWeaponInfo.AnimWeaponFire && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSettings.AnimWeaponInfo.AnimWeaponFire);
	}

	if (WeaponSettings.ShellBullets.DropMesh)
	{
		if (WeaponSettings.ShellBullets.DropMeshTime < 0.0f)
		{
			InitDropMesh(WeaponSettings.ShellBullets.DropMesh, //
				WeaponSettings.ShellBullets.DropMeshOffset, //
				WeaponSettings.ShellBullets.DropMeshImpulseDir, //
				WeaponSettings.ShellBullets.DropMeshLifeTime, //
				WeaponSettings.ShellBullets.ImpulseRandomDispersion, //
				WeaponSettings.ShellBullets.PowerImpulse, //
				WeaponSettings.ShellBullets.CustomMass);
		}
		else
		{
			DropShellFlag = true;
			DropShellTimer = WeaponSettings.ShellBullets.DropMeshTime;
		}
	}

	OnWeaponFire.Broadcast(AnimToPlay);

	FireTimer = WeaponSettings.RateOfFire;
	--WeaponInfo.Round;
	ChangeDispersionByShot();

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSettings.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.EffectFireWeapon, ShootLocation->GetComponentTransform());

	if (WeaponSettings.EffectFireWeaponBack)
	{
		FVector ForwardVec = ShootLocation->GetForwardVector();
		FVector ReservedVec = -ForwardVec;
		FRotator ReversedSpawnRotation = ReservedVec.Rotation();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.EffectFireWeaponBack, ShootLocation->GetComponentLocation() - ForwardVec * 120.f, ReversedSpawnRotation);
	}

	int32 NumberProjectile = GetNumberProjectileByShot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();

		FVector EndLocation;

		for (int8 i = 0; i < NumberProjectile; ++i)
		{
			EndLocation = GetFireEndLocation();

			if (ProjectileInfo.Projectile)
			{
				//Projectile Init ballistic fire
				FVector Direction = EndLocation - SpawnLocation;
				Direction.Normalize();

				FMatrix MyMatrix(Direction, FVector(0, 0, 0), FVector(0, 0, 0), FVector::ZeroVector);
				SpawnRotation = MyMatrix.Rotator();

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				auto* MyProjectile = Cast<ATDS_ProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (MyProjectile)
				{
					MyProjectile->InitProjectile(WeaponSettings.ProjectileSettings);
					UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::Fire - Projectile #%d"), i);
					UE_LOG(TDSWeaponDefaultLog, Display, TEXT("ATDS_WeaponDefault::initialSpeed of projectile is: %f"), WeaponSettings.ProjectileSettings.ProjectileInitSpeed);
				}
			}
			else
			{
				FHitResult Hit;
				TArray<AActor*> Actors;

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponSettings.DistanceTrace, ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

				if (ShowDebug)
				{
					DrawDebugLine(GetWorld(), SpawnLocation, EndLocation * WeaponSettings.DistanceTrace, FColor::Yellow, false, 5.0f);
				}

				if (WeaponSettings.ProjectileSettings.ProjectileTrailFX && !GetWorldTimerManager().IsTimerActive(FXTimerHandle))
				{
					UParticleSystemComponent* TrailFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSettings.ProjectileSettings.ProjectileTrailFX, ShootLocation->GetComponentTransform());

					if (TrailFX)
					{
						FVector Start = ShootLocation->GetComponentLocation();
						FVector End = Hit.Location;

						TrailFX->SetWorldLocation(Start);

						FVector Direction = (End - Start).GetSafeNormal();
						float Speed = WeaponSettings.ProjectileSettings.ProjectileInitSpeed * 1.25f;

						GetWorldTimerManager().SetTimer(FXTimerHandle, FTimerDelegate::CreateLambda([this, TrailFX, Direction, End, Speed, Hit]()
							{
								FVector NewLocation = TrailFX->GetComponentLocation() + Direction * Speed * GetWorld()->GetDeltaSeconds();
								TrailFX->SetWorldLocation(NewLocation);

								if ((NewLocation - End).SizeSquared() <= 100.0f)
								{
									TrailFX->Deactivate();
									TrailFX->DestroyComponent();

									GetWorldTimerManager().ClearTimer(FXTimerHandle);

									if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
									{
										EPhysicalSurface MySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

										if (WeaponSettings.ProjectileSettings.HitDecals.Contains(MySurfaceType))
										{
											UMaterialInterface* MyMaterial = WeaponSettings.ProjectileSettings.HitDecals[MySurfaceType];

											if (MyMaterial && Hit.GetComponent())
											{
												UGameplayStatics::SpawnDecalAttached(MyMaterial, FVector(20.0f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepRelativeOffset, 10.0F);
											}
										}

										if (WeaponSettings.ProjectileSettings.HitFXs.Contains(MySurfaceType))
										{
											UParticleSystem* MyParticle = WeaponSettings.ProjectileSettings.HitFXs[MySurfaceType];

											if (MyParticle && Hit.GetComponent())
											{
												UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MyParticle, FTransform(Hit.ImpactPoint));
											}
										}

										if (WeaponSettings.ProjectileSettings.HitSound)
										{
											UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSettings.ProjectileSettings.HitSound, Hit.ImpactPoint);
										}

										UGameplayStatics::ApplyDamage(Hit.GetActor(), WeaponSettings.ProjectileSettings.ProjectileDamage, GetInstigatorController(), this, NULL);
									}
								}
							}), 0.01f, true);
					}
				}
			}
		}
	}
}

void ATDS_WeaponDefault::UpdateStateWeapon(EMovementState InMovementState)
{
	// todo Dispersion
	BlockFire = false;
	switch (InMovementState)
	{
	case EMovementState::Aim_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSettings.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSettings.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSettings.DispersionWeapon.Run_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		BlockFire = true;
		SetWeaponStateFire(false);
		break;
	default:
		break;
	}
}

void ATDS_WeaponDefault::ChangeDispersionByShot()
{
	CurrentDispersion += CurrentDispersionRecoil;
}

int32 ATDS_WeaponDefault::GetWeaponRound()
{
	return WeaponInfo.Round;
}

FVector ATDS_WeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.0f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;

		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSettings.DistanceTrace, GetCurrentDispersion() * PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, 0.1f, (uint8)'\000', 1.0f);
		}
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;

		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSettings.DistanceTrace, GetCurrentDispersion() * PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, 0.1f, (uint8)'\000', 1.0f);
		}
	}

	if (ShowDebug)
	{
		//direction weapon look
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 10.0f, (uint8)'\000', 0.5f);
		//direction projectile must fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 10.0f, (uint8)'\000', 0.5f);
		//direction projectile current fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 10.0f, (uint8)'\000', 0.5f);

		//DrawDebugSphere(GetWorld(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector()*SizeVectorToChangeShootDirectionLogic, 10.0f, 8, FColor::Red, false, 10.0f);
	}

	return EndLocation;
}

FVector ATDS_WeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.0f);
}

float ATDS_WeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

int32 ATDS_WeaponDefault::GetNumberProjectileByShot() const
{
	return WeaponSettings.NumberProjectilesByShot;
}


void ATDS_WeaponDefault::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDir, float LifeTimeMesh, float ImpulseRandDispersion, float PowerImpulse, float CustomMass)
{
	if (!GetWorld() || !DropMesh)
	{
		return;
	}

	FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

	FTransform Transform;
	Transform.SetLocation(GetActorLocation() + LocalDir);
	Transform.SetScale3D(Offset.GetScale3D());
	Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Params.Owner = this;

	AStaticMeshActor* NewActor = nullptr;
	NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Params);

	if (NewActor && NewActor->GetStaticMeshComponent())
	{
		NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		NewActor->SetActorTickEnabled(false);
		NewActor->SetLifeSpan(LifeTimeMesh);

		NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
		NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
		NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);

		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel11, ECollisionResponse::ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel12, ECollisionResponse::ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);

		if (CustomMass > 0.0f)
		{
			NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, CustomMass, true);
		}

		if (!DropImpulseDir.IsNearlyZero())
		{
			LocalDir += DropImpulseDir;

			FVector FinalDir;

			if (!FMath::IsNearlyZero(ImpulseRandDispersion))
			{
				FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandDispersion);
			}

			NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
		}
	}
}