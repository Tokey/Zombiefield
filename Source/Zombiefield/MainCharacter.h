// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ZombiefieldAnimInstance.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCurrentWeaponChangedDelegate, class AWeapon*, CurrentWeapon, const class AWeapon*, OldWeapon);

UCLASS()
class ZOMBIEFIELD_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

public:	
	//// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class UCameraComponent* Camera;

	void MoveForward(float Val);
	void MoveRight(float Val);

	virtual void NextWeapon();
	virtual void PreviousWeapon();
	void OnFire(float FirePressed);
	void StartSprint();
	void StopSprint();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Components")
	class USkeletalMeshComponent* ClientMesh;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	bool IsFiring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	bool IsWalkingForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Mesh)
	bool IsWalkingBackward;

protected:
	//Weapon spawned by default
	UPROPERTY(EditDefaultsOnly, Category="Configurations")
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category="State")
	TArray <class AWeapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,ReplicatedUsing = OnRep_CurrentWeapon, Category= "State")
	class AWeapon* CurrentWeapon;

	//Called after changing weapons
	UPROPERTY(BlueprintAssignable, Category="Delegates")
	FCurrentWeaponChangedDelegate CurrentWeaponChangedDelegate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
	int32 CurrentIndex =0;

	UFUNCTION(BlueprintCallable, Category="Character")
	virtual void EquipWeapon(const int32 Index);

public:
	UFUNCTION(BlueprintCallable, Category="Anim")
	virtual  void StartAiming();

	UFUNCTION(BlueprintCallable, Category="Anim")
	virtual void ReverseAiming();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Anim")
	float ADSWeight = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	EMovement EMovementEnumsMain;
	
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configurations|Anim")
	class UCurveFloat* AimingCurve;
	

	FTimeline AimingTimeline;
	
	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bForward = true);
	virtual FORCEINLINE void Server_Aim_Implementation(const bool bForward)
	{
		Multi_Aim(bForward);
		Multi_Aim_Implementation(bForward);
	}

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Aim(const bool bForward);
	virtual void Multi_Aim_Implementation(const bool bForward);

	UFUNCTION()
	virtual void TimeLineProgress(const float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;
	

protected:
	UFUNCTION()
	virtual void OnRep_CurrentWeapon(const class AWeapon* oldWeapon);

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(class AWeapon* Weapon);
	virtual void Server_SetCurrentWeapon_Implementation(class AWeapon* Weapon);

	class UCharacterMovementComponent* playerController;
	
};
