// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Weapon.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh(), FName("Head"));
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		for (const TSubclassOf<AWeapon>& WeaponClass : defaultWeapons)
		{
			if (!WeaponClass) continue;
			FActorSpawnParameters params;
			params.Owner = this;

			AWeapon* spawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, params);
			const int32 index = weapons.Add(spawnedWeapon);

			if (index == currentIndex)
			{
				currentWeapon = spawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, weapons, COND_None);
	DOREPLIFETIME_CONDITION(AMainCharacter, currentWeapon, COND_None);
}



// Called every frame
//void AMainCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AMainCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMainCharacter::OnRep_CurrentWeapon(const AWeapon* oldWeapon)
{
	if (currentWeapon) {
		if (!currentWeapon->currentOwner)
		{
			const FTransform placementTransform = currentWeapon->placementTransform * GetMesh()->GetSocketTransform(FName("Weapon_R"));
			currentWeapon->SetActorTransform(placementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			currentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("Weapon_R"));

			currentWeapon->mesh->SetVisibility(true);
			currentWeapon->currentOwner = this;
		}
	}
	if (oldWeapon)
	{
		oldWeapon->mesh->SetVisibility(false);
	}
}

