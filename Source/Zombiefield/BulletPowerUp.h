// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletPowerUp.generated.h"

UCLASS()
class ZOMBIEFIELD_API ABulletPowerUp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABulletPowerUp();



	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Collect();

	UPROPERTY(EditAnywhere, Category = Projectile)
		class USphereComponent* CollisionCompSphere;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	USphereComponent* GetCollisionComp() const { return CollisionCompSphere; }
};
