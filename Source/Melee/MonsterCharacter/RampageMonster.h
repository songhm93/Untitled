#pragma once

#include "CoreMinimal.h"
#include "BossMonster.h"
#include "RampageMonster.generated.h"

class ARock;


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

	void Special4Explode();

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

public:
	
};
