#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "NonPreemptiveMonster.generated.h"


UCLASS()
class MELEE_API ANonPreemptiveMonster : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ANonPreemptiveMonster();

protected:
	virtual void BeginPlay() override;
	virtual void ReceiveDamage(
		AActor* DamagedActor, 
		float EnemyATK, 
		AController* InstigatedBy, 
		FVector HitLocation, 
		UPrimitiveComponent* FHitComponent, 
		FName BoneName, 
		FVector ShotFromDirection, 
		const UDamageType* DamageType, 
		AActor* DamageCauser) override;

private:
	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
