#include "CombatComponent.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Character.h"
#include "../Equipment/DualWeapon.h"
#include "../Equipment/BaseArmor.h"
#include "../AttackDamageType.h"
#include "../Interface/CombatInterface.h"
#include "CollisionComponent.h"
#include "StateManagerComponent.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AttackCount = 0;
	HitFromDirection = FVector::ZeroVector;
	AttackActionCorrectionValue = 1.f;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner())
	{
		Controller = Cast<APawn>(GetOwner())->GetController();
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
				EquippedWeapon->OnHitResult.BindUObject(this, &ThisClass::HitCauseDamage);
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
	case EEquipmentType::ARMOR:
		if(EquippedHelmet)
		{
			EquippedHelmet->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedHelmet->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<ACharacter>(GetOwner())->GetMesh());
		}
		else if(EquippedGauntlet)
		{
			EquippedGauntlet->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedGauntlet->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<ACharacter>(GetOwner())->GetMesh());
		}
		else if(EquippedChest)
		{
			EquippedChest->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedChest->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<ACharacter>(GetOwner())->GetMesh());
		}
		else if(EquippedBoot)
		{
			EquippedBoot->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedBoot->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<ACharacter>(GetOwner())->GetMesh());
		}
		break;
	}
}

void UCombatComponent::AttachWeapon()
{
	const ECurrentCombatState CurrentCombatState = GetCurrentCombatState.Execute();
	if(CurrentCombatState == ECurrentCombatState::COMBAT_STATE && EquippedWeapon)
	{
		if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
		{
			AttachSecondWeapon(Cast<ADualWeapon>(EquippedWeapon)->GetSecondWeaponHandSocket());
			AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetHandSocketName());
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

void UCombatComponent::AttachSecondWeapon(FName SocketName)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	Cast<ADualWeapon>(EquippedWeapon)->GetDualSwordStaticMeshComp()->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
}

void UCombatComponent::HitCauseDamage(FHitResult& HitResult) //내 총 공격력을 계산해서 적용. 대미지 받는 쪽에서 추가 계산
{
	if(HitResult.GetActor() && GetOwner() && EquippedWeapon)
    {
        Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;

		if(Controller)
		{
			HitFromDirection = GetOwner()->GetActorForwardVector();
			
			if(Cast<ICombatInterface>(HitResult.GetActor())->CanRecieveDamage())
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
	
	EquippedWeapon->GetCollisionComp()->SetCollisionMeshComponent(EquippedWeapon->GetItemMeshComp());
	if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
	{
		ADualWeapon* DualWeapon = Cast<ADualWeapon>(EquippedWeapon);
		if(DualWeapon && Character)
		{
			DualWeapon->GetSecondWeaponCollisionComp()->SetCollisionMeshComponent(DualWeapon->GetDualSwordStaticMeshComp());
			DualWeapon->GetRightFootCollisionComp()->SetCollisionMeshComponent(Character->GetMesh());
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