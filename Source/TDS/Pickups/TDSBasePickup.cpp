// Top Down Shooter. All Rights Reserved

#include "TDSBasePickup.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(TDSBasePickupLog, All, All);

ATDSBasePickup::ATDSBasePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName("NoCollision");
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SetRootComponent(CollisionComponent);
}

void ATDSBasePickup::BeginPlay()
{
	Super::BeginPlay();

	check(CollisionComponent);

	SpawnPickupVFX();
	GenerateRotationYaw();
}

void ATDSBasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.0f, RotationYaw * DeltaTime, 0.0f));
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

void ATDSBasePickup::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
}

bool ATDSBasePickup::GivePickupTo(APawn* PlayerPawn)
{
	return false;
}

void ATDSBasePickup::PickupSuccess()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());
	}

	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (NiagaraComponent)
	{
		NiagaraComponent->DestroyComponent();
	}

	Destroy();
	UE_LOG(TDSBasePickupLog, Display, TEXT("ATDSBasePickup::PickupSuccess - Pickup success!"));
}

void ATDSBasePickup::GenerateRotationYaw()
{
	const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(25.0f, 50.0f) * Direction;
}

void ATDSBasePickup::SpawnPickupVFX()
{
	if (PickupVFX)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PickupVFX, GetActorLocation());
	}
}
