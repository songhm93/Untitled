#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MeleeAnimInstance.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Components/WidgetComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "../AttackDamageType.h"
#include "../Interface/Interactable.h"
#include "../Interface/ItemInterface.h"
#include "../Type/Stats.h"
#include "../Type/Types.h"
#include "../Equipment/DualWeapon.h"
#include "../Equipment/BaseArmor.h"
#include "../Component/CombatComponent.h"
#include "../Component/StateManagerComponent.h"
#include "../Component/StatsComponent.h"
#include "../Component/TargetingComponent.h"
#include "../Component/InventoryComponent.h"
#include "../Component/QuestLogComponent.h"
#include "../Item/MasterItem.h"
#include "../SkillActor/Rock.h"
#include "../Widget/InventoryWidget.h"
#include "../Widget/MainHUDWidget.h"
#include "../Widget/FadeWidget.h"
#include "../Quest/BaseQuest.h"
#include "../PlayerController/MeleePlayerController.h"


ABaseCharacter::ABaseCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	CombatCompo = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatCompo"));
	StateManagerComp = CreateDefaultSubobject<UStateManagerComponent>(TEXT("StateManagerComp"));
	StatComp = CreateDefaultSubobject<UStatsComponent>(TEXT("StatComp"));
	TargetingComp = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComp"));
	LockOnWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	QuestLogComp = CreateDefaultSubobject<UQuestLogComponent>(TEXT("QuestLogComp"));
	
	bLeftClicked = false;
	
	LockOnWidgetComp->SetupAttachment(GetMesh());
	PelvisBoneName = TEXT("pelvis");
	DestroyDeadTime = 4.f;

	WalkSpeed = 300.f;
	JogSpeed = 500.f;
	SprintSpeed = 700;
	SprintStaminaCost = 0.2f;
	bSprintKeyPressed = false;
	MouseSensitivity = 25.f;
	bHitFront = false;
	bAltPressed = false;
	RespawnTime = 5.f;

    ResetCombat();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	ResetCombat();
	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::ReceiveDamage);
	if(StateManagerComp)
	{
		StateManagerComp->OnStateBegin.AddUObject(this, &ThisClass::CharacterStateBegin);
	}
	
	if(StatComp)
	{
		//StatComp->InitStats();
	}

	if(LockOnWidgetComp)
	{
		LockOnWidgetComp->SetVisibility(false);
		LockOnWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		LockOnWidgetComp->SetDrawSize(FVector2D(14.f, 14.f));
	}

	if(MainHUDClass)
    {
        MainHUDWidget = CreateWidget<UMainHUDWidget>(GetWorld(), MainHUDClass);
        
		if(MainHUDWidget)
        {
            MainHUDWidget->AddToViewport();
        }
    }

	APlayerController* PC = Cast<APlayerController>(GetController());
	if(PC)
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true; 
		PC->bEnableMouseOverEvents = true;
	}

	if(InventoryComp)
	{
		InventoryComp->OnEquipWeapon.BindUObject(this, &ThisClass::EquipWeapon);
		InventoryComp->OnEquipArmor.BindUObject(this, &ThisClass::EquipArmor);
		InventoryComp->UnequipArmor.BindUObject(this, &ThisClass::UnequipArmor);
		InventoryComp->OnEquippedWeaponSpawn.BindUObject(this, &ThisClass::EquippedWeaponSpawn);
		InventoryComp->OnEquippedArmorApply.BindUObject(this, &ThisClass::EquippedArmorApply);
	}
	
}

void ABaseCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("ToggleCombat", IE_Pressed, this, &ThisClass::ToggleCombat);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ThisClass::InteractButtonPressed);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ThisClass::Dodge);
	PlayerInputComponent->BindAction("ToggleWalk", IE_Pressed, this, &ThisClass::ToggleWalk);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ThisClass::SprintButtonPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ThisClass::SprintButtonReleased);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ThisClass::HeavyAttack); //삭제예정
	PlayerInputComponent->BindAction("ToggleLockOn", IE_Pressed, this, &ThisClass::ToggleLockOn);
	PlayerInputComponent->BindAction("Skill1", IE_Pressed, this, &ThisClass::HotkeySlot1ButtonPressed);
	PlayerInputComponent->BindAction("Skill2", IE_Pressed, this, &ThisClass::HotkeySlot2ButtonPressed);
	PlayerInputComponent->BindAction("Skill3", IE_Pressed, this, &ThisClass::HotkeySlot3ButtonPressed);
	PlayerInputComponent->BindAction("Ultimate", IE_Pressed, this, &ThisClass::HotkeySlot4ButtonPressed);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &ThisClass::LeftClickPressed);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &ThisClass::LeftClickReleased);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &ThisClass::ToggleInventory);
	PlayerInputComponent->BindAction("Alt", IE_Pressed, this, &ThisClass::AltPressed);
	PlayerInputComponent->BindAction("Alt", IE_Released, this, &ThisClass::AltReleased);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ThisClass::Test);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ABaseCharacter::TurnRight);
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ABaseCharacter::LookUp);
	PlayerInputComponent->BindAxis("CameraZoomInOut", this, &ABaseCharacter::CameraZoomInOut);
	
}


void ABaseCharacter::TurnRight(float Rate)
{
	if(bLeftClicked)
		AddControllerYawInput(Rate * MouseSensitivity * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::LookUp(float Rate)
{
	if(bLeftClicked)
		AddControllerPitchInput(Rate * MouseSensitivity * GetWorld()->GetDeltaSeconds());
}

void ABaseCharacter::Jump()
{
	TArray<ECurrentState> CharacterStates;
	CharacterStates.Add(ECurrentState::DODGING);
	CharacterStates.Add(ECurrentState::DISABLED);
	CharacterStates.Add(ECurrentState::ATTACKING);
	
	if(StateManagerComp && StateManagerComp->IsCurrentStateEqualToThis(CharacterStates) && StateManagerComp->GetCurrentAction() == ECurrentAction::CHARGED_ATTACK) return;
	StopAnimMontage();
	Super::Jump();
	ResetCombat();
}

void ABaseCharacter::MoveForward(float Value)
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() == ECurrentState::ATTACKING) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		if(bSprintKeyPressed) SetMovementType(EMovementType::SPRINTING);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::MoveRight(float Value)
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() == ECurrentState::ATTACKING) return;
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::ToggleCombat()
{
	if(!CanToggleCombat() || (CombatCompo && !CombatCompo->GetEquippedWeapon())) return;

	EWeaponType WeaponType = CombatCompo->GetEquippedWeapon()->GetWeaponType();

	if(StateManagerComp)
	{
		StateManagerComp->SetCurrentState(ECurrentState::GENERAL_STATE);
		
		UAnimMontage* EnterCombatMontage = nullptr;
		UAnimMontage* ExitCombatMontage = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::LIGHT_SWORD:
			EnterCombatMontage = CombatCompo->GetLSEnterCombatMontage();
			ExitCombatMontage = CombatCompo->GetLSExitCombatMontage();
			break;
		case EWeaponType::DUAL_SWORD:
			EnterCombatMontage = CombatCompo->GetDSEnterCombatMontage();
			ExitCombatMontage = CombatCompo->GetDSEnterCombatMontage();
			break;
		}

		if (StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::NONE_COMBAT_STATE)
		{
			
			PerformAction(EnterCombatMontage, ECurrentState::GENERAL_STATE, ECurrentAction::ENTER_COMBAT);
			StateManagerComp->SetCurrentCombatState(ECurrentCombatState::COMBAT_STATE);
			if(GetMesh() && GetMesh()->GetAnimInstance())
			{
				Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetCombatState(true);
			}
			if(TargetingComp) TargetingComp->UpdateRotationMode();
		}
		else if(StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::COMBAT_STATE)
		{
			PerformAction(ExitCombatMontage, ECurrentState::GENERAL_STATE, ECurrentAction::EXIT_COMBAT);
			CombatCompo->ResetAttackCount();
			StateManagerComp->SetCurrentCombatState(ECurrentCombatState::NONE_COMBAT_STATE);
			if(GetMesh() && GetMesh()->GetAnimInstance())
			{
				Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetCombatState(false);
			}
			if(TargetingComp)
			{
				TargetingComp->DisableLockOn();
				TargetingComp->UpdateRotationMode();
			}
		}
	}
}

void ABaseCharacter::InteractButtonPressed()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 300.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Reserve(1);
	ObjectTypesArray.Emplace(ECollisionChannel::ECC_GameTraceChannel1);
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, 
		Start, 
		End, 
		30.f, 
		ObjectTypesArray, 
		false, 
		IgnoredActors, 
		EDrawDebugTrace::None, 
		OutHit, 
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		10.f);

	if(OutHit.GetActor())
	{
		IInteractable* Interactable = Cast<IInteractable>(OutHit.GetActor());
		if(Interactable)
		{
			Interactable->Interact(this);
		}
	}
}

void ABaseCharacter::HeavyAttack() //삭제예정
{
	if(CombatCompo)
		CombatCompo->HeavyAttack();
}

void ABaseCharacter::Dodge()
{
	if(!CanDodge()) return;
	
	CombatCompo->PerformDodge();
}


bool ABaseCharacter::CanToggleCombat()
{
	TArray<ECurrentState> CharacterStates;
	CharacterStates.Add(ECurrentState::ATTACKING);
	CharacterStates.Add(ECurrentState::DODGING);
	CharacterStates.Add(ECurrentState::DEAD);
	CharacterStates.Add(ECurrentState::DISABLED);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = !StateManagerComp->IsCurrentStateEqualToThis(CharacterStates);
	return ReturnValue;
}

bool ABaseCharacter::CanDodge()
{
	if(StatComp && CombatCompo &&(StatComp->GetCurrentStatValue(EStats::STAMINA) < CombatCompo->GetDodgeStaminaCost())) return false;
	
	TArray<ECurrentState> CharacterStates;
	CharacterStates.Add(ECurrentState::DODGING);
	CharacterStates.Add(ECurrentState::DEAD);
	CharacterStates.Add(ECurrentState::DISABLED);
	CharacterStates.Add(ECurrentState::GENERAL_STATE);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = (!StateManagerComp->IsCurrentStateEqualToThis(CharacterStates))  && (!GetCharacterMovement()->IsFalling()) && (!StateManagerComp->GetIsDodgeCooldown());
	return ReturnValue;
}

FRotator ABaseCharacter::GetDesiredRotation() //구르기시 캐릭터가 움직이고 있는 방향의 회전값을 반환
{
	if(GetCharacterMovement())
	{
		FVector LastVector = GetCharacterMovement()->GetLastInputVector();
		bool VectorResult = UKismetMathLibrary::NotEqual_VectorVector(LastVector, FVector(0.f, 0.f, 0.f), 0.001f);
		if(VectorResult)
		{
			return UKismetMathLibrary::MakeRotFromX(GetLastMovementInputVector());
		}
	}
	return GetActorRotation();
}

void ABaseCharacter::ResetCombat()
{
	if(CombatCompo) CombatCompo->ResetAttackCount();
	if(StateManagerComp)
	{
		StateManagerComp->ResetState();
		StateManagerComp->ResetAction();
	}
}

void ABaseCharacter::ReceiveDamage(
	AActor* DamagedActor, 
	float EnemyATK, 
	const UDamageType* DamageType, 
	AController* InstigatedBy, 
	AActor* DamageCauser)
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() == ECurrentState::DODGING) return;
	
	if(InstigatedBy)
	{
		float DotProductResult = GetDotProductTo(InstigatedBy->GetPawn()); //맞은 캐릭터와 때린 캐릭터간의 내적
		bHitFront = FMath::IsWithin(DotProductResult, -0.1f, 1.f);
	}

	ApplyImpactEffect();

	if(Cast<UAttackDamageType>(DamageType))
	{
		ApplyHitReaction(Cast<UAttackDamageType>(DamageType)->GetDamageType());
	}
		
	
	CalcReceiveDamage(EnemyATK);
}

bool ABaseCharacter::CalcCritical(float Percent)
{
	bool ReturnValue = false;
	const float RandValue = FMath::RandRange(0.f, 999.f);
	const float MaxRangeValue = Percent * 10.f;

	if(MaxRangeValue >= RandValue)
	{
		ReturnValue = true;
	}

	return ReturnValue;
}

void ABaseCharacter::CalcReceiveDamage(float EnemyATK) //받는 총 대미지 계산
{
	bool IsCritical = CalcCritical(10.f);
	//대미지 계산
	if(StatComp)
	{
		const float Def = StatComp->GetCurrentStatValue(EStats::DEF);
		float Result = FMath::Clamp((EnemyATK * FMath::RandRange(0.8, 1.2)) * (1 - (Def / (100 + Def))), 0, INT_MAX);
		if(IsCritical) Result *= 2.f;
		StatComp->PlusCurrentStatValue(EStats::HP, Result * -1); //HP 적용
		if(StatComp->GetCurrentStatValue(EStats::HP) <= 0)
		{
			if(StateManagerComp)
				StateManagerComp->SetCurrentState(ECurrentState::DEAD);
		}
		//Result로 대미지 위젯
		ShowDamageText(Result, IsCritical);
	}
}



void ABaseCharacter::Dead()
{
	AMeleePlayerController* PC = Cast<AMeleePlayerController>(GetController());
	if(PC)
	{
		DeathWidget = CreateWidget<UFadeWidget>(GetWorld(), PC->GetDeathWidgetClass());
		if(DeathWidget)
		{
			DeathWidget->AddToViewport();
		}
	}
	
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->Deactivate();
	}
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	if(DeathMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
	}
	
	GetWorld()->GetTimerManager().SetTimer(DestroyDeadTimerHandle, this, &ThisClass::DestroyDead, DestroyDeadTime);
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::Respawn, RespawnTime);
}

void ABaseCharacter::DestroyDead()
{
	if(CombatCompo && Cast<ADualWeapon>(CombatCompo->GetEquippedWeapon()))
	{
		Cast<ADualWeapon>(CombatCompo->GetEquippedWeapon())->ShowHideCharacter(false);
	}
	
}

void ABaseCharacter::Respawn()
{
	FVector Loc = FVector(-16455.0, -15790.0, 734.374246);
	SetActorLocation(Loc);
	if(StateManagerComp && TargetingComp && StatComp)
	{
		StateManagerComp->SetCurrentState(ECurrentState::NOTHING);
	
		TargetingComp->DisableLockOn();
		StatComp->SetCurrentStatValue(EStats::HP, StatComp->GetMaxValue(EStats::HP) * 0.3);
		StatComp->SetCurrentStatValue(EStats::STAMINA, StatComp->GetMaxValue(EStats::STAMINA) * 0.3);
	}
	if(Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetIsDead(false);
	}
	
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->Activate();
	}
	if(CombatCompo && Cast<ADualWeapon>(CombatCompo->GetEquippedWeapon()))
	{
		Cast<ADualWeapon>(CombatCompo->GetEquippedWeapon())->ShowHideCharacter(true);
	}
	if(DeathWidget)
	{
		DeathWidget->RemoveFromParent();
	}
}

float ABaseCharacter::GetRemainRespawnTime()
{
	if(StateManagerComp && (StateManagerComp->GetCurrentState() != ECurrentState::DEAD)) return 0.f;
	
	return GetWorld()->GetTimerManager().GetTimerRemaining(RespawnTimerHandle);
}

void ABaseCharacter::Test()
{	
	//테스트할 함수 넣기. Key Mapping : 5
	if(StatComp)
	{
		StatComp->DevLevelUp();
	}
	

}

bool ABaseCharacter::CanReceiveDamage()
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() != ECurrentState::DEAD)
		return true;
	else 
		return false;
}

void ABaseCharacter::CharacterStateBegin(ECurrentState State)
{
	switch (State)
	{
	case ECurrentState::NOTHING:
		
		break;
	case ECurrentState::ATTACKING:
		
		break;
	case ECurrentState::DODGING:
		
		break;
	case ECurrentState::GENERAL_STATE:
		
		break;
	case ECurrentState::DEAD:
			Dead();
		break;
	case ECurrentState::DISABLED:
		
		break;
	}
}

void ABaseCharacter::PerformAction(UAnimMontage* Montage, ECurrentState State, ECurrentAction Action)
{
	if(CombatCompo && CombatCompo->GetEquippedWeapon() && StateManagerComp)
	{
		StateManagerComp->SetCurrentState(State);
		StateManagerComp->SetCurrentAction(Action);

		if(Montage)	PlayAnimMontage(Montage);
	}
}

void ABaseCharacter::ToggleWalk()
{
	if(StateManagerComp && StateManagerComp->GetMovementType() != EMovementType::WALKING)
		SetMovementType(EMovementType::WALKING);
	else
		SetMovementType(EMovementType::JOGGING);
	
}

void ABaseCharacter::SprintButtonPressed()
{
	bSprintKeyPressed = true;
	if(GetVelocity().Size() <= 0.f) return;
	SetMovementType(EMovementType::SPRINTING);
}

void ABaseCharacter::SprintButtonReleased()
{
	bSprintKeyPressed = false;
	SetMovementType(EMovementType::JOGGING);
}

void ABaseCharacter::SetMovementType(EMovementType Type)
{	
	if(GetCharacterMovement() && StateManagerComp)
	{
		StateManagerComp->SetMovementType(Type);
		switch(Type)
		{
			case EMovementType::WALKING:
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
				break;
			case EMovementType::JOGGING:
				GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
				break;
			case EMovementType::SPRINTING:
				GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
				break;
		}
	}
	if (TargetingComp)
		TargetingComp->UpdateRotationMode();
}

void ABaseCharacter::EquipWeapon(int32 ItemId)
{
	FActorSpawnParameters Params; 
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(this);
	
	FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/WeaponClass"));
	UDataTable* WeaponClassTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
	if(WeaponClassTableObject)
	{
		FWeaponClassTable* WeaponClassRow = nullptr;
		WeaponClassRow = WeaponClassTableObject->FindRow<FWeaponClassTable>(FName(FString::FromInt(ItemId)), TEXT(""));
		if(WeaponClassRow)
		{
			ABaseWeapon* Equipment = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClassRow->Weapon, GetActorTransform(), Params);

			if(CombatCompo && Equipment)
				CombatCompo->OnEquipWeapon(Equipment);
		}
	}
}

void ABaseCharacter::EquipArmor(int32 ItemId)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemId11 : %d"), ItemId);
	FActorSpawnParameters Params; 
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(this);

	FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/ArmorClass"));
	UDataTable* ArmorClassTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
	if(ArmorClassTableObject)
	{
		FArmorClassTable* ArmorClassRow = nullptr;
		ArmorClassRow = ArmorClassTableObject->FindRow<FArmorClassTable>(FName(FString::FromInt(ItemId)), TEXT(""));
		if(ArmorClassRow && CombatCompo)
		{
			ABaseArmor* Armor = GetWorld()->SpawnActor<ABaseArmor>(ArmorClassRow->Armor, GetActorTransform(), Params);
			if(Armor)
			{
				CombatCompo->OnEquipArmor(Armor);
			}
		}
	}
}

void ABaseCharacter::UnequipArmor(EItemCategory ArmorCategory)
{
	if(CombatCompo)
	{
		CombatCompo->OnUnequipArmor(ArmorCategory);
	}
}

void ABaseCharacter::EquippedWeaponSpawn(int32 ItemId)
{
	FActorSpawnParameters Params; 
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(this);
	
	FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/WeaponClass"));
	UDataTable* WeaponClassTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
	if(WeaponClassTableObject)
	{
		FWeaponClassTable* WeaponClassRow = nullptr;
		WeaponClassRow = WeaponClassTableObject->FindRow<FWeaponClassTable>(FName(FString::FromInt(ItemId)), TEXT(""));
		if(WeaponClassRow)
		{
			ABaseWeapon* Equipment = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClassRow->Weapon, GetActorTransform(), Params);

			if(CombatCompo && Equipment)
			{
				CombatCompo->EquippedWeaponSpawn(Equipment, ItemId);
			}
		}
	}
}

void ABaseCharacter::EquippedArmorApply(int32 ItemId)
{
	FActorSpawnParameters Params; 
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(this);
	
	FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/ArmorClass"));
	UDataTable* ArmorClassTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
	if(ArmorClassTableObject)
	{
		FArmorClassTable* ArmorClassRow = nullptr;
		ArmorClassRow = ArmorClassTableObject->FindRow<FArmorClassTable>(FName(FString::FromInt(ItemId)), TEXT(""));
		if(ArmorClassRow && CombatCompo)
		{
			ABaseArmor* Armor = GetWorld()->SpawnActor<ABaseArmor>(ArmorClassRow->Armor, GetActorTransform(), Params);
			if(Armor)
			{
				CombatCompo->OnEquippedArmorApply(Armor);
			}
		}
	}
}

void ABaseCharacter::ToggleLockOn()
{
	if(StateManagerComp && StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::NONE_COMBAT_STATE) return;
	if(TargetingComp) TargetingComp->ToggleLockOn();
}

bool ABaseCharacter::CanBeTargeted()
{
	if(StateManagerComp)
	{
		TArray<ECurrentState> State;
		State.Add(ECurrentState::DEAD);
		return !StateManagerComp->IsCurrentStateEqualToThis(State);
	}
	return false;
}

void ABaseCharacter::OnTargeted(bool IsTargeted)
{
	if(LockOnWidgetComp)
	{
		LockOnWidgetComp->SetVisibility(IsTargeted);
	}
}

void ABaseCharacter::ApplyHitReaction(EDamageType DamageType)
{
	switch (DamageType)
	{
	case EDamageType::MELEE_DAMAGE:
		PerformHitReact();
		break;
	case EDamageType::KNOCKDOWN_DAMAGE:
		PerformKnockdown();
		break;
	}
}

void ABaseCharacter::ApplyImpactEffect()
{
	if(ImpactSound && ImpactParticle)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation()); 
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorLocation());
	}
}

void ABaseCharacter::PerformHitReact()
{
	if(bHitFront)
	{
		if(HitReactFrontMontage)
			PlayAnimMontage(HitReactFrontMontage);
	}
	else
	{
		if(HitReactBackMontage)
			PlayAnimMontage(HitReactBackMontage);
	}
	
}

void ABaseCharacter::PerformKnockdown()
{
	if(bHitFront)
	{
		if(KnockdownFrontMontage)
			PlayAnimMontage(KnockdownFrontMontage);
	}
	else
	{
		if(KnockdownBackMontage)
			PlayAnimMontage(KnockdownBackMontage);
	}
}

void ABaseCharacter::CameraZoomInOut(float Rate)
{
	
}

bool ABaseCharacter::CanExecuteSkill()
{
	bool Condition = StateManagerComp && ( GetMovementComponent()->IsFalling() ||
		(StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::NONE_COMBAT_STATE) ||
		(StateManagerComp->GetCurrentState() == ECurrentState::ATTACKING));

	return !Condition;
}

void ABaseCharacter::HotkeySlot1ButtonPressed() //Q
{
	
	// if(Condition) return;
	// if(CombatCompo)
	// 	CombatCompo->Skill1();
}

void ABaseCharacter::HotkeySlot2ButtonPressed() //E
{
	// bool Condition = StateManagerComp && ( GetMovementComponent()->IsFalling() ||
	// 	(StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::NONE_COMBAT_STATE) ||
	// 	(StateManagerComp->GetCurrentState() == ECurrentState::ATTACKING));
	// if(Condition) return;
	// if(CombatCompo)
	// 	CombatCompo->Skill2();
}

void ABaseCharacter::HotkeySlot3ButtonPressed() //R
{
	// bool Condition = StateManagerComp && ( GetMovementComponent()->IsFalling() ||
	// 	(StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::NONE_COMBAT_STATE) ||
	// 	(StateManagerComp->GetCurrentState() == ECurrentState::ATTACKING));
	// if(Condition) return;
	// if(CombatCompo)											
	// 	CombatCompo->Skill3();								
}

void ABaseCharacter::HotkeySlot4ButtonPressed() //T 에 뭐가 등록되어있는지를 알아야함
{
	// bool Condition = StateManagerComp && ( GetMovementComponent()->IsFalling() ||
	// 	(StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::NONE_COMBAT_STATE) ||
	// 	(StateManagerComp->GetCurrentState() == ECurrentState::ATTACKING));
	// if(Condition) return;
	// if(CombatCompo)
	// 	CombatCompo->SkillUltimate();
}

EPhysicalSurface ABaseCharacter::GetPhysicsSurface()
{
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -400.f);
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		Params
	);
	
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}

bool ABaseCharacter::AddItem(int32 ItemId, int32 Amount, bool bFromMonster)
{
	if(ItemId == 0 || Amount == 0) return false;
	
	if(InventoryComp)
	{
		InventoryComp->AddItem(ItemId, Amount, bFromMonster);
	}

	return false;
}

void ABaseCharacter::AddGold(int32 GoldAmount, bool bFromMonster)
{
	if(InventoryComp)
	{
		InventoryComp->AddGold(GoldAmount, bFromMonster);
	}
}

void ABaseCharacter::LeftClickPressed()
{
	bLeftClicked = true;
}

void ABaseCharacter::LeftClickReleased()
{
	bLeftClicked = false;
}

void ABaseCharacter::ToggleInventory()
{
	if(InventoryComp)
	{
		if(InventoryComp->GetIsVisible())
		{
			InventoryComp->VisibleInventory(false);
		}
		else
		{
			InventoryComp->VisibleInventory(true);
		}
	}
}

void ABaseCharacter::AltPressed()
{
	bAltPressed = true;
}

void ABaseCharacter::AltReleased()
{
	bAltPressed = false;
}

int32 ABaseCharacter::DeathMob(int32 MId)
{
	int32 CollectQuest = 0;
	if(QuestLogComp)
	{
		CollectQuest = QuestLogComp->CheckCollectQuest(MId); //true면 퀘스트인거
	}
	return CollectQuest;
}

void ABaseCharacter::PlusCollectCurrentNum(int32 Amount)
{
	if(QuestLogComp)
	{
		QuestLogComp->PlusCurrentNum(Amount);
	}
}

void ABaseCharacter::AddExp(int32 Exp)
{
	if(StatComp && MainHUDWidget)
	{
		StatComp->PlusExp(Exp);
		MainHUDWidget->GetExp(Exp);
	}
}