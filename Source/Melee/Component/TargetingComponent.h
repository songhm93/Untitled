#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/RotationMode.h"
#include "TargetingComponent.generated.h"

class ACharacter;
class AController;
class UCameraComponent;
class UCombatComponent;
class UStateManagerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool IsMonster(AActor* Target);
	void SetTargeting(AActor* Target);
protected:
	virtual void BeginPlay() override;
	
private:
	bool FindTarget(FHitResult& HitResult);

	void UpdateTargetingControlRotation(float DeltaTime); //타겟팅되면 카메라가 타겟을 바라보게.

	void SetRotationMode(ERotationMode RotationMode);

	bool CanKeepDist(AActor* Target); //타겟팅을 한 상태에서, 타겟팅이 유지될 거리인지.

	void SetIsTargeting(bool Boolean);

	

	UPROPERTY()
	UCameraComponent* FollowCamera;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	AController* OwnerController;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	AActor* TargetActor;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess = "true"))
	float TargetingDist;

	UPROPERTY(EditAnywhere, Meta=(AllowPrivateAccess = "true"))
	float TargetRotationInterpSpeed;

	UPROPERTY()
	UCombatComponent* CombatComp;

	UPROPERTY()
	UStateManagerComponent* StateManagerComp;

	ERotationMode CurrentRotationMode;

	bool bIsTargeting;
	
	
public: //get
	FORCEINLINE bool GetIsTargeting() const { return bIsTargeting; }
	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }
	FORCEINLINE ERotationMode GetCurrentRotationMode() const { return CurrentRotationMode; }
	
public:	//set
	FORCEINLINE void SetCurrentRotationMode(ERotationMode Mode){ CurrentRotationMode = Mode; }

public:
	void ToggleLockOn();
	void UpdateRotationMode();
	void EnableLockOn();
	void DisableLockOn();
};
