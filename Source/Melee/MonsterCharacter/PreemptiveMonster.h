#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "PreemptiveMonster.generated.h"


UCLASS()
class MELEE_API APreemptiveMonster : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	APreemptiveMonster();
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
};
