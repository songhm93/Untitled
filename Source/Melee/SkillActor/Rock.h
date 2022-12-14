#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Rock.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
class UParticleSystem;
class USoundCue;
class UAttackDamageType;

UCLASS()
class MELEE_API ARock : public AActor
{
	GENERATED_BODY()
	
public:	
	ARock();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess ="true"))
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess ="true"))
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;

	void HitEffect();

	float Damage;

	float SpawnTime;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAttackDamageType> DamageType;

public:
	FORCEINLINE	UStaticMeshComponent* GetMeshComp() const { return MeshComp; }
	FORCEINLINE UCapsuleComponent* GetCapsuleComp() const { return CapsuleComp; }

};
