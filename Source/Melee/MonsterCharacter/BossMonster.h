#pragma once

#include "CoreMinimal.h"
#include "PreemptiveMonster.h"
#include "../Interface/BossInterface.h"
#include "BossMonster.generated.h"

class UEnemyHPBarWidget;


UCLASS()
class MELEE_API ABossMonster : public APreemptiveMonster, public IBossInterface
{
	GENERATED_BODY()

public:
	ABossMonster();
	virtual void Special1() override;
	virtual void Special2() override;
	virtual void Special3() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnTargeted(bool IsTargeted) override;
	virtual void Dead() override;
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Special1Montage;

	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Special2Montage;

	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Special3Montage;

	UPROPERTY(EditAnywhere, Category = "SpecialMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AgroMontage;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	FText BossName;
	
private:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<UEnemyHPBarWidget> HPBarWidget;

	void HideHPBar();

	void HPBarOnOff(bool Show);

	FTimerHandle HideHPBarTimerHandle;

	float HideHPBarTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HPBarWidgetComp;

	UPROPERTY()
	UUserWidget* BossHPBarWidget;

	bool bExecuteHide;

	float HideTime;

	void HiddenHPBar();

public:
	
};
