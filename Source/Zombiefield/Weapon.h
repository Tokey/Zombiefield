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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		class USceneComponent* root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		class USkeletalMeshComponent* mesh;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
		class AMainCharacter* currentOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FIKProperties iKProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
		FTransform placementTransform;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="IK")
	FTransform GetSightsWorldTransform() const;
	virtual FTransform GetSightsWorldTransform_Implementation() const {return mesh->GetSocketTransform(FName("Sights"));}
};
