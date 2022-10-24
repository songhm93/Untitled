#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "NonPreemptiveMonster.generated.h"

class UWidgetComponent;

UCLASS()
class MELEE_API ANonPreemptiveMonster : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ANonPreemptiveMonster();
	virtual void OnTargeted(bool IsTargeted) override;
	virtual void Dead() override;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HPBarWidget;

	void HideHPBar();

	void HPBarOnOff(bool Show);

	FTimerHandle HideHPBarTimerHandle;

	float HideHPBarTime;
};
