// Top Down Shooter. All Rights Reserved

#include "TDSBasePickup.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

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

void ATDSBasePickup::BeginPlay()
{
	Super::BeginPlay();

	check(CollisionComponent);
	check(PickupStaticMesh);
	check(PickupSkeletalMesh);

	InitSelf();

	GenerateRotationYaw();
}

void ATDSBasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.0f, RotationYaw * DeltaTime, 0.0f));

	for (const auto OverlapPawn : OverlappingPawns)
	{
		if (GivePickupTo(OverlapPawn))
		{
			PickupSuccess();
			break;
		}
	}
}

void ATDSBasePickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	if (GivePickupTo(Pawn))
	{
		PickupSuccess();
	}
	else if (Pawn)
	{
		OverlappingPawns.Add(Pawn);
	}
}

void ATDSBasePickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	const auto Pawn = Cast<APawn>(OtherActor);
	OverlappingPawns.Remove(Pawn);
}

bool ATDSBasePickup::GivePickupTo(APawn* PlayerPawn)
{
	return false;
}

void ATDSBasePickup::PickupSuccess()
{
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
	Destroy();

	if (NiagaraComponent)
	{
		NiagaraComponent->DestroyComponent();
	}
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

	NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupVFX, GetActorLocation());
}

void ATDSBasePickup::GenerateRotationYaw()
{
	const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(25.0f, 50.0f) * Direction;
}
