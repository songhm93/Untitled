#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseQuest.generated.h"

UCLASS(Abstract)
class MELEE_API ABaseQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseQuest();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
