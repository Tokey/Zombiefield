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
		for (const TSubclassOf<AWeapon>& WeaponClass : DefaultWeapons)
		{
			if (!WeaponClass) continue;
			FActorSpawnParameters params;
			params.Owner = this;

			AWeapon* spawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, params);
			const int32 index = Weapons.Add(spawnedWeapon);

			if (index == CurrentIndex)
			{
				CurrentWeapon = spawnedWeapon;
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(AMainCharacter, CurrentWeapon, COND_None);
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

	PlayerInputComponent->BindAction("NextWeapon", EInputEvent::IE_Pressed, this, &AMainCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", EInputEvent::IE_Pressed, this, &AMainCharacter::PreviousWeapon);

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



void AMainCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);

}

void AMainCharacter::PreviousWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
}

void AMainCharacter::EquipWeapon(const int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index]) return;

	if (IsLocallyControlled()) {
		CurrentIndex = Index;

		const AWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);

	}

	else if (!HasAuthority())
	{
		Server_SetCurrentWeapon_Implementation(Weapons[Index]);
	}
}
void AMainCharacter::Server_SetCurrentWeapon_Implementation(class AWeapon* NewWeapon)
{
	const AWeapon* OldWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}


void AMainCharacter::OnRep_CurrentWeapon(const AWeapon* OldWeapon)
{
	if (CurrentWeapon) {
		if (!CurrentWeapon->currentOwner)
		{
			const FTransform placementTransform = CurrentWeapon->placementTransform * GetMesh()->GetSocketTransform(FName("Weapon_R"));
			CurrentWeapon->SetActorTransform(placementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("Weapon_R"));

			
			CurrentWeapon->currentOwner = this;
		}
		CurrentWeapon->mesh->SetVisibility(true);
	}
	if (OldWeapon)
	{
		OldWeapon->mesh->SetVisibility(false);
	}

	CurrentWeaponChangedDelegate.Broadcast(CurrentWeapon,OldWeapon);
}





