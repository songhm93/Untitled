#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollisionComponent.generated.h"

class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCollisionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

private:
	bool bIsCollisionEnabled;
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	UPROPERTY()
	UPrimitiveComponent* CollisionMeshComponent;
	UFUNCTION(BlueprintCallable)
	void EnableCollision();
	UFUNCTION(BlueprintCallable)
	void DisableCollision();
	void CollisionTrace();
	void ClearHitActors();

	UPROPERTY()
	ABaseWeapon* Weapon;
public:	//get
	FORCEINLINE	TArray<AActor*> GetAlreayHitActors() const { return AlreadyHitActors; }
	FORCEINLINE	bool GetIsCollisionEnabled() const { return bIsCollisionEnabled; }
	FORCEINLINE	UPrimitiveComponent* GetCollisionMeshComponent() const { return CollisionMeshComponent; }
public:
	FORCEINLINE void SetCollisionMeshComponent(UPrimitiveComponent* Comp) { CollisionMeshComponent = Comp; }
		
};
