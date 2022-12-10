#pragma once

#include "CoreMinimal.h"
#include "BossMonster.h"
#include "RampageMonster.generated.h"

class ARock;
class UCurveFloat;

UCLASS()
class MELEE_API ARampageMonster : public ABossMonster
{
	GENERATED_BODY()

public:
	ARampageMonster();
	virtual void Special1() override;
	virtual void Special2() override;
	virtual void Special3() override;
	virtual void Special4() override;
	virtual void Special5() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> RockActor;

	UPROPERTY()
	ARock* Rock;

	bool bIsFirstMontagePlayed;

	bool bIsSecondMontagePlayed;

	void DeattachRock();

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> SkillRangeActor;

	FVector FindFloor();

	FVector FindRandomFloor();

	FTimerHandle SkillRangeDestroyTimerHandle;

	FTimerDelegate SkillRangeDestroyDeletage;

	float SkillRangeDestroyTime;

	UFUNCTION()
	void SkillRangeDestroy(AActor* RangeActor);

	bool bPauseTimeTrack;

	float S3PlayTime;

	void PauseS3Montage();

	bool bPauseS3Montage;

	uint32 S3RandSkillCount;

	uint32 CurrentSkillCount;

	void ResumeS3Montage();

	bool bPlayCurve;

	FTimerHandle HeightCurveTimerHandle;

	float CurveTime;

	FVector CurrentLocation;

	void SetHeight(float DeltaTime);

	void FinishSetHeight();

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UCurveFloat* MonsterHeightCurve;

	float DistFromPlayer;

	FTimerHandle FlightTimerHandle;

	float FlightTime;

	void MonsterFalling();

	FVector JumpTargetLocation;
	
public:
	
};
