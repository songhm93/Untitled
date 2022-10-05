#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, Meta=(AllowPrivateAccess = "true"))
	bool bCombatState;
public: //get
	ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetCombatState() const { return bCombatState; }
public: //set
	FORCEINLINE void SetCombatState(bool Boolean) { bCombatState = Boolean; }
public:
	void SetEquippedWeapon(ABaseWeapon* NewWeapon);
		
};
