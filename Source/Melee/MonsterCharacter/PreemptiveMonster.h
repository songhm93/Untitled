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
	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Special1Montage;

	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Special2Montage;

	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Special3Montage;

private:
	UFUNCTION()
	void AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
};
