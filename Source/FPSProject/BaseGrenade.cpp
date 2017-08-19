// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGrenade.h"
#include "Net/UnrealNetwork.h"




ABaseGrenade::ABaseGrenade()
{
	bReplicates = true;
	Mass = 1.0f;
	bInstantExplode = false;
	ExplosionDelay = 3.0f;
	BlastRadius = 200.0f;
	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetSimulatePhysics(false);
	SphereCollider->SetupAttachment(RootComponent);
	GetStaticMeshComponent()->SetSimulatePhysics(false);
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;
	GetStaticMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABaseGrenade::OnOverlapBegin);



}

void ABaseGrenade::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}
void ABaseGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseGrenade, Mass);
	DOREPLIFETIME(ABaseGrenade, ExplosionDelay);
	DOREPLIFETIME(ABaseGrenade, BlastRadius);
	DOREPLIFETIME(ABaseGrenade, bInstantExplode);
	DOREPLIFETIME(ABaseGrenade, MaxDamage);

}

bool ABaseGrenade::SetExplosionTimer_Validate()
{
	return true;
}


void ABaseGrenade::SetExplosionTimer_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("EXPLODE"));
}
void ABaseGrenade::Explode_Implementation()
{

}