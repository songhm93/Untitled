#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "BaseWeapon.generated.h"


UCLASS()
class MELEE_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	virtual void OnEquipped() override; 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName HandSocketName;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	bool bIsAttachedToHand;
public: //get
	FORCEINLINE bool GetIsAttachedToHand() const { return bIsAttachedToHand; }
public: //set
	FORCEINLINE void SetIsAttachedToHand(bool Boolean) { bIsAttachedToHand = Boolean;}
};	
