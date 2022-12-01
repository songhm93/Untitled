#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Http.h"
#include "../Interface/DBInterface.h"
#include "../Interface/InventoryInterface.h"
#include "MeleePlayerController.generated.h"


DECLARE_DELEGATE(FOnLightAttack);
DECLARE_DELEGATE(FOnChargedAttack);

class ABaseCharacter;
class UGetItemWidget;
class UMerchantWidget;
class UStatsComponent;
class UFadeWidget;

USTRUCT()
struct FGetItemQueue
{
	GENERATED_BODY()
	FGetItemQueue(){}
	FGetItemQueue(FItemInfoInSlot _ItemInfo, int32 _Amount)
	{
		ItemInfo = _ItemInfo;
		Amount = _Amount;
	}


	FItemInfoInSlot ItemInfo;

	int32 Amount;

};	



UCLASS()
class MELEE_API AMeleePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMeleePlayerController();
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	FOnLightAttack OnLightAttack;
	FOnChargedAttack OnChargedAttack;
	void RequestEntry();
	void SaveData();
	void GetItemWidgetVisible(FItemInfoInSlot AddItemInfo, int32 Amount);
	void RemoveGetItemWidget();

protected:
	virtual void SetupInputComponent() override;
	FHttpModule* Http;
	void OnPlayerInfoRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	void OnInventoryRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FPlayerInfoDB ConvertToPlayerInfo(const FString& ResponseString);
	TArray<FPlayerInventoryDB> ConvertToPlayerInventory(const FString& ResponseString);

private:
	void AttackButtonPressed();

	void AttackButtonReleased();

	void TrackingChargedAttack(float DeltaTime);

	void TrackingSprint();

	void GetItemWidgetScrollUp();

	UPROPERTY()
	UStatsComponent* StatComp;

	UPROPERTY()
	TArray<UGetItemWidget*> GetItemWidgetList;

	UPROPERTY()
	UGetItemWidget* GetItemWidget;

	UPROPERTY()
	ABaseCharacter* BaseCharacter;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGetItemWidget> GetItemWidgetClass;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFadeWidget> DeathWidgetClass;

	bool bLeftClickIsPressed;

	float LeftClickTime;

	float ChargedTime;

	bool bCharged;

	int32 ExistGetItemWidgetNum; //현재 띄워져있는 위젯 갯수 -> 최대 4개

	int32 WaitGetItemWidgetNum; //큐에 들어가 있는
	//큐에 들어가있는 갯수   -> 위 변수가 4면 이게 늘어남
	TArray<FGetItemQueue> GetItemWidgetQueue;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMerchantWidget> MerchantWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UMerchantWidget* MerchantWidget;

public:
	FORCEINLINE TSubclassOf<UMerchantWidget> GetMerchantWidgetClass() const { return MerchantWidgetClass; }
	FORCEINLINE TSubclassOf<UFadeWidget> GetDeathWidgetClass() const { return DeathWidgetClass; }

public:
	FORCEINLINE void SetMerchantWidget(UMerchantWidget* Widget) { MerchantWidget = Widget; }
	
};
