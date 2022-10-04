#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEquippable.generated.h"

UCLASS()
class MELEE_API ABaseEquippable : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseEquippable();

protected:
	virtual void BeginPlay() override;
	virtual void OnEquipped(); //자식들한테서 재정의해서 기능 추가할 것
	virtual void OnUnequipped();
	void AttachActor(FName ScoketName);
public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemSkeletalMesh;
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemStaticMesh;
	UPROPERTY(VisibleAnywhere, Category = "Combat", Meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped;
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName AttachSocketName;
public: //get
	
public: //set

public:
	UPrimitiveComponent* GetItemMesh();
};
