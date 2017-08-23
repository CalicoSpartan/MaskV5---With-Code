// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "BaseGrenade.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API ABaseGrenade : public AStaticMeshActor
{
	GENERATED_BODY()

	
public:
	ABaseGrenade();

	UPROPERTY(Replicated,EditAnywhere)
		class USphereComponent* SphereCollider;

	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		bool bInstantExplode;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		bool bPendingExplode;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		FTimerHandle ExplosionTimer;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		float BlastRadius;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		float MaxDamage;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		float Mass;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		float ExplosionForce = 500000.0f;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		float ExplosionDelay;
	UPROPERTY(Replicated, EditAnywhere, Category = "Grenade")
		APawn* GrenadeThrower;


	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void EnablePhysics();

	UFUNCTION(Server, Reliable, WithValidation)
		void Thrown(FVector Force,APawn* Thrower);

	UFUNCTION(Server, Reliable, WithValidation)
		void SetExplosionTimer();
	UFUNCTION(Server, Reliable,WithValidation)
		void Explode();
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
