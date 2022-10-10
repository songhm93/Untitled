#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "Types.h"
#include "BaseWeapon.generated.h"

class UCollisionComponent;

UCLASS()
class MELEE_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();

protected:
	
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName HandSocketName;

	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	UCollisionComponent* CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	float WeaponATK; //무기 공격력

public: //get
	FORCEINLINE FName GetHandSocketName() const { return HandSocketName; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE UCollisionComponent* GetCollisionComp() const { return CollisionComponent; }
	FORCEINLINE float GetWeaponATK() const { return WeaponATK; }
	
public: //set
	FORCEINLINE void SetHandSocketName(FName SocketName) { HandSocketName = SocketName;}
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type;}
	
public:
	void OnHit(FHitResult& HitResult);
	void SimulateWeaponPhysics();
};	

