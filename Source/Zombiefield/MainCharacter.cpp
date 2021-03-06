// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Weapon.h"
#include "AmmoWidget.h"
#include "ZombiefieldAnimInstance.h"
#include "AICharacter.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Public/Net/UnrealNetwork.h"
#include "ZombiefieldProjectile.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


float FireRate;
float Fired;
float ReloadDurationTicker;
bool IsAiming;
bool IsStrafing;
bool IsWalking;
bool IsReloading;
float DefaultWalkSpeed;
float SprintingSpeed;
float SuperBulletCurrentTimer;



bool IsSprinting;
// Sets default values
AMainCharacter::AMainCharacter()
{

	Fired = FireRate;
	Score = 0;
	Health = 100;
	BulletPowerupTimer = 10;
	SuperBulletCurrentTimer = BulletPowerupTimer;
	DefaultWalkSpeed = 600;
	SprintingSpeed = 1000;

	playerController = GetCharacterMovement();
	playerController->MaxWalkSpeed = DefaultWalkSpeed;

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	GetMesh()->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	GetMesh()->bVisibleInReflectionCaptures = true;
	GetMesh()->bCastHiddenShadow = true;

	ClientMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClientMesh"));
	ClientMesh->SetCastShadow(false);
	ClientMesh->SetCastHiddenShadow(false);
	ClientMesh->bVisibleInReflectionCaptures = false;
	ClientMesh->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	ClientMesh->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh(), FName("Head"));

	GunOffset = FVector(50.0f, 0.0f, 0.0f);
	//AnimInstance = Cast<UZombiefieldAnimInstance>(GetClass());
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMainCharacter::AISpawner, 1.0f, true, 5.0f);


	// Setup ADS timeline
	if (AimingCurve)
	{
		FOnTimelineFloat TimelineFloat;
		TimelineFloat.BindDynamic(this, &AMainCharacter::TimeLineProgress);

		AimingTimeline.AddInterpFloat(AimingCurve, TimelineFloat);
	}

	// Client Mesh logic
	if (IsLocallyControlled())
	{
		ClientMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
		GetMesh()->SetVisibility(false);
	}
	else
	{
		ClientMesh->DestroyComponent();
	}

	// Spawning weapon
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
				FireRate = (1 / CurrentWeapon->FireRate) * 60;
			}
		}
	}
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(AMainCharacter, CurrentWeapon, COND_None);
	DOREPLIFETIME_CONDITION(AMainCharacter, ADSWeight, COND_None);
}

void AMainCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	DOREPLIFETIME_ACTIVE_OVERRIDE(AMainCharacter, ADSWeight, ADSWeight >= 1.f || ADSWeight <= 0.f);
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimingTimeline.TickTimeline(DeltaTime);
	InterpFinalRecoil(DeltaTime);
	InterpRecoil(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("HEALTH: %d"), Health));

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AMainCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AMainCharacter::ReverseAiming);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("NextWeapon", EInputEvent::IE_Pressed, this, &AMainCharacter::NextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", EInputEvent::IE_Pressed, this, &AMainCharacter::PreviousWeapon);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainCharacter::StopSprint);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("Fire", this, &AMainCharacter::OnFire);
	PlayerInputComponent->BindAxis("Reload", this, &AMainCharacter::OnReload);

}

void AMainCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		IsWalking = true;
		if (Value == 1.0)
		{
			if (!IsSprinting && !IsReloading)
				EMovementEnumsMain = EMovement::EMForward;
			else if (!IsReloading)
				EMovementEnumsMain = EMovement::EMForwardSprint;
		}
		else if (Value == -1.0)
		{
			if (!IsSprinting && !IsReloading)
				EMovementEnumsMain = EMovement::EMBackward;
			else if (!IsReloading)
				EMovementEnumsMain = EMovement::EMBackwardSprint;

		}
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
	else if (Value == 0.0)
	{
		IsWalking = false;
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		IsStrafing = true;
		if (Value == 1.0)
		{
			if (!IsSprinting && !IsReloading)
				EMovementEnumsMain = EMovement::EMRight;
			else if (!IsReloading)
				EMovementEnumsMain = EMovement::EMRightSprint;
		}
		else if (Value == -1.0)
		{
			if (!IsSprinting && !IsReloading)
				EMovementEnumsMain = EMovement::EMLeft;
			else if (!IsReloading)
				EMovementEnumsMain = EMovement::EMLeftSprint;
		}
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
	else if (Value == 0.0)
	{
		IsStrafing = false;
	}
	if (!IsStrafing && !IsWalking && !IsReloading)
		EMovementEnumsMain = EMovement::EMIdle;
}

void AMainCharacter::StartAiming()
{
	IsAiming = true;
	if (IsLocallyControlled() || HasAuthority()) 
	{
		Multi_Aim(true);
	}
	if (!HasAuthority())
	{
		Server_Aim(true);
	}
}

void AMainCharacter::ReverseAiming()
{
	IsAiming = false;
	if (IsLocallyControlled() || HasAuthority()) {
		Multi_Aim(false);
	}
	if (!HasAuthority())
	{
		Server_Aim(false);
	}
}

void AMainCharacter::Multi_Aim_Implementation(const bool bForward)
{
	if (bForward)
	{
		AimingTimeline.Play();
	}
	else
	{
		AimingTimeline.Reverse();
	}
}


void AMainCharacter::TimeLineProgress(const float Value)
{
	ADSWeight = Value;
}



void AMainCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);
	FireRate = (1 / CurrentWeapon->FireRate) * 60;

}

void AMainCharacter::PreviousWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
	FireRate = (1 / CurrentWeapon->FireRate) * 60;
}

void AMainCharacter::EquipWeapon(const int32 Index)
{
	if (!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index]) return;

	if (IsLocallyControlled() || HasAuthority()) {
		CurrentIndex = Index;

		const AWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		OnRep_CurrentWeapon(OldWeapon);

	}

	if (!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
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
		if (!CurrentWeapon->CurrentOwner)
		{
			const FTransform placementTransform = CurrentWeapon->placementTransform * GetMesh()->GetSocketTransform(FName("Weapon_R"));
			CurrentWeapon->SetActorTransform(placementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("Weapon_R"));


			CurrentWeapon->CurrentOwner = this;
		}
		CurrentWeapon->Mesh->SetVisibility(true);
		CurrentWeapon->SightStaticMesh->SetVisibility(true);
	}
	if (OldWeapon)
	{
		OldWeapon->Mesh->SetVisibility(false);
		OldWeapon->SightStaticMesh->SetVisibility(false);
	}

	CurrentWeaponChangedDelegate.Broadcast(CurrentWeapon, OldWeapon);
}

void AMainCharacter::OnFire(float FirePressed)
{

	if (IsSuperBulletEnabled == true)
	{
		SuperBulletCurrentTimer -= GetWorld()->DeltaTimeSeconds;
		RageModeText = "Rage Mode: " + FString::FromInt((int)SuperBulletCurrentTimer);
	}
	if (SuperBulletCurrentTimer <= 0)
	{
		RageModeText = "";
		IsSuperBulletEnabled = false;
		SuperBulletCurrentTimer = BulletPowerupTimer;
	}
	Fired -= GetWorld()->DeltaTimeSeconds;
	if (FirePressed == 1.0)
	{
		//AnimInstance->IsFiring = true;

		if (Fired <= 0 && CurrentWeapon->WeaponAmmo.CurrentAmmoSize > 0 && !IsReloading)
		{
			CurrentWeapon->WeaponAmmo.CurrentAmmoSize--;

			if (CurrentWeapon->WeaponAmmo.CurrentAmmoSize <= 0)
				OnReload(1.0);
			IsFiring = true;
			Fired = FireRate;
			if (CurrentWeapon->BulletClass != nullptr)
			{
				UWorld* const World = GetWorld();
				if (World != nullptr)
				{
					Shoot();
					FRotator SpawnRotation = CurrentWeapon->Mesh->GetSocketRotation("MuzzleFlash");
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					FVector SpawnLocation;
					if (!IsAiming)
						SpawnLocation = ((CurrentWeapon != nullptr) ? CurrentWeapon->Mesh->GetSocketLocation("MuzzleFlash") : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
					else
					{
						SpawnRotation = CurrentWeapon->Mesh->GetSocketRotation("ADS");
						SpawnLocation = ((CurrentWeapon != nullptr) ? CurrentWeapon->Mesh->GetSocketLocation("ADS") : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
					}

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the projectile at the muzzle
					if (!IsSuperBulletEnabled)
						World->SpawnActor<AZombiefieldProjectile>(CurrentWeapon->BulletClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
					else
						World->SpawnActor<AZombiefieldProjectile>(CurrentWeapon->SuperBulletClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				}
			}
		}
	}
	else if (FirePressed == 0.0)
	{
		IsFiring = false;
		//AnimInstance->IsFiring = false;
		//Fired = 0;
	}
}

void AMainCharacter::OnReload(float RealoadPressed)
{
	if (IsReloading)
		ReloadDurationTicker -= GetWorld()->DeltaTimeSeconds;
	if (ReloadDurationTicker <= 0)
		IsReloading = false;
	if (RealoadPressed == 1.0f)
	{
		if (!IsReloading && CurrentWeapon->WeaponAmmo.CurrentAmmoSize < CurrentWeapon->WeaponAmmo.ClipSize && ReloadDurationTicker == CurrentWeapon->WeaponAmmo.ReloadDuration)
		{
			IsReloading = true;
			EMovementEnumsMain = EMovement::EMReload;
			CurrentWeapon->WeaponAmmo.CurrentAmmoSize = CurrentWeapon->WeaponAmmo.ClipSize;
		}
	}
	else if (!IsReloading)
		ReloadDurationTicker = CurrentWeapon->WeaponAmmo.ReloadDuration;
}

void AMainCharacter::StartSprint()
{
	playerController->MaxWalkSpeed = SprintingSpeed;
	IsSprinting = true;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("SprintSpeed %f"), playerController->MaxWalkSpeed));
}

void AMainCharacter::StopSprint()
{
	playerController->MaxWalkSpeed = DefaultWalkSpeed;
	IsSprinting = false;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("WalkSpeed %f"), playerController->MaxWalkSpeed));
}

void AMainCharacter::InterpFinalRecoil(float DeltaTime)
{
	if (IsAiming)
		FinalRecoilTransform = UKismetMathLibrary::TInterpTo(FinalRecoilTransform, FTransform(), DeltaTime, CurrentWeapon->RecoilCompensationStrength * 2);
	else
		FinalRecoilTransform = UKismetMathLibrary::TInterpTo(FinalRecoilTransform, FTransform(), DeltaTime, CurrentWeapon->RecoilCompensationStrength);
}

void AMainCharacter::InterpRecoil(float DeltaTime)
{
	if (IsAiming)
		RecoilTransform = UKismetMathLibrary::TInterpTo(RecoilTransform, FinalRecoilTransform, DeltaTime, CurrentWeapon->RecoilCompensationStrength * 2);
	else
		RecoilTransform = UKismetMathLibrary::TInterpTo(RecoilTransform, FinalRecoilTransform, DeltaTime, CurrentWeapon->RecoilCompensationStrength);
}

void AMainCharacter::Shoot()
{

	FVector RecoilVector = FinalRecoilTransform.GetLocation();
	RecoilVector += FVector(FMath::RandRange(-CurrentWeapon->RecoilStrength, -CurrentWeapon->RecoilStrength / 7),
		FMath::RandRange(-CurrentWeapon->RecoilStrength / 7, CurrentWeapon->RecoilStrength / 7),
		FMath::RandRange(-CurrentWeapon->RecoilStrength / 21, CurrentWeapon->RecoilStrength / 21));

	FRotator RecoilRot = FinalRecoilTransform.GetRotation().Rotator();
	RecoilRot += FRotator(FMath::RandRange(-CurrentWeapon->RecoilStrength, -CurrentWeapon->RecoilStrength / 7),
		FMath::RandRange(-CurrentWeapon->RecoilStrength / 7, CurrentWeapon->RecoilStrength / 7),
		FMath::RandRange(-CurrentWeapon->RecoilStrength / 1.1, CurrentWeapon->RecoilStrength / 1.1));
	FinalRecoilTransform.SetRotation(RecoilRot.Quaternion());
	FinalRecoilTransform.SetLocation(RecoilVector);
}


void AMainCharacter::AISpawner()
{
	FVector SpawnLocation = FVector(FMath::RandRange(900, 1200), FMath::RandRange(2500, 2900), 270);
	FVector SpawnLocation2 = FVector(FMath::RandRange(-3333, -2999), FMath::RandRange(-4200, -3699), 270);
	FRotator SpawnRotation = FRotator(0, 0, 0);

	if (WeakAItoSpawn != nullptr && StrongAItoSpawn != nullptr)
	{
		if (FMath::RandBool() == true)
		{
			GetWorld()->SpawnActor<AAICharacter>(WeakAItoSpawn, SpawnLocation, SpawnRotation);
			GetWorld()->SpawnActor<AAICharacter>(WeakAItoSpawn, SpawnLocation2, SpawnRotation);
		}
		else {
			GetWorld()->SpawnActor<AAICharacter>(StrongAItoSpawn, SpawnLocation, SpawnRotation);
			GetWorld()->SpawnActor<AAICharacter>(StrongAItoSpawn, SpawnLocation2, SpawnRotation);
		}
	}

	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMainCharacter::AISpawner, 1.0f, true, FMath::RandRange(3, 6));
}





