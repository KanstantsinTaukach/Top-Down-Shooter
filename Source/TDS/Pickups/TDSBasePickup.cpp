// Top Down Shooter. All Rights Reserved

#include "TDSBasePickup.h"
#include "Components/SphereComponent.h"

ATDSBasePickup::ATDSBasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetRootComponent(CollisionComponent);

	PickupStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("AmmoStaticMesh");
	PickupStaticMesh->SetGenerateOverlapEvents(false);
	PickupStaticMesh->SetCollisionProfileName("NoCollision");
	PickupStaticMesh->SetupAttachment(RootComponent);

	PickupSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	PickupSkeletalMesh->SetGenerateOverlapEvents(false);
	PickupSkeletalMesh->SetCollisionProfileName("NoCollision");
	PickupSkeletalMesh->SetupAttachment(RootComponent);
}

void ATDSBasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.0f, RotationYaw * DeltaTime, 0.0f));
}

void ATDSBasePickup::BeginPlay()
{
	Super::BeginPlay();

	check(CollisionComponent);
	check(PickupStaticMesh);
	check(PickupSkeletalMesh);

	InitSelf();

	GenerateRotationYaw();
}

void ATDSBasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	if (GivePickupTo(Pawn))
	{
		PickupSuccess();
	}
}

bool ATDSBasePickup::GivePickupTo(APawn* PlayerPawn)
{
	return false;
}

void ATDSBasePickup::PickupSuccess()
{
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
	Destroy();
}

void ATDSBasePickup::InitSelf()
{
	if (PickupSkeletalMesh && !PickupSkeletalMesh->SkeletalMesh)
	{
		PickupSkeletalMesh->DestroyComponent(true);
	}

	if (PickupStaticMesh && !PickupStaticMesh->GetStaticMesh())
	{
		PickupStaticMesh->DestroyComponent(true);
	}
}

void ATDSBasePickup::GenerateRotationYaw()
{
	const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(25.0f, 50.0f) * Direction;
}
