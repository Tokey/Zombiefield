// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletPowerUp.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MainCharacter.h"

// Sets default values
ABulletPowerUp::ABulletPowerUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionCompSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionCompSphere->InitSphereRadius(5.0f);

	CollisionCompSphere->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionCompSphere->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionCompSphere;
}

// Called when the game starts or when spawned
void ABulletPowerUp::BeginPlay()
{
	Super::BeginPlay();
	CollisionCompSphere->OnComponentHit.AddDynamic(this, &ABulletPowerUp::OnHit);
}

// Called every frame
void ABulletPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletPowerUp::Collect()
{
	AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->IsSuperBulletEnabled = true;
	Destroy();
}

void ABulletPowerUp::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("HITTINNN!")));
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	// Only add impulse and destroy projectile if we hit a physics
	if (Player != nullptr)
	{
		Player->IsSuperBulletEnabled = true;
		
	}
}

