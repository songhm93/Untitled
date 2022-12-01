#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Portal.generated.h"

UENUM(BlueprintType)
enum class EPortalIdentify : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    FIRST UMETA(DisplayName = "FIRST"),
    SECOND UMETA(DisplayName = "SECOND"),
    THIRD UMETA(DisplayName = "THIRD"),
    FOURTH UMETA(DisplayName = "FOURTH"),
    FIFTH UMETA(DisplayName = "FIFTH"),
    SIXTH UMETA(DisplayName = "SIXTH"),
    SEVENTH UMETA(DisplayName = "SEVENTH"),
    EIGHTH UMETA(DisplayName = "EIGHTH"),

    MAX UMETA(DisplayName = "MAX")
};

class UFadeWidget;

UCLASS()
class MELEE_API APortal : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	APortal();
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AActor* Caller) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SM1;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SM2;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	EPortalIdentify Identify;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> FadeWidgetClass;

	UPROPERTY()
	UFadeWidget* FadeWidget;

	FTimerHandle SwitchingTimerHandle;

	FTimerDelegate SwitchingDelegate;

	float FadeTime;

	UFUNCTION()
	void SwitchingLocation(AActor* Caller, TArray<AActor*> OutActor);

	FTimerHandle RemoveFadeWidgetTimerHandle;

	float RemoveFadeWidgetTime;

	void RemoveFadeWidget();
public:
	FORCEINLINE EPortalIdentify GetIdentify() const { return Identify; }

};
