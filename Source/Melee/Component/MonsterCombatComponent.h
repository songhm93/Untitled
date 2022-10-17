// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/Stats.h"
#include "MonsterCombatComponent.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(float, FGetCurrentStatValue, EStats);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UMonsterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMonsterCombatComponent();
	FGetCurrentStatValue GetCurrentStatValue;
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	bool bEnemyWeaponCollisionEnabled;
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	UPROPERTY()
	UPrimitiveComponent* CollisionMeshComponent;

	void CollisionTrace();
	void ClearHitActors();

	FName RightWeaponStartSocketName;
	FName RightWeaponEndSocketName;
	FName LeftWeaponStartSocketName;
	FName LeftWeaponEndSocketName;
	FName WeaponStartSocketName;
	FName WeaponEndSocketName;
	float CloseAttackCorrectionValue;
	
public:	//get
	FORCEINLINE	TArray<AActor*> GetAlreayHitActors() const { return AlreadyHitActors; }
	FORCEINLINE	bool GetIsCollisionEnabled() const { return bEnemyWeaponCollisionEnabled; }
	FORCEINLINE	UPrimitiveComponent* GetCollisionMeshComponent() const { return CollisionMeshComponent; }
public:
	FORCEINLINE void SetCollisionMeshComponent(UPrimitiveComponent* Comp) { CollisionMeshComponent = Comp; }
	FORCEINLINE void SetStartSocketName(FName SocketName) { RightWeaponStartSocketName = SocketName; }
	FORCEINLINE void SetEndSocketName(FName SocketName) { RightWeaponEndSocketName = SocketName; }
	FORCEINLINE void SetCloseAttackCorrectionValue(float Value) { CloseAttackCorrectionValue = Value; }
public:
	UFUNCTION(BlueprintCallable)
	void EnableCollision(bool bLeftWeapon);
	UFUNCTION(BlueprintCallable)
	void DisableCollision();

		
};
