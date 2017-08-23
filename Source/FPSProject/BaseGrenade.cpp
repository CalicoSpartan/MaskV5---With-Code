// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseGrenade.h"
#include "FPSCharacter.h"
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
	DOREPLIFETIME(ABaseGrenade, SphereCollider);
	DOREPLIFETIME(ABaseGrenade, ExplosionDelay);
	DOREPLIFETIME(ABaseGrenade, BlastRadius);
	DOREPLIFETIME(ABaseGrenade, bInstantExplode);
	DOREPLIFETIME(ABaseGrenade, MaxDamage);
	DOREPLIFETIME(ABaseGrenade, bPendingExplode);
	DOREPLIFETIME(ABaseGrenade, GrenadeThrower);
	DOREPLIFETIME(ABaseGrenade, ExplosionForce);

}
bool ABaseGrenade::EnablePhysics_Validate()
{
	return true;
}
void ABaseGrenade::EnablePhysics_Implementation()
{
	SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	SphereCollider->SetMobility(EComponentMobility::Movable);
}
bool ABaseGrenade::SetExplosionTimer_Validate()
{
	return true;
}

bool ABaseGrenade::Thrown_Validate(FVector Force, APawn* Thrower)
{
	return true;
}
void ABaseGrenade::Thrown_Implementation(FVector Force, APawn* Thrower)
{
	if (!bInstantExplode)
	{
		SetExplosionTimer();
	}
	GrenadeThrower = Thrower;
	SphereCollider->IgnoreActorWhenMoving(Thrower, true);
	GetStaticMeshComponent()->IgnoreActorWhenMoving(Thrower, true);
	if (AFPSCharacter* Character = Cast<AFPSCharacter>(Thrower))
	{
		SphereCollider->IgnoreComponentWhenMoving(Character->GetMesh(), true);
		SphereCollider->IgnoreComponentWhenMoving(Character->GetCapsuleComponent(), true);
		GetStaticMeshComponent()->IgnoreComponentWhenMoving(Character->GetMesh(), true);
		GetStaticMeshComponent()->IgnoreComponentWhenMoving(Character->GetCapsuleComponent(), true);
	}
	
	GetStaticMeshComponent()->AddForce(Force);
	UE_LOG(LogClass, Log, TEXT("WORKED"));
}


void ABaseGrenade::SetExplosionTimer_Implementation()
{
	bPendingExplode = true;
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimer, this, &ABaseGrenade::Explode, ExplosionDelay, false);
}

bool ABaseGrenade::Explode_Validate()
{
	return true;
}


void ABaseGrenade::Explode_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(ExplosionTimer);
	TArray<class AActor*> PossiblePlayers;
	const TArray<AActor*> IgnoredObjects;
	TArray<TEnumAsByte<EObjectTypeQuery>> PossibleObjects;
	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), BlastRadius, PossibleObjects,NULL,IgnoredObjects,PossiblePlayers))
	{
		for (int32 i = 0; i < PossiblePlayers.Num(); ++i)
		{
			if (AFPSCharacter* const Character = Cast<AFPSCharacter>(PossiblePlayers[i]))
			{
				FVector dirtoplayer = Character->GetActorLocation() - GetActorLocation();
				float disttoplayer = dirtoplayer.Size();
				float oppositedamagepercent = disttoplayer / BlastRadius;
				float damagepercent = 1.0f - oppositedamagepercent;
				if (damagepercent > 1.0f)
				{
					damagepercent = 1.0f;
				}
				if (damagepercent < 0.1f)
				{
					damagepercent = 0.1f;
				}
				Character->Shooter = GrenadeThrower;
				Character->SetHitData(ExplosionForce, NAME_None, dirtoplayer.GetSafeNormal());
				Character->ServerChangeHealthBy(-MaxDamage * damagepercent);
				Character->TriggerUpdateUI();
				UE_LOG(LogClass, Log, TEXT("DistanceToPlayer: %f"), disttoplayer);
				UE_LOG(LogClass, Log, TEXT("Hit Player: %s"),*Character->GetName());
			}
		}
	}
	UE_LOG(LogClass, Log, TEXT("Detonated"));
	Destroy();
}