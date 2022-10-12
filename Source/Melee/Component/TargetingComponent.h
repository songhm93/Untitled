#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/RotationMode.h"
#include "TargetingComponent.generated.h"

class ACharacter;
class AController;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	UCameraComponent* FollowCamera;

	
	bool bIsTargeting;
	
	bool FindTarget(FHitResult& HitResult);
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
	void UpdateTargetingControlRotation(float DeltaTime); //타겟팅되면 카메라가 타겟을 바라보게.
	void SetRotationMode(ERotationMode RotationMode);
	ERotationMode CurrentRotationMode;

	bool CanKeepDist(AActor* Target); //타겟팅을 한 상태에서 타겟팅 거리가 유지될 거리인지.
	void SetIsTargeting(bool Boolean);
	
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
