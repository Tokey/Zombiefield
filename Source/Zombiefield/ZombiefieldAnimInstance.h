// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapon.h"
#include "ZombiefieldAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EMovement : uint8
{
	EMForward UMETA(DisplayName = "Forward"),
	EMBackward UMETA(DisplayName = "Backward"),
	EMLeft UMETA(DisplayName = "Left"),
	EMRight UMETA(DisplayName = "Right"),

	EMForwardSprint UMETA(DisplayName = "ForwardSprint"),
	EMBackwardSprint UMETA(DisplayName = "BackwardSprint"),
	EMLeftSprint UMETA(DisplayName = "LeftSprint"),
	EMRightSprint UMETA(DisplayName = "RightSprint"),
	
	EMIdle UMETA (DisplayName = "Idle")
};

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


	//State
	UPROPERTY(BlueprintReadOnly, Category="Anim")
	FRotator LastRotation;
	
	UPROPERTY(BlueprintReadOnly, Category="Anim")
	FVector LastLocation;
	
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

	//Accumulative Offsets
	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FRotator AccumulativeRotation;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FRotator AccumulativeRotationInterp;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FVector AccumulativeLocation;

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	FVector AccumulativeLocationInterp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool IsFiring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	EMovement EMovementEnums;
};
