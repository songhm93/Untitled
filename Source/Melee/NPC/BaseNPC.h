#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/Interactable.h"
#include "../Type/NPCType.h"
#include "BaseNPC.generated.h"

class UWidgetComponent;
class USphereComponent;

UCLASS()
class MELEE_API ABaseNPC : public APawn, public IInteractable
{
	GENERATED_BODY()
	
public:
	ABaseNPC();
	virtual void Interact(AActor* Caller) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DefaultMentWidgetComp;

	UFUNCTION()
	void VisibleMent(bool IsVisible);

	float DefaultMentHideTime;

	FTimerHandle DefaultMentHideTimerHandle;

	FTimerDelegate DefaultMentHideDeletage;

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphereComp;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InteractWidgetComp;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMeshComp;

	UFUNCTION()
	void SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 NPCId;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	ENPCType NPCType;
	
public:
	FORCEINLINE ENPCType GetNPCType() const { return NPCType; }
};

