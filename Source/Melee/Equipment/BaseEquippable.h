#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "../Interface/Interactable.h"
#include "../Type/Types.h"
#include "BaseEquippable.generated.h"

UCLASS()
class MELEE_API ABaseEquippable : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseEquippable();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemStaticMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;

	EEquipmentType EquipmentType;
public: //get
	FORCEINLINE FName GetAttachSocketName() const { return AttachSocketName; }
	FORCEINLINE UStaticMeshComponent* GetItemStaticMeshComp() const { return ItemStaticMeshComp; }
	FORCEINLINE EEquipmentType GetEquipmentType() const { return EquipmentType; }

public: //set
	FORCEINLINE void SetItemStaticMeshComp(UStaticMeshComponent* StaticMeshComp) { ItemStaticMeshComp = StaticMeshComp; }
	FORCEINLINE void SetAttachSocketName(FName SocketName) { AttachSocketName = SocketName; }
	FORCEINLINE void SetEquipmentType(EEquipmentType Type) { EquipmentType = Type; }

public:
	UPrimitiveComponent* GetItemMeshComp();
	
};
