#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Http.h"
#include "../Interface/DBInterface.h"
#include "MeleePlayerController.generated.h"


DECLARE_DELEGATE(FOnLightAttack);
DECLARE_DELEGATE(FOnChargedAttack);

class ABaseCharacter;
class UUserWidget;



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
protected:
	virtual void SetupInputComponent() override;
	FHttpModule* Http;
	void OnPlayerInfoRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	void OnInventoryRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FPlayerInfo ConvertToPlayerInfo(const FString& ResponseString);
	TArray<FPlayerInventory> ConvertToPlayerInventory(const FString& ResponseString);
private:
	void AttackButtonPressed();

	void AttackButtonReleased();

	void TrackingChargedAttack(float DeltaTime);

	void TrackingSprint();

	

	UPROPERTY()
	ABaseCharacter* BaseCharacter;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> MainHUDClass;

	UPROPERTY()
	UUserWidget* MainHUDWidget;

	bool bLeftClickIsPressed;

	float LeftClickTime;

	float ChargedTime;

	bool bCharged;
	
};
