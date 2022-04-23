// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombiefieldAnimInstance.h"
#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"


UZombiefieldAnimInstance::UZombiefieldAnimInstance()
{
}

void UZombiefieldAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	/*Character = Cast<AMainCharacter>(TryGetPawnOwner());
	if (Character)
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

	//RHandToSightTransform = 
}

void UZombiefieldAnimInstance::CalculateWeaponSway(const float DeltaTime)
{
	
}

void UZombiefieldAnimInstance::SetIKTransforms()
{
	RHandToSightTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("Weapon_R")));
}
