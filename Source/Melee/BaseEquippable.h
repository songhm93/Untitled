#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BaseEquippable.generated.h"

UCLASS()
class MELEE_API ABaseEquippable : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ABaseEquippable();

protected:
	virtual void BeginPlay() override;
	
	
public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemSkeletalMesh;
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(VisibleAnywhere, Category = "Combat", Meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComp;
public: //get
	FORCEINLINE bool GetIsEquipped() const { return bIsEquipped; }
	FORCEINLINE FName GetAttachSocketName() const { return AttachSocketName; }
public: //set
	FORCEINLINE void SetIsEquipped(bool IsEquipped) { bIsEquipped = IsEquipped; }
public:
	UPrimitiveComponent* GetItemMesh();
	virtual void OnEquipped(); 
	void OnUnequipped();
	UFUNCTION(BlueprintCallable)
	void AttachActor(FName SocketName);
	virtual void Interact(AActor* Caller) override;
};
