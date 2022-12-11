#pragma once

#include "CoreMinimal.h"
#include "BossMonster.h"
#include "RampageMonster.generated.h"

class ARock;
class UCurveFloat;
class ASquareArea;

struct FSquareArea
{
	FSquareArea(){}
	FSquareArea(int32 _SquareAreaX, int32 _SquareAreaY, int32 _CoordPlusValue, ASquareArea* _SquareArea)
	{
		SquareAreaX = _SquareAreaX;
		SquareAreaY = _SquareAreaY;
		CoordPlusValue = _CoordPlusValue;
		SquareArea = _SquareArea;
	}
	int32 SquareAreaX;
	int32 SquareAreaY;
	int32 CoordPlusValue;
	UPROPERTY()
	ASquareArea* SquareArea;
};


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
	virtual void ActiveSquareArea(bool IsActive) override;
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

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> SquareAreaClass;

	TArray<FSquareArea> SquareAreaCoord;

	int32 SquareAreaXLength;

	int32 SquareAreaYLength;

	FVector SquareAreaRefLoc;

	void SetSquareArea();

	bool IsExistLoc(int32 PlusValue);

	float TestTime;

	bool bActiveSquare;
public:
	
};
