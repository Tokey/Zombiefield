// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombiefieldAnimInstance.h"
#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"


UZombiefieldAnimInstance::UZombiefieldAnimInstance()
{
}

void UZombiefieldAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	Character = Cast<AMainCharacter>(TryGetPawnOwner());
	/*if (Character)
	{
		Mesh = Character->GetMesh();
		Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UZombiefieldAnimInstance::CurrentWeaponChanged);
		CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
	}*/
}

void UZombiefieldAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if(!Character)
	{
		Character = Cast<AMainCharacter>(TryGetPawnOwner());
		if(Character)
		{

			Mesh = Character->GetMesh();
			Character->CurrentWeaponChangedDelegate.AddDynamic(this, &UZombiefieldAnimInstance::CurrentWeaponChanged);
			CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
		}
		else return;
	}

	SetVariables(DeltaTime);
	CalculateWeaponSway(DeltaTime);

	LastRotation = CameraTransform.Rotator();
}

void UZombiefieldAnimInstance::CurrentWeaponChanged(AWeapon* NewWeapon, const AWeapon* OldWeapon)
{
	 CurrentWeapon = NewWeapon;
	if(CurrentWeapon)
	{
		IKProperties = CurrentWeapon->iKProperties;
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UZombiefieldAnimInstance::SetIKTransforms);
	}
	
}

void UZombiefieldAnimInstance::SetVariables(const float DeltaTime)
{
	CameraTransform = FTransform(Character->GetBaseAimRotation(), Character->Camera->GetComponentLocation());

	const FTransform& RootOffset = Mesh->GetSocketTransform(FName("root"),RTS_Component).Inverse() * Mesh->GetSocketTransform(FName("ik_hand_root"));
	RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);

	ADSWeight = Character->ADSWeight;
	
	//Offsets

	//Accumulative Rotation
	constexpr float AngleClamp = 6.f;
	const FRotator& AddRotation = CameraTransform.Rotator() -LastRotation;
	FRotator AddRotationClamped = FRotator(FMath::ClampAngle(AddRotation.Pitch, -AngleClamp, AngleClamp)*1.5,
		FMath::ClampAngle(AddRotation.Yaw, -AngleClamp, AngleClamp), 0);
	AddRotationClamped.Roll = AddRotationClamped.Yaw * .7f;

	AccumulativeRotation += AddRotationClamped;
	AccumulativeRotation = UKismetMathLibrary::RInterpTo(AccumulativeRotation, FRotator::ZeroRotator, DeltaTime, 30.f);
	AccumulativeRotationInterp = UKismetMathLibrary::RInterpTo(AccumulativeRotationInterp, AccumulativeRotation, DeltaTime, 5.);
	
}

void UZombiefieldAnimInstance::CalculateWeaponSway(const float DeltaTime)
{
	FVector LocationOffset = FVector::ZeroVector;
	FRotator RotationOffset = FRotator::ZeroRotator;

	const FRotator& AccumulativeRotationInterpInverse = AccumulativeRotationInterp.GetInverse();
	RotationOffset += AccumulativeRotationInterpInverse;

	
	LocationOffset += FVector(0.f, AccumulativeRotationInterpInverse.Yaw, AccumulativeRotationInterpInverse.Pitch)/6.f;

	LocationOffset *= IKProperties.WeightScale;

	RotationOffset.Pitch *= IKProperties.WeightScale;
	RotationOffset.Yaw *= IKProperties.WeightScale;
	RotationOffset.Roll *= IKProperties.WeightScale;

	OffsetTransform = FTransform(RotationOffset, LocationOffset);
}

void UZombiefieldAnimInstance::SetIKTransforms()
{
	RHandToSightTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r")));
}
