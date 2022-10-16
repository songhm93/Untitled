#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "Type/Types.h"
#include "BaseWeapon.generated.h"

class UCollisionComponent;

DECLARE_DELEGATE_OneParam(FOnHit, FHitResult&);

UCLASS()
class MELEE_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	FOnHit OnHitResult;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	float WeaponATK; //무기 공격력
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName HandSocketName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	UCollisionComponent* CollisionComponent;

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

