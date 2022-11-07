#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Type/ItemCategory.h"
#include "MasterItem.generated.h"

USTRUCT()
struct FItemInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    FString Name;
    
    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    FString Desc;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    FString UseText = TEXT("Use");

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    bool bCanbeUsed = true;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    bool bCanbeStacked;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    EItemCategory Category;
};

UCLASS()
class MELEE_API AMasterItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AMasterItem();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

private:	
    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	FItemInfo ItemInfo;

public:
    FORCEINLINE FItemInfo GetItemInfo() const { return ItemInfo; }

public:
    FORCEINLINE void SetItemInfo(FItemInfo Info) { ItemInfo = Info; }

};
