// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FIKProperties
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAnimSequence* animPose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float aimOffset = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WeightScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FTransform customOffsetTransform;
};

USTRUCT(BlueprintType)
struct FAmmo
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ClipSize = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentAmmoSize = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int NumberOfMags = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ReloadDuration = 2;
};

UCLASS()
class ZOMBIEFIELD_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	//virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AZombiefieldProjectile> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AZombiefieldProjectile> SuperBulletClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
		class AMainCharacter* CurrentOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FIKProperties iKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FAmmo WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform placementTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float RecoilStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float RecoilCompensationStrength;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
		FTransform GetSightsWorldTransform() const;
	virtual FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights")); }
};
