// Top Down Shooter. All Rights Reserved


#include "TDSWeaponPickup.h"
#include "../Components/TDSInventoryComponent.h"
#include "../Character/TDSCharacter.h"
#include "../Game/TDSGameInstance.h"

DEFINE_LOG_CATEGORY_STATIC(TDSWeaponPickupLog, All, All);

ATDSWeaponPickup::ATDSWeaponPickup()
{
	PickupSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	PickupSkeletalMesh->SetGenerateOverlapEvents(false);
	PickupSkeletalMesh->SetCollisionProfileName("NoCollision");
	PickupSkeletalMesh->SetupAttachment(RootComponent);
}

void ATDSWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	check(PickupSkeletalMesh);

	UTDSGameInstance* MyGameInstance = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (MyGameInstance)
	{
		FWeaponInfo WeaponInfo;
		MyGameInstance->GetWeaponInfoByName(WeaponSlot.NameItem, WeaponInfo);
		
		WeaponSlot.AdditionalInfo.Round = WeaponInfo.MaxRound;
	}
}

bool ATDSWeaponPickup::GivePickupTo(APawn* PlayerPawn)
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

	if (InventoryComponent->IsWeaponExistsInInventory())
	{
		InventoryComponent->SetIsNewPickupWeaponAllowed(false);
	}
		
	if (InventoryComponent->TryGetWeaponToInventory(WeaponSlot))
	{
		return true;
	}
	else
	{
		const auto Player = Cast<ATDSCharacter>(InventoryComponent->GetOwner());
		Player->StartSwitchWeapon(WeaponSlot, this);
	}

	return false;
}

void ATDSWeaponPickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	const auto Player = Cast<ATDSCharacter>(OtherActor);
	Player->EndSwitchWeapon();
}

void ATDSWeaponPickup::InitPickup(const FDropItem& DropItemInfo)
{
	if (PickupSkeletalMesh && DropItemInfo.WeaponDropSkeletalMesh)
	{
		PickupSkeletalMesh->SetSkeletalMesh(DropItemInfo.WeaponDropSkeletalMesh);
		PickupSkeletalMesh->SetRelativeTransform(DropItemInfo.WeaponDropMeshOffset);
		
		if (PickupSkeletalMesh->SkeletalMesh == nullptr)
		{
			UE_LOG(TDSWeaponPickupLog, Warning, TEXT("ATDSWeaponPickup::InitPickup - Failed to set SkeletalMesh on PickupSkeletalMesh"));
		}
	}
	else
	{
		UE_LOG(TDSWeaponPickupLog, Warning, TEXT("ATDSWeaponPickup::InitPickup - WeaponSkeletalMesh is NULL"));
	}

	if (DropItemInfo.WeaponDropVFX)
	{
		PickupVFX = DropItemInfo.WeaponDropVFX;
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupVFX, GetActorLocation());
	}

	if (DropItemInfo.WeaponDropSound)
	{
		PickupSound = DropItemInfo.WeaponDropSound;
	}

	WeaponSlot = DropItemInfo.WeaponDropSlot;
}