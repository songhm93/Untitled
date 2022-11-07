#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlameSkill.generated.h"


class USphereComponent; 

UCLASS()
class MELEE_API AFlameSkill : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlameSkill();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComp;

	bool bOvelapped;

	FTimerHandle DestoryFlameTimerHandle;

	float DestroyFlameTime;

	TArray<AActor*> DamageActor;

	float DamageTime; //0.25초마다 대미지 적용

	//델리게이트로 플레이어 공격력 가져와야할듯

	UFUNCTION()
	void OverlappedApplyDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RemoveOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DestroyFlame();

};
