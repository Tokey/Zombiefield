// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZombiefieldProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MainCharacter.h"
#include "NiagaraComponent.h"
#include "AICharacter.h"
#include "BulletPowerUp.h"
#include "NiagaraFunctionLibrary.h"


AZombiefieldProjectile::AZombiefieldProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AZombiefieldProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->bRotationFollowsVelocity = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 2.0f;

	this->SetActorScale3D(FVector(.1, .1, .1));
}



void AZombiefieldProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AAICharacter* Enemy = Cast<AAICharacter>(OtherActor);
	ABulletPowerUp* power = Cast<ABulletPowerUp>(OtherActor);;
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 3.0f, GetActorLocation());

		Destroy();
	}
	else if (Enemy != nullptr)
	{
		Enemy->Health = Enemy->Health - ProjectileDamage;
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 2.0f, GetActorLocation());
	}
	else if (power != nullptr)
	{
		power->Collect();
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), spawnEffect, this->GetActorLocation(), Hit.ImpactNormal.Rotation());
	Destroy();
}