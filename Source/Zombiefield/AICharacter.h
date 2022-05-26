// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Zombiefield/ZombiefieldProjectile.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "AICharacter.generated.h"

UENUM(BlueprintType)
enum class EZombieMovement : uint8
{
	EMWalking UMETA(DisplayName = "Walking"),
	EMRunning UMETA(DisplayName = "Running"),
	EMAttacking UMETA(DisplayName = "Attacking"),

	EMIdle UMETA(DisplayName = "Idle")
};
UCLASS()
class ZOMBIEFIELD_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		EZombieMovement EZombieMovementEnums;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
	class AWaypoint* NextWaypoint;

	UPROPERTY(EditAnywhere, BlueprintreadWrite)
		int Health;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* DeathParticleEffect;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* AttckFireParticleEffect;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* AttackSparkParticleEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UCapsuleComponent* CollisionCompCap;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UCapsuleComponent* GetCollisionComp() const { return CollisionCompCap; }
};
