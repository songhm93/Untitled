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
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, Category = "Combat", Meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped;
	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemSkeletalMesh;
	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;
public: //get
	FORCEINLINE bool GetIsEquipped() const { return bIsEquipped; }
	FORCEINLINE FName GetAttachSocketName() const { return AttachSocketName; }
	FORCEINLINE USkeletalMeshComponent* GetItemSkeletalMesh() const { return ItemSkeletalMesh; }
	FORCEINLINE UStaticMeshComponent* GetItemStaticMesh() const { return ItemStaticMesh; }
public: //set
	FORCEINLINE void SetIsEquipped(bool IsEquipped) { bIsEquipped = IsEquipped; }
	FORCEINLINE void SetItemSkeletalMesh(USkeletalMeshComponent* SkeletalMesh) { ItemSkeletalMesh = SkeletalMesh; }
	FORCEINLINE void SetItemStaticMesh(UStaticMeshComponent* StaticMesh) { ItemStaticMesh = StaticMesh; }
	FORCEINLINE void SetAttachSocketName(FName SocketName) { AttachSocketName = SocketName; }
public:
	virtual void OnEquipped(); 
	void OnUnequipped();
	UFUNCTION(BlueprintCallable)
	void AttachActor(FName SocketName);
	virtual void Interact(AActor* Caller) override;
	UPrimitiveComponent* GetItemMesh();
};
