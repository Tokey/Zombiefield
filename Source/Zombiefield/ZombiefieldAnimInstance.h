// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapon.h"
#include "ZombiefieldAnimInstance.generated.h"

UCLASS()
class ZOMBIEFIELD_API UZombiefieldAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UZombiefieldAnimInstance();
	

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	UFUNCTION()
	virtual  void CurrentWeaponChanged(class AWeapon* NewWeapon, const class AWeapon* OldWeapon);
	
	virtual  void SetVariables(const float DeltaTime);
	virtual  void CalculateWeaponSway(const float DeltaTime);

	virtual void SetIKTransforms();
public:

	// References
	UPROPERTY(BlueprintReadWrite, Category = "Anim")
		class AMainCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
		class USkeletalMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, Category = "Anim")
		class AWeapon* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
		FIKProperties IKProperties;

	//IK Variables
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Anim")
	FTransform CameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform RelativeCameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform RHandToSightTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	FTransform OffsetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	float ADSWeight = 0.f;
};
