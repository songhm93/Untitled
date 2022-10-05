#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "PickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MELEE_API APickupActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	APickupActor();
protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AActor* Caller) override;
private:
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AActor> SpawnActor;
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta=(AllowPrivateAccess = "true"))
	USphereComponent* Sphere;

};
