#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"

#include "../Equipment/DualWeapon.h"
#include "../Equipment/BaseArmor.h"
#include "../AttackDamageType.h"
#include "../Interface/CombatInterface.h"
#include "../Interface/TargetingInterface.h"
#include "../PlayerController/MeleePlayerController.h"
#include "../PlayerCharacter/MeleeAnimInstance.h"
#include "../PlayerCharacter/BaseCharacter.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	AttackCount = 0;
	HitFromDirection = FVector::ZeroVector;
	AttackActionCorrectionValue = 1.f;
	bFirstSkillTimerRunning = false;
	bSecondSkillTimerRunning = false;
	bThirdSkillTimerRunning = false;
	bUltimateSkillTimerRunning = false;
	bHoldWeapon = false;
	HoldTime = 0.f;

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner())
	{
		Controller = Cast<APawn>(GetOwner())->GetController();
		if(Cast<AMeleePlayerController>(Controller))
		{
			Cast<AMeleePlayerController>(Controller)->OnLightAttack.BindUObject(this, &ThisClass::LightAttack);
		}
		if(Cast<ACharacter>(GetOwner())->GetMesh())
		{
			AnimInst = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
			if(Cast<UMeleeAnimInstance>(AnimInst))
			{
				Cast<UMeleeAnimInstance>(AnimInst)->OnImpact.BindUObject(this, &ThisClass::ImpactTrace);
				Cast<UMeleeAnimInstance>(AnimInst)->OnUltimateImpact.BindUObject(this, &ThisClass::UltimateImpact);
			}
		}
		BaseCharacter = Cast<ABaseCharacter>(GetOwner());
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bFirstSkillTimerRunning)
	{
		float Skill1Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(FirstSkillTimerHandle);
		if(Skill1Remaning <= 0.f)
		{
			bFirstSkillTimerRunning = false;
		}
	}

	if(bSecondSkillTimerRunning)
	{
		float Skill2Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(SecondSkillTimerHandle);
		if(Skill2Remaning <= 0.f)
		{
			bSecondSkillTimerRunning = false;
		}
	}

	if(bThirdSkillTimerRunning)
	{
		float Skill3Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(ThirdSkillTimerHandle);
		if(Skill3Remaning <= 0.f)
		{
			bThirdSkillTimerRunning = false;
		}
	}

	if(bUltimateSkillTimerRunning)
	{
		float SkillUltimateRemaning = GetWorld()->GetTimerManager().GetTimerRemaining(UltimateSkillTimerHandle);
		if(SkillUltimateRemaning <= 0.f)
		{
			bUltimateSkillTimerRunning = false;
		}
	}

	if(bHoldWeapon)
	{
		if(BaseCharacter)
		{
			if(!BaseCharacter->GetIsHoldWeapon())
			{
				BaseCharacter->SetIsHoldWeapon(true); //무기 들게
			}

			HoldTime += DeltaTime;
			if(HoldTime >= 8.f)
			{
				bHoldWeapon = false;
				HoldTime = 0.f;
				//무기 집어넣게
				BaseCharacter->SetIsHoldWeapon(false);
			}

		}
		
	}
}

float UCombatComponent::GetSkillCurrentCooldownTime(int32 SkillNum)
{
	switch (SkillNum)
	{
	case 1:
		if(bFirstSkillTimerRunning)
			return GetWorld()->GetTimerManager().GetTimerRemaining(FirstSkillTimerHandle);
		else
			return 0.f;
	case 2:
		if(bSecondSkillTimerRunning)
			return GetWorld()->GetTimerManager().GetTimerRemaining(SecondSkillTimerHandle);
		else
			return 0.f;
	case 3:
		if(bThirdSkillTimerRunning)
			return GetWorld()->GetTimerManager().GetTimerRemaining(ThirdSkillTimerHandle);
		else
			return 0.f;
	case 4:
		if(bUltimateSkillTimerRunning)
			return GetWorld()->GetTimerManager().GetTimerRemaining(UltimateSkillTimerHandle);
		else
			return 0.f;
	}
	return 0.f;
}

float UCombatComponent::GetWeaponSkillCooldownTime(int32 SkillNum)
{
	if(EquippedWeapon)
	{
		switch (SkillNum)
		{
		case 1:
			return EquippedWeapon->GetSkill1Cooldown();
		case 2:
			return EquippedWeapon->GetSkill2Cooldown();
		case 3:
			return EquippedWeapon->GetSkill3Cooldown();
		case 4:
			return EquippedWeapon->GetSkillUltimateCooldown();
		}
	}
	return 0.f;
}

void UCombatComponent::OnEquipWeapon(ABaseWeapon* Equipment)
{
	if(Equipment)
	{
		if(EquippedWeapon)
		{
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::ATK, -EquippedWeapon->GetWeaponATK());
			EquippedWeapon->Destroy();
		}
		EquippedWeapon = Equipment;

		if(EquippedWeapon)
		{
			AttachWeapon();
			WeaponBaseSetting();
		}
	}
}

void UCombatComponent::EquippedWeaponSpawn(ABaseWeapon* Equipment, int32 WeaponId)
{
	if(Equipment)
	{
		EquippedWeapon = Cast<ABaseWeapon>(Equipment);

		if(EquippedWeapon)
		{
			AttachWeapon();
			OnUpdateWeaponType.ExecuteIfBound(EquippedWeapon->GetWeaponType());
	
			ADualWeapon* DualWeapon = Cast<ADualWeapon>(EquippedWeapon);
			if(DualWeapon)
			{
				DualWeapon->SetSkillATK.BindUObject(this, &ThisClass::ApplySkillExplodeDamage);
			}
		}
	}
}

void UCombatComponent::OnEquipArmor(ABaseArmor* Equipment)
{
	if(Equipment)
	{
		ArmorBaseSetting(Equipment);
	}
}

void UCombatComponent::OnUnequipArmor(EItemCategory ArmorCategory)
{
	switch (ArmorCategory)
	{
	case EItemCategory::HELMET:
		if(EquippedHelmet)
		{
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedHelmet->GetArmorDEF());
			EquippedHelmet->Destroy();
		}
		EquippedHelmet = nullptr;
		break;
	case EItemCategory::CHEST:
		if(EquippedChest)
		{
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedChest->GetArmorDEF());
			EquippedChest->Destroy();
		}
		EquippedChest = nullptr;
		break;
	case EItemCategory::GAUNTLET:
		if(EquippedGauntlet)
		{
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedGauntlet->GetArmorDEF());
			EquippedGauntlet->Destroy();
		}
		EquippedGauntlet = nullptr;
		break;
	case EItemCategory::BOOT:
		if(EquippedBoot)
		{
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedBoot->GetArmorDEF());
			EquippedBoot->Destroy();
		}
		EquippedBoot = nullptr;
		break;
	
	}
}

void UCombatComponent::OnEquippedArmorApply(ABaseArmor* Armor)
{
	if(Armor)
	{
		if(Armor->GetArmorType() == EArmorType::HELMET)
		{
			EquippedHelmet = Armor;
		}	
		else if(Armor->GetArmorType() == EArmorType::GAUNTLET)
		{
			EquippedGauntlet = Armor;
		}
		else if(Armor->GetArmorType() == EArmorType::CHEST)
		{
			EquippedChest = Armor;
		}
		else if(Armor->GetArmorType() == EArmorType::BOOT)
		{
			EquippedBoot = Armor;
		}
	}
}


void UCombatComponent::AttachActor(EEquipmentType Type, FName SocketName)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	switch (Type)
	{
	case EEquipmentType::WEAPON:
		if(EquippedWeapon)
			EquippedWeapon->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
		break;
	}
}

void UCombatComponent::AttachSecondWeapon(FName SocketName)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	Cast<ADualWeapon>(EquippedWeapon)->GetDualSwordStaticMeshComp()->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
}

void UCombatComponent::AttachWeapon()
{
	if(!GetCurrentCombatState.IsBound()) return;
	const ECurrentCombatState CurrentCombatState = GetCurrentCombatState.Execute();
	if(CurrentCombatState == ECurrentCombatState::COMBAT_STATE && EquippedWeapon)
	{
		if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
		{
			AttachSecondWeapon(Cast<ADualWeapon>(EquippedWeapon)->GetSecondWeaponHandSocket());
			AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetHandSocketName());
			Cast<ADualWeapon>(EquippedWeapon)->GetSkillTimerRunning.BindUObject(this, &ThisClass::GetThirdSkillTimerRunning);
		}
		else
		{
			AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetHandSocketName());
		}
	}
    else if(CurrentCombatState == ECurrentCombatState::NONE_COMBAT_STATE && EquippedWeapon)
	{
		if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
		{
			AttachSecondWeapon(Cast<ADualWeapon>(EquippedWeapon)->GetSecondWeaponAttachSocket());
			AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetAttachSocketName());
		}
		else
		{
			AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetAttachSocketName());
		}
	}
} 

void UCombatComponent::HitCauseDamage(FHitResult& HitResult) //내 총 공격력을 계산해서 적용. 대미지 받는 쪽에서 추가 계산
{
	if(HitResult.GetActor() && GetOwner() && EquippedWeapon)
    {
        Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;

		if(Controller)
		{
			HitFromDirection = GetOwner()->GetActorForwardVector();
			
			if(HitResult.GetActor()->Implements<UCombatInterface>() && Cast<ICombatInterface>(HitResult.GetActor())->CanReceiveDamage())
			{
				if (GetCurrentStatValue.IsBound())
				{
					const float PlayerATK = GetCurrentStatValue.Execute(EStats::ATK);
					const float CalcATK = AttackActionCorrectionValue * PlayerATK;

					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), CalcATK, HitFromDirection, HitResult, Controller, EquippedWeapon, AttackDamageType->StaticClass());
				}
			}
		}
    }
}

void UCombatComponent::WeaponBaseSetting()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	
	OnUpdateWeaponType.ExecuteIfBound(EquippedWeapon->GetWeaponType());

	if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
	{
		ADualWeapon* DualWeapon = Cast<ADualWeapon>(EquippedWeapon);
		if(DualWeapon)
		{
			DualWeapon->SetSkillATK.BindUObject(this, &ThisClass::ApplySkillExplodeDamage);
		}
	}
	OnUpdateCurrentStatValue.ExecuteIfBound(EStats::ATK, EquippedWeapon->GetWeaponATK());
}

void UCombatComponent::ArmorBaseSetting(ABaseArmor* Armor)
{
	if(Armor)
	{
		if(Armor->GetArmorType() == EArmorType::HELMET)
		{
			if(EquippedHelmet)
			{
				OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedHelmet->GetArmorDEF());
				EquippedHelmet->Destroy();
			}
			EquippedHelmet = Armor;
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, EquippedHelmet->GetArmorDEF());
		}	
		else if(Armor->GetArmorType() == EArmorType::GAUNTLET)
		{
			if(EquippedGauntlet)
			{
				OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedGauntlet->GetArmorDEF());
				EquippedGauntlet->Destroy();
			}
			EquippedGauntlet = Armor;	
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, EquippedGauntlet->GetArmorDEF());
		}
		else if(Armor->GetArmorType() == EArmorType::CHEST)
		{
			if(EquippedChest)
			{
				OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedChest->GetArmorDEF());
				EquippedChest->Destroy();
			}
			EquippedChest = Armor;
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, EquippedChest->GetArmorDEF());
		}
		else if(Armor->GetArmorType() == EArmorType::BOOT)
		{
			if(EquippedBoot)
			{
				OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, -EquippedBoot->GetArmorDEF());
				EquippedBoot->Destroy();
			}
			EquippedBoot = Armor;
			OnUpdateCurrentStatValue.ExecuteIfBound(EStats::DEF, EquippedBoot->GetArmorDEF());
		}
	}
}

void UCombatComponent::LightAttack()
{
	if(!CanAttack() || !GetCurrentState.IsBound()) return;
	const ECurrentState CurrentState = GetCurrentState.Execute();
	if(CurrentState == ECurrentState::NOTHING)
	{
		if(CurrentState == ECurrentState::ATTACKING)
		{
			bIsAttackSaved = true;
		}
		else
		{
			Attack(AttackCount);
		}
	}
}

void UCombatComponent::Attack(int32 Count)
{
	bHoldWeapon = true;
	HoldTime = 0.f;
	
	EMovementType MovementType = GetCurrentMovementType.Execute();
	UAnimMontage* TempLightAttackMontage = nullptr;
	UAnimMontage* TempSprintAttackMontage = nullptr;
	FName SectionName = TEXT("");

	if(AnimInst && EquippedWeapon)
	{
		SectionName = GetLightAttackSectionName(Count);

		if(EquippedWeapon->GetWeaponType() == EWeaponType::LIGHT_SWORD)
		{
			TempLightAttackMontage = LSLightAttackMontage;
			TempSprintAttackMontage = LSSprintAttackMontage;
		}
		else if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
		{
			TempLightAttackMontage = DSLightAttackMontage;
			TempSprintAttackMontage = DSSprintAttackMontage;
		}
		
		if(TempLightAttackMontage && MovementType != EMovementType::SPRINTING)
		{
			AnimInst->Montage_Play(TempLightAttackMontage);
			AnimInst->Montage_JumpToSection(SectionName, TempLightAttackMontage);
			IncrementAttackCount();
		}
		else if(TempSprintAttackMontage && MovementType == EMovementType::SPRINTING)
		{
			AnimInst->Montage_Play(TempSprintAttackMontage);
		}
		
		OnUpdateCurrentState.ExecuteIfBound(ECurrentState::ATTACKING);
		OnUpdateCurrentAction.ExecuteIfBound(ECurrentAction::LIGHT_ATTACK);
	}
}


void UCombatComponent::ContinueAttack()
{
	OnUpdateCurrentState.ExecuteIfBound(ECurrentState::NOTHING);

	if(bIsAttackSaved)
	{
		bIsAttackSaved = false;
		Attack(AttackCount);
	}
}

bool UCombatComponent::CanAttack()
{
	if(!GetCurrentCombatState.IsBound() || !IsCurrentStateEqualToThis.IsBound()) return false;
	const ECurrentCombatState CurrentCombatState = GetCurrentCombatState.Execute();
	bool Condition =  
		(!EquippedWeapon || CurrentCombatState == ECurrentCombatState::NONE_COMBAT_STATE);
	if(!EquippedWeapon) return false;
	
	TArray<ECurrentState> CharacterStates;
	CharacterStates.Add(ECurrentState::ATTACKING);
	CharacterStates.Add(ECurrentState::DODGING);
	CharacterStates.Add(ECurrentState::DEAD);
	CharacterStates.Add(ECurrentState::DISABLED);
	CharacterStates.Add(ECurrentState::GENERAL_STATE);
	CharacterStates.Add(ECurrentState::STUN);
	bool ReturnValue = false;

	const bool EqualTo = IsCurrentStateEqualToThis.Execute(CharacterStates);
	if(Cast<ACharacter>(GetOwner())->GetCharacterMovement())
		ReturnValue = (!EqualTo) && (!Cast<ACharacter>(GetOwner())->GetCharacterMovement()->IsFalling());
	return ReturnValue;
}

FName UCombatComponent::GetLightAttackSectionName(int32 Count)
{
	if (Count == 0)
	{
		AttackActionCorrectionValue = 1.f;
		return TEXT("First");
	}
	else if (Count == 1)
	{
		AttackActionCorrectionValue = 2.f;
		return TEXT("Second");
	}
	else if (Count == 2)
	{
		AttackActionCorrectionValue = 3.f;
		return TEXT("Third");
	}
	else
	{
		ResetAttackCount();
		AttackActionCorrectionValue = 1.f;
		return TEXT("First");
	}
	
	return NAME_None;
	
}

void UCombatComponent::PerformDodge()
{
	if(DodgeMontage && GetOwner())
	{
		bHoldWeapon = true;
		HoldTime = 0.f;
		OnUpdateCurrentState.ExecuteIfBound(ECurrentState::DODGING);
		OnUpdateCurrentAction.ExecuteIfBound(ECurrentAction::DODGE);
	
		Cast<ACharacter>(GetOwner())->PlayAnimMontage(DodgeMontage);
		
		
		
	}
}

void UCombatComponent::ImpactTrace(int32 SkillNum)
{
	const FVector Start = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 50.f;
	const FVector End = Start + GetOwner()->GetActorForwardVector() * 100.f;
	const FVector HalfSize = FVector(50.f, 50.f, 100.f);
	const FRotator Oritentation = GetOwner()->GetActorRotation();


	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectType;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	CollisionObjectType.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	TArray<FHitResult> OutHitResult;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		this,
		Start,
		End,
		HalfSize,
		Oritentation,
		CollisionObjectType,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.f
	);

	ClearHitActors();

	if(!OutHitResult.IsEmpty())
	{
		for (auto LastHit : OutHitResult)
		{
			if (!AlreadyHitActors.Contains(LastHit.GetActor())) //중복이 아니면
			{
				HitFromDirection = GetOwner()->GetActorForwardVector();
				AlreadyHitActors.Add(LastHit.GetActor());
				if(SkillNum == 0)
					HitCauseDamage(LastHit);
				else
					HitCauseSkillDamage(LastHit, SkillNum);
				ApplyImpact(LastHit.GetActor());
			}
		}
	}
}

void UCombatComponent::ClearHitActors()
{
	AlreadyHitActors.Empty();
}

void UCombatComponent::ApplyImpact(AActor* HitActor)
{
	if(HitActor)
	{
		if(BasicAttackImpactSound)
			UGameplayStatics::PlaySoundAtLocation(this, BasicAttackImpactSound, HitActor->GetActorLocation());
		if(BasicAttackImpactParticle)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BasicAttackImpactParticle, HitActor->GetActorTransform());
	}
}

void UCombatComponent::Skill1()
{
	if(bFirstSkillTimerRunning) return;
	

	if(EquippedWeapon)
	{
		bHoldWeapon = true;
		HoldTime = 0.f;
		EquippedWeapon->Skill1();

		FirstSkillSlotCooldown = EquippedWeapon->GetSkill1Cooldown();
		GetWorld()->GetTimerManager().SetTimer(FirstSkillTimerHandle, FirstSkillSlotCooldown, false);
		bFirstSkillTimerRunning = true;
	}
}

void UCombatComponent::Skill2()
{
	if(bSecondSkillTimerRunning) return;
	

	if(EquippedWeapon)
	{
		bHoldWeapon = true;
		HoldTime = 0.f;
		EquippedWeapon->Skill2();

		SecondSkillSlotCooldown = EquippedWeapon->GetSkill2Cooldown();
		GetWorld()->GetTimerManager().SetTimer(SecondSkillTimerHandle, SecondSkillSlotCooldown, false);
		bSecondSkillTimerRunning = true;
	}
}

void UCombatComponent::Skill3()
{
	if(bThirdSkillTimerRunning)
	{
		if(!Cast<ADualWeapon>(EquippedWeapon)) return; //DualWeapon만 해당
	}

	if(Cast<ADualWeapon>(EquippedWeapon)->GetbBlinkReturnTimerRunning()) //DualWeapon만 해당
	{
		EquippedWeapon->Skill3(); //재사용
		return;
	}

	if(EquippedWeapon)
	{
		bHoldWeapon = true;
		HoldTime = 0.f;
		EquippedWeapon->Skill3();

		if(bThirdSkillTimerRunning) return; //위는 블링크 재사용때
		ThirdSkillSlotCooldown = EquippedWeapon->GetSkill3Cooldown();
		GetWorld()->GetTimerManager().SetTimer(ThirdSkillTimerHandle, ThirdSkillSlotCooldown, false);
		bThirdSkillTimerRunning = true;
	}
}

void UCombatComponent::SkillUltimate()
{
	if(bUltimateSkillTimerRunning)
	{
		//float SkillUltimateRemaning = GetWorld()->GetTimerManager().GetTimerRemaining(UltimateSkillTimerHandle);
		
		return;
	}
	if(EquippedWeapon)
	{
		bHoldWeapon = true;
		HoldTime = 0.f;
		EquippedWeapon->SkillUltimate();
		UltimateSkillSlotCooldown = EquippedWeapon->GetSkillUltimateCooldown();
		GetWorld()->GetTimerManager().SetTimer(UltimateSkillTimerHandle, UltimateSkillSlotCooldown, false);
		bUltimateSkillTimerRunning = true;
	}
}

void UCombatComponent::ApplySkillExplodeDamage(float SkillATK, FHitResult HitResult)
{
	if(HitResult.bBlockingHit)
	{
		TArray<AActor*> Ignore;
		Ignore.Add(GetOwner());
	
		UGameplayStatics::ApplyRadialDamage(this, SkillATK, HitResult.GetActor()->GetActorLocation(), 256.f, UDamageType::StaticClass(), Ignore, EquippedWeapon, Cast<APawn>(GetOwner())->GetController());
	}
}

void UCombatComponent::UltimateImpact()
{
	const FVector Start = GetOwner()->GetActorLocation() + (GetOwner()->GetActorForwardVector() * 50.f) + (GetOwner()->GetActorUpVector() * -150.f);
	const FVector End = Start + GetOwner()->GetActorForwardVector() * 100.f;
	const FVector HalfSize = FVector(50.f, 50.f, 50.f);
	const FRotator Oritentation = GetOwner()->GetActorRotation();


	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectType;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	CollisionObjectType.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	TArray<FHitResult> OutHitResult;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		this,
		Start,
		End,
		HalfSize,
		Oritentation,
		CollisionObjectType,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.f
	);

	ClearHitActors();

	if(!OutHitResult.IsEmpty())
	{
		for (auto LastHit : OutHitResult)
		{
			if (!AlreadyHitActors.Contains(LastHit.GetActor())) //중복이 아니면
			{
				HitFromDirection = GetOwner()->GetActorForwardVector();
				AlreadyHitActors.Add(LastHit.GetActor());

				HitCauseSkillDamage(LastHit, 4);
				ApplyImpact(LastHit.GetActor());
			}
		}
	}
}

void UCombatComponent::HitCauseSkillDamage(FHitResult& HitResult, int32 SkillNum)
{
	if(HitResult.GetActor() && GetOwner() && EquippedWeapon)
    {
        Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;

		if(Controller)
		{
			HitFromDirection = GetOwner()->GetActorForwardVector();
			
			if(HitResult.GetActor()->Implements<UCombatInterface>() && Cast<ICombatInterface>(HitResult.GetActor())->CanReceiveDamage())
			{
				if (GetCurrentStatValue.IsBound())
				{
					const float PlayerATK = GetCurrentStatValue.Execute(EStats::ATK);
					const float WeaponSkillATK = EquippedWeapon->GetWeaponSkillATK(SkillNum);
					const float SkillLevel = EquippedWeapon->SkillATKCalc(SkillNum);
					const float CalcATK = PlayerATK + WeaponSkillATK * SkillLevel;
					
					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), CalcATK, HitFromDirection, HitResult, Controller, EquippedWeapon, AttackDamageType->StaticClass());

					if(SkillNum == 4 && DefaultCameraShakeClass)
					{
						Cast<APlayerController>(GetOwner()->GetInstigatorController())->ClientStartCameraShake(DefaultCameraShakeClass);
					}
				}
			}
		}
    }
}
