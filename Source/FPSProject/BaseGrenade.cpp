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
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetSimulatePhysics(false);
	
	RootComponent = SphereCollider;

	GetStaticMeshComponent()->SetSimulatePhysics(false);
	GetStaticMeshComponent()->SetupAttachment(RootComponent);
	GetStaticMeshComponent()->SetRelativeLocation(FVector(0, 0, 0));
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
	DOREPLIFETIME(ABaseGrenade, MaxDamageBlastRadius);
	DOREPLIFETIME(ABaseGrenade, ProjectileMovement);

}
bool ABaseGrenade::EnablePhysics_Validate(APawn* Thrower)
{
	return true;
}
void ABaseGrenade::EnablePhysics_Implementation(APawn* Thrower)
{


}
bool ABaseGrenade::SetExplosionTimer_Validate()
{
	return true;
}

bool ABaseGrenade::Thrown_Validate(FVector Direction,float Force, APawn* Thrower)
{
	return true;
}
void ABaseGrenade::Thrown_Implementation(FVector Direction,float Force, APawn* Thrower)
{
	if (!bInstantExplode)
	{
		SetExplosionTimer();
	}
	GrenadeThrower = Thrower;

	//ProjectileMovement->Activate(true);
	//ProjectileMovement->ProjectileGravityScale = 1.0f;
	//GetStaticMeshComponent()->IgnoreActorWhenMoving(Thrower, true);
	
	ProjectileMovement->SetVelocityInLocalSpace(Direction.GetSafeNormal() * Force);
	ProjectileMovement->InitialSpeed = Force;
	ProjectileMovement->MaxSpeed = Force * 1.5f;
	
	UE_LOG(LogClass, Log, TEXT("Velocity: %s"), *ProjectileMovement->Velocity.ToString());
	UE_LOG(LogClass, Log, TEXT("InitialSpeed: %f"),ProjectileMovement->InitialSpeed);
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
				if (disttoplayer <= MaxDamageBlastRadius)
				{
					damagepercent = 1.0f;
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