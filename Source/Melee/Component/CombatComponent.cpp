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


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	AttackCount = 0;
	HitFromDirection = FVector::ZeroVector;
	AttackActionCorrectionValue = 1.f;
	DodgeStaminaCost = 10.f;
	bFirstSkillTimerRunning = false;
	bSecondSkillTimerRunning = false;
	bThirdSkillTimerRunning = false;
	bUltimateSkillTimerRunning = false;
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
			Cast<AMeleePlayerController>(Controller)->OnChargedAttack.BindUObject(this, &ThisClass::ChargedAttack); //Attack함수를 어떻게 나눌까
		}
		if(Cast<ACharacter>(GetOwner())->GetMesh())
		{
			AnimInst = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
			if(Cast<UMeleeAnimInstance>(AnimInst))
			{
				Cast<UMeleeAnimInstance>(AnimInst)->OnImpact.BindUObject(this, &ThisClass::ImpactTrace);
			}
		}

	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bFirstSkillTimerRunning)
	{
		float Skill1Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(FirstSkillTimerHandle);
		if(Skill1Remaning <= 0)
		{
			bFirstSkillTimerRunning = false;
		}
	}

	if(bSecondSkillTimerRunning)
	{
		float Skill2Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(SecondSkillTimerHandle);
		if(Skill2Remaning <= 0)
		{
			bSecondSkillTimerRunning = false;
		}
	}

	if(bThirdSkillTimerRunning)
	{
		float Skill3Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(ThirdSkillTimerHandle);
		if(Skill3Remaning <= 0)
		{
			bThirdSkillTimerRunning = false;
		}
	}

	if(bUltimateSkillTimerRunning)
	{
		float SkillUltimateRemaning = GetWorld()->GetTimerManager().GetTimerRemaining(UltimateSkillTimerHandle);
		if(SkillUltimateRemaning <= 0)
		{
			bUltimateSkillTimerRunning = false;
		}
	}

}

void UCombatComponent::OnEquipped(ABaseEquippable* Equipment)
{
	if(Equipment)
	{
		if(Equipment->GetEquipmentType() == EEquipmentType::WEAPON)
		{
			if(EquippedWeapon)
			{
				OnUpdateCurrentStatValue.ExecuteIfBound(EStats::ATK, -EquippedWeapon->GetWeaponATK());
				EquippedWeapon->Destroy();
			}
			EquippedWeapon = Cast<ABaseWeapon>(Equipment);

			if(EquippedWeapon)
			{
				AttachWeapon();
				WeaponBaseSetting();
			}
		}
		else if(Equipment->GetEquipmentType() == EEquipmentType::ARMOR)
		{
			ABaseArmor* Armor = Cast<ABaseArmor>(Equipment); 
			if(Armor)
			{
				ArmorBaseSetting(Armor);
			}
			AttachActor(EEquipmentType::ARMOR, TEXT(""));
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
			
			if(HitResult.GetActor()->Implements<UCombatInterface>() &&  Cast<ICombatInterface>(HitResult.GetActor())->CanRecieveDamage())
			{
				if (GetCurrentStatValue.IsBound())
				{
					const float PlayerATK = GetCurrentStatValue.Execute(EStats::ATK);
					const float CalcATK = AttackActionCorrectionValue * PlayerATK;

					UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), CalcATK, HitFromDirection, HitResult, Controller, EquippedWeapon, AttackDamageType->StaticClass());
					//일단 무기의 기본 공격력과 보정치로 계산.
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
			SetIsAttackSaved(true);
		}
		else
		{
			Attack(AttackCount);
		}
	}
}

void UCombatComponent::HeavyAttack()
{
	if(!CanAttack() || !GetCurrentState.IsBound()) return;
	const ECurrentState CurrentState = GetCurrentState.Execute();
	if(CurrentState == ECurrentState::NOTHING)
	{
		if(CurrentState == ECurrentState::ATTACKING)
		{
			SetIsAttackSaved(true);
		}
		else
		{
			SubAttack(ECurrentAction::HEAVY_ATTACK);
		}
	}
}

void UCombatComponent::ChargedAttack()
{
	if(!CanAttack() || !GetCurrentState.IsBound()) return;
	const ECurrentState CurrentState = GetCurrentState.Execute();
	if(CurrentState == ECurrentState::NOTHING)
	{
		if(CurrentState == ECurrentState::ATTACKING)
		{
			SetIsAttackSaved(true);
		}
		else
		{
			SubAttack(ECurrentAction::CHARGED_ATTACK);
		}
	}
}

void UCombatComponent::Attack(int32 Count)
{
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

void UCombatComponent::SubAttack(ECurrentAction Action)
{
	UAnimMontage* TempMontage = nullptr;
	if(EquippedWeapon)
	{
		EWeaponType WeaponType = EquippedWeapon->GetWeaponType();
		if(WeaponType == EWeaponType::LIGHT_SWORD)
		{
			switch(Action)
			{
				case ECurrentAction::HEAVY_ATTACK:
					TempMontage = LSHeavyAttackMontage;
					AttackActionCorrectionValue = 3.f;
				break;
				case ECurrentAction::CHARGED_ATTACK:
					TempMontage = LSChargedAttackMontage;
					AttackActionCorrectionValue = 4.f;
				break;
			}
		}
		else if(WeaponType == EWeaponType::DUAL_SWORD)
		{
			switch(Action)
			{
				case ECurrentAction::HEAVY_ATTACK:
					TempMontage = DSHeavyAttackMontage;
					AttackActionCorrectionValue = 3.f;
				break;
				case ECurrentAction::CHARGED_ATTACK:
					TempMontage = DSChargedAttackMontage;
					AttackActionCorrectionValue = 4.f;
				break;
			}
		}
		OnUpdateCurrentState.ExecuteIfBound(ECurrentState::ATTACKING);
		OnUpdateCurrentAction.ExecuteIfBound(Action);
		
		Cast<ACharacter>(GetOwner())->PlayAnimMontage(TempMontage);
	}
}


void UCombatComponent::ContinueAttack()
{
	OnUpdateCurrentState.ExecuteIfBound(ECurrentState::NOTHING);

	if(GetIsAttackSaved())
	{
		SetIsAttackSaved(false);
		Attack(AttackCount);
	}
	
}

bool UCombatComponent::CanAttack()
{
	if(!GetCurrentCombatState.IsBound() || !IsCurrentStateEqualToThis.IsBound()) return false;
	const ECurrentCombatState CurrentCombatState = GetCurrentCombatState.Execute();
	bool Condition =  
		(!EquippedWeapon || CurrentCombatState == ECurrentCombatState::NONE_COMBAT_STATE);
	if(Condition) return false;
	
	TArray<ECurrentState> CharacterStates;
	CharacterStates.Add(ECurrentState::ATTACKING);
	CharacterStates.Add(ECurrentState::DODGING);
	CharacterStates.Add(ECurrentState::DEAD);
	CharacterStates.Add(ECurrentState::DISABLED);
	CharacterStates.Add(ECurrentState::GENERAL_STATE);
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
		OnUpdateCurrentState.ExecuteIfBound(ECurrentState::DODGING);
		OnUpdateCurrentAction.ExecuteIfBound(ECurrentAction::DODGE);
	
		Cast<ACharacter>(GetOwner())->PlayAnimMontage(DodgeMontage);
		OnUpdateCurrentStatValue.ExecuteIfBound(EStats::STAMINA, -(DodgeStaminaCost));
		
	}
}

void UCombatComponent::ImpactTrace()
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
	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::ForDuration;
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
		EDrawDebugTrace::ForDuration,
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

				HitCauseDamage(LastHit);
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
	if(bFirstSkillTimerRunning)
	{
		float Skill1Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(FirstSkillTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("스킬 1 쿨타임 도는중 남은 시간 :%f"), Skill1Remaning);
		return;
	}
	if(EquippedWeapon)
	{
		EquippedWeapon->Skill1();
		//범위랑 대미지는 여기서 해야겠지?
		FirstSkillSlotCooldown = EquippedWeapon->GetSkill1Cooldown();
		GetWorld()->GetTimerManager().SetTimer(FirstSkillTimerHandle, FirstSkillSlotCooldown, false);
		bFirstSkillTimerRunning = true;
	}
}

void UCombatComponent::Skill2()
{
	if(bSecondSkillTimerRunning)
	{
		float Skill2Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(SecondSkillTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("스킬 2 쿨타임 도는중 남은 시간 :%f"), Skill2Remaning);
		return;
	}
	if(EquippedWeapon)
	{
		EquippedWeapon->Skill2();
		//범위랑 대미지는 여기서 해야겠지?
		SecondSkillSlotCooldown = EquippedWeapon->GetSkill2Cooldown();
		GetWorld()->GetTimerManager().SetTimer(SecondSkillTimerHandle, SecondSkillSlotCooldown, false);
		bSecondSkillTimerRunning = true;
	}
}

void UCombatComponent::Skill3()
{
	if(bThirdSkillTimerRunning)
	{
		float Skill3Remaning = GetWorld()->GetTimerManager().GetTimerRemaining(ThirdSkillTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("스킬 3 쿨타임 도는중 남은 시간 :%f"), Skill3Remaning);
		
		if(!Cast<ADualWeapon>(EquippedWeapon)) return;
		
		
	}
	if(Cast<ADualWeapon>(EquippedWeapon)->GetbBlinkReturnTimerRunning())
	{
		EquippedWeapon->Skill3(); //재사용
		return;
	}
	if(EquippedWeapon)
	{
		EquippedWeapon->Skill3();
		//범위랑 대미지는 여기서 해야겠지?
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
		float SkillUltimateRemaning = GetWorld()->GetTimerManager().GetTimerRemaining(UltimateSkillTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("궁극기 쿨타임 도는중 남은 시간 :%f"), SkillUltimateRemaning);
		return;
	}
	if(EquippedWeapon)
	{
		EquippedWeapon->SkillUltimate();
		UltimateSkillSlotCooldown = EquippedWeapon->GetSkillUltimateCooldown();
		GetWorld()->GetTimerManager().SetTimer(UltimateSkillTimerHandle, UltimateSkillSlotCooldown, false);
		bUltimateSkillTimerRunning = true;
	}
}