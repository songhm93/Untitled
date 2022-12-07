// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillRange.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS()
class MELEE_API ASkillRange : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillRange();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;

	float Skill4DestoryTime;

	FTimerHandle Skill4DestroyTimerHandle;
};
