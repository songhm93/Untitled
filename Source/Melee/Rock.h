#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rock.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;

UCLASS()
class MELEE_API ARock : public AActor
{
	GENERATED_BODY()
	
public:	
	ARock();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess ="true"))
	UStaticMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess ="true"))
	UCapsuleComponent* CapsuleComp;
public:
	FORCEINLINE	UStaticMeshComponent* GetMeshComp() const { return MeshComp; }
	FORCEINLINE UCapsuleComponent* GetCapsuleComp() const { return CapsuleComp; }

};
