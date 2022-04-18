// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

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

public:	
	//// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class UCameraComponent* Camera;

	void MoveForward(float Val);
	void MoveRight(float Val);


protected:
	//Weapon spawned by default
	UPROPERTY(EditDefaultsOnly, Category="Configurations")
	TArray<TSubclassOf<class AWeapon>> defaultWeapons;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category="State")
	TArray <class AWeapon*> weapons;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,ReplicatedUsing = OnRep_CurrentWeapon, Category= "State")
	class AWeapon* currentWeapon;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
	int32 currentIndex =0;

protected:
	UFUNCTION()
	virtual void OnRep_CurrentWeapon(const class AWeapon* oldWeapon);
};
