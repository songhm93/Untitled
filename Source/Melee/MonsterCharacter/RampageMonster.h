#pragma once

#include "CoreMinimal.h"
#include "PreemptiveMonster.h"
#include "../Interface/BossInterface.h"
#include "RampageMonster.generated.h"

class ARock;

UCLASS()
class MELEE_API ARampageMonster : public APreemptiveMonster, public IBossInterface
{
	GENERATED_BODY()

public:
	ARampageMonster();
	virtual void Special1() override;
	virtual void Special2() override;
	virtual void Special3() override;
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> RockActor;

	UPROPERTY()
	ARock* Rock;

	void DeattachRock();
	
};
