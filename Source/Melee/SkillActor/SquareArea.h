#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquareArea.generated.h"

class UBoxComponent;

UCLASS()
class MELEE_API ASquareArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ASquareArea();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComp;

	UFUNCTION()
	void OverlappedApplyDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void RemoveOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool bOvelapped;

	TArray<AActor*> DamageActor;

	float DamageTime;

	bool IsMove;

	FVector CurrentLoc;

	float DestroyTime;
public:
	void RequestDestroy();
};
