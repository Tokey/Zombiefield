// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "MainCharacter.h"
#include "BulletPowerUp.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AAICharacter::AAICharacter():
EZombieMovementEnums(EZombieMovement::EMIdle)
{
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CollisionCompCap = GetCapsuleComponent();
	CollisionCompCap->SetCapsuleRadius(50.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	Health = 3;
}

// Called when the game starts or when spawned
void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	CollisionCompCap->OnComponentHit.AddDynamic(this, &AAICharacter::OnHit);
	GetCharacterMovement()->MaxWalkSpeed = FMath::RandRange(200, 550);
}

// Called every frame
void AAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Health <= 0)
	{
		AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		Player->Score++;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathParticleEffect, GetTransform());
		int RageModifier = FMath::RandRange(0.f, 10.f);
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("Rage! %d"), RageModifier));
		if (RageModifier == 4)
			Player->IsSuperBulletEnabled = true;
		Destroy();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("VELOCITY: %d"), this->GetVelocity().Length()));
	if (this->GetVelocity().Length() > 3 && this->GetVelocity().Length() < 401)
		EZombieMovementEnums = EZombieMovement::EMWalking;
	else if(this->GetVelocity().Length() >= 401)
		EZombieMovementEnums = EZombieMovement::EMRunning;
	if (this->GetVelocity().Length() <= 3)
		EZombieMovementEnums = EZombieMovement::EMIdle;

}

// Called to bind functionality to input
void AAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAICharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	AMainCharacter* Player = Cast<AMainCharacter>(OtherActor);
	// Only add impulse and destroy projectile if we hit a physics
	if (Player != nullptr)
	{
		EZombieMovementEnums = EZombieMovement::EMAttacking;
		Player->Health--;

		if (Player->Health <= 0)
			UGameplayStatics::OpenLevel(GetWorld(), FName("FirstPersonExampleMap"));
	}
}

