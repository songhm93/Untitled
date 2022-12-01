#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UWidgetComponent;
class UInventoryWidget;
class UItemThrowWidget;
class UGetItemWidget;
class UGetExpWidget;
class UVerticalBox;
class UHotkeyWidget;

UCLASS()
class MELEE_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UItemThrowWidget* ItemThrowWidget;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UVerticalBox* GetExpBox;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGetExpWidget> GetExpWidgetClass;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeyWidget* Potion1Hotkey;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeyWidget* Potion2Hotkey;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeyWidget* Skill1Hotkey;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeyWidget* Skill2Hotkey;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeyWidget* Skill3Hotkey;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeyWidget* Skill4Hotkey;

	UPROPERTY()
	UGetExpWidget* GetExpWidget;

	int32 ExistGetExpWidgetNum;

	int32 WaitGetExpWidgetNum;

	UPROPERTY()
	TArray<int32> GetExpWidgetQueue;

	UPROPERTY()
	TArray<UGetExpWidget*> GetExpWidgetList;

	void GetExpWidgetScrollUp();

	
public:
	FORCEINLINE UItemThrowWidget* GetItemThrowWidget() const { return ItemThrowWidget; }
	FORCEINLINE UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }
	FORCEINLINE UHotkeyWidget* GetPotion1Hotkey() const { return Potion1Hotkey; }
	FORCEINLINE UHotkeyWidget* GetPotion2Hotkey() const { return Potion2Hotkey; }
	FORCEINLINE UHotkeyWidget* GetSkill1Hotkey() const { return Skill1Hotkey; }
	FORCEINLINE UHotkeyWidget* GetSkill2Hotkey() const { return Skill2Hotkey; }
	FORCEINLINE UHotkeyWidget* GetSkill3Hotkey() const { return Skill3Hotkey; }
	FORCEINLINE UHotkeyWidget* GetSkill4Hotkey() const { return Skill4Hotkey; }

	void GetExp(int32 Exp);
	void RemoveGetExpWidget();

};
