#include "CombatComponent.h"
#include "../MeleeCharacter.h"
#include "../MeleeAnimInstance.h"
#include "../BaseWeapon.h"
#include "../DualWeapon.h"
#include "../BaseArmor.h"
#include "CollisionComponent.h"
#include "StatsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../AttackDamageType.h"
#include "../Interface/CombatInterface.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AttackCount = 0;
	
	HitFromDirection = FVector::ZeroVector;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner())
	{
		Controller = Cast<APawn>(GetOwner())->GetController();
		Character = Cast<AMeleeCharacter>(GetOwner());
	}
		
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::OnEquipped(ABaseEquippable* Equipment)
{
	Character = Character == nullptr ? Cast<AMeleeCharacter>(GetOwner()) : Character;
	if(Equipment && Character && Character->GetStatComp())
	{
		if(Equipment->GetEquipmentType() == EEquipmentType::WEAPON)
		{
			if(EquippedWeapon)
			{
				Character->GetStatComp()->PlusCurrentStatValue(EStats::ATK, -EquippedWeapon->GetWeaponATK());
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
			EquippedWeapon->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
		break;
	case EEquipmentType::ARMOR:
		if(EquippedHelmet)
		{
			EquippedHelmet->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedHelmet->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		else if(EquippedGauntlet)
		{
			EquippedGauntlet->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedGauntlet->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		else if(EquippedChest)
		{
			EquippedChest->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedChest->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		else if(EquippedBoot)
		{
			EquippedBoot->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedBoot->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		break;
	}
}

void UCombatComponent::AttachWeapon()
{
	if(GetCombatState() && EquippedWeapon)
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
    else
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
	Cast<ADualWeapon>(EquippedWeapon)->GetDualSwordStaticMeshComp()->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
}

void UCombatComponent::HitCauseDamage(FHitResult& HitResult) //내 총 공격력을 계산해서 적용. 대미지 받는 쪽에서 추가 계산
{
	if(HitResult.GetActor() && GetOwner() && EquippedWeapon)
    {
        Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;
		Character = Character == nullptr ? Cast<AMeleeCharacter>(GetOwner()) : Character;

		if(Controller && Character && Character->GetStatComp())
		{
			HitFromDirection = GetOwner()->GetActorForwardVector();
			
			if(Cast<ICombatInterface>(HitResult.GetActor())->CanRecieveDamage())
			{
				const float PlayerATK = Character->GetStatComp()->GetCurrentStatValue(EStats::ATK);
				const float CalcATK = Cast<AMeleeCharacter>(GetOwner())->GetAttackActionCorrectionValue() * PlayerATK;
				UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), CalcATK, HitFromDirection, HitResult, Controller, EquippedWeapon, AttackDamageType->StaticClass());
				//일단 무기의 기본 공격력과 보정치로 계산.
			}
		}
    }
}

void UCombatComponent::WeaponBaseSetting()
{
	if(Character->GetMesh() &&  Character->GetMesh()->GetAnimInstance())
	{
		Cast<UMeleeAnimInstance>(Character->GetMesh()->GetAnimInstance())->SetWeaponType(EquippedWeapon->GetWeaponType());
	}
	EquippedWeapon->GetCollisionComp()->SetCollisionMeshComponent(EquippedWeapon->GetItemMeshComp());
	if(EquippedWeapon->GetWeaponType() == EWeaponType::DUAL_SWORD)
	{
		ADualWeapon* DualWeapon = Cast<ADualWeapon>(EquippedWeapon);
		if(DualWeapon)
		{
			DualWeapon->GetSecondWeaponCollisionComp()->SetCollisionMeshComponent(DualWeapon->GetDualSwordStaticMeshComp());
			DualWeapon->GetRightFootCollisionComp()->SetCollisionMeshComponent(Character->GetMesh());
		}
	}
	Character->GetStatComp()->PlusCurrentStatValue(EStats::ATK, EquippedWeapon->GetWeaponATK());
}

void UCombatComponent::ArmorBaseSetting(ABaseArmor* Armor)
{
	if(Armor)
	{
		if(Armor->GetArmorType() == EArmorType::HELMET)
		{
			if(EquippedHelmet)
			{
				Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, -EquippedHelmet->GetArmorDEF());
				EquippedHelmet->Destroy();
			}
			EquippedHelmet = Armor;
			Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedHelmet->GetArmorDEF());
		}	
		else if(Armor->GetArmorType() == EArmorType::GAUNTLET)
		{
			if(EquippedGauntlet)
			{
				Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, -EquippedGauntlet->GetArmorDEF());
				EquippedGauntlet->Destroy();
			}
			EquippedGauntlet = Armor;	
			Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedGauntlet->GetArmorDEF());
		}
		else if(Armor->GetArmorType() == EArmorType::CHEST)
		{
			if(EquippedChest)
			{
				Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, -EquippedChest->GetArmorDEF());
				EquippedChest->Destroy();
			}
			EquippedChest = Armor;
			Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedChest->GetArmorDEF());
		}
		else if(Armor->GetArmorType() == EArmorType::BOOT)
		{
			if(EquippedBoot)
			{
				Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, -EquippedBoot->GetArmorDEF());
				EquippedBoot->Destroy();
			}
			EquippedBoot = Armor;
			Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedBoot->GetArmorDEF());
		}
	}
}