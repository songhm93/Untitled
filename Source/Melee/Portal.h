#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Portal.generated.h"

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
	FString Identify;


};
