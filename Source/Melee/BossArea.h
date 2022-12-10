#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossArea.generated.h"

class AEnemyCharacter;
class USphereComponent;
class USceneComponent;

UCLASS()
class MELEE_API ABossArea : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossArea();
	void Init(AEnemyCharacter* Boss);
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 AreaNum;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USphereComponent* AreaSphereComp;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USceneComponent* DefaultComp;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	AEnemyCharacter* BossCharacter;

	UFUNCTION()
	void AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	FORCEINLINE int32 GetAreaNum() const { return AreaNum; }

};
