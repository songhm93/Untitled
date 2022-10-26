#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "../Type/CollisionPart.h"
#include "DualWeapon.generated.h"

UCLASS()
class MELEE_API ADualWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ADualWeapon();

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DualSwordStaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName SecondWeaponAttachSocket;

	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName SecondWeaponHandSocket;

public: //get
	FORCEINLINE UStaticMeshComponent* GetDualSwordStaticMeshComp() const { return DualSwordStaticMeshComp;}
	FORCEINLINE FName GetSecondWeaponHandSocket() const { return SecondWeaponHandSocket; }
	FORCEINLINE FName GetSecondWeaponAttachSocket() const { return SecondWeaponAttachSocket; }
	
};
