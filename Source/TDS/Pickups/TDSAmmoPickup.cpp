// Top Down Shooter. All Rights Reserved


#include "TDSAmmoPickup.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Weapons/TDS_WeaponDefault.h"
#include "../Character/TDSCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(ATDSAmmoPickupLog, All, All);

ATDSAmmoPickup::ATDSAmmoPickup()
{
	PickupStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("AmmoStaticMesh");
	PickupStaticMesh->SetGenerateOverlapEvents(false);
	PickupStaticMesh->SetCollisionProfileName("NoCollision");
	PickupStaticMesh->SetupAttachment(RootComponent);
}

void ATDSAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (const auto OverlapPawn : OverlappingPawns)
	{
		if (GivePickupTo(OverlapPawn))
		{
			ATDSBasePickup::PickupSuccess();
			break;
		}
	}
}

void ATDSAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	check(PickupStaticMesh);
}

void ATDSAmmoPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);

	if (Pawn)
	{
		OverlappingPawns.Add(Pawn);
	}
}

void ATDSAmmoPickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	OverlappingPawns.Remove(Pawn);
}

bool ATDSAmmoPickup::GivePickupTo(APawn* PlayerPawn)
{	
	if (!PlayerPawn)
	{
		return false;
	}

	const auto InventoryComponent = Cast<UTDSInventoryComponent>(PlayerPawn->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
	if (!InventoryComponent)
	{
		return false;
	}

	bool Result = InventoryComponent->CheckCanTakeAmmo(WeaponType);
	if (Result)
	{
		InventoryComponent->AmmoSlotChangeValue(WeaponType, BulletsAmount);

		int32 BulletsStoredInWeapon;
		const auto WeaponSlots = InventoryComponent->GetWeaponSlots();
		for (auto Slot : WeaponSlots)
		{
			if (WeaponType == Slot.WeaponType)
			{
				BulletsStoredInWeapon = Slot.AdditionalInfo.Round;
				break;
			}
		}

		const auto Player = Cast<ATDSCharacter>(InventoryComponent->GetOwner());
		const auto PlayerCurrentWeapon = Player->GetCurrentWeapon();
		if (Player && PlayerCurrentWeapon)
		{
			const auto WeaponInfo = PlayerCurrentWeapon->GetWeaponInfo();
			if (WeaponInfo.WeaponType == WeaponType && BulletsStoredInWeapon == 0)
			{
				PlayerCurrentWeapon->InitReload();
				InventoryComponent->OnWeaponAmmoAvailable.Broadcast(WeaponType);
			}
		}
		
	}

	return Result;
}
