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
    FString ItemId;
    
    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    FString Name;
    
    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    FString Desc;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    FString Usetext = TEXT("Use");

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    bool Canuse = true;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    bool Canstack;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    EItemCategory Category;

    UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
    bool Isactor = false;
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
