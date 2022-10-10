#include "CombatComponent.h"
#include "MeleeCharacter.h"
#include "MeleeAnimInstance.h"
#include "BaseWeapon.h"
#include "BaseArmor.h"
#include "CollisionComponent.h"
#include "StatsComponent.h"
#include "Kismet/GameplayStatics.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquippedWeapon = nullptr;
	EquippedHelmet = nullptr;
	EquippedGauntlet = nullptr;
	EquippedChest = nullptr;
	EquippedBoot = nullptr;
	AttackCount = 0;
	PlayerATK = 0.f;
	CalcATK = 0.f;
	
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
				EquippedWeapon->Destroy();
			}
			EquippedWeapon = Cast<ABaseWeapon>(Equipment);

			if(EquippedWeapon)
			{
				AttachWeapon();
				
				if(Character->GetMesh() &&  Character->GetMesh()->GetAnimInstance())
				{
					Cast<UMeleeAnimInstance>(Character->GetMesh()->GetAnimInstance())->SetWeaponType(EquippedWeapon->GetWeaponType());
				}
				EquippedWeapon->GetCollisionComp()->SetCollisionMeshComponent(EquippedWeapon->GetItemMeshComp());
				//장착했으니 스탯 적용. 일단
				Character->GetStatComp()->PlusCurrentStatValue(EStats::ATK, EquippedWeapon->GetWeaponATK());
			}
		}
		else if(Equipment->GetEquipmentType() == EEquipmentType::ARMOR)
		{
			ABaseArmor* Armor = Cast<ABaseArmor>(Equipment); 
			if(Armor)
			{
				if(Armor->GetArmorType() == EArmorType::HELMET)
				{
					EquippedHelmet = Armor;
					Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedHelmet->GetArmorDEF());
					//스탯 적용. 대미지 입힐때랑 받을때 스탯 
				}	
				else if(Armor->GetArmorType() == EArmorType::GAUNTLET)
				{
					EquippedGauntlet = Armor;	
					Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedGauntlet->GetArmorDEF());
				}
				else if(Armor->GetArmorType() == EArmorType::CHEST)
				{
					EquippedChest = Armor;
					Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedChest->GetArmorDEF());
				}
				else if(Armor->GetArmorType() == EArmorType::BOOT)
				{
					EquippedBoot = Armor;
					Character->GetStatComp()->PlusCurrentStatValue(EStats::DEF, EquippedBoot->GetArmorDEF());
				}
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
		if(EquippedGauntlet)
		{
			EquippedGauntlet->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedGauntlet->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		if(EquippedChest)
		{
			EquippedChest->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedChest->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		if(EquippedBoot)
		{
			EquippedBoot->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
			EquippedBoot->GetItemSkeletalMeshComp()->SetMasterPoseComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh());
		}
		break;
	}
}

void UCombatComponent::AttachWeapon()
{
	if(GetCombatState())
		AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetHandSocketName());
    else
        AttachActor(EEquipmentType::WEAPON, EquippedWeapon->GetAttachSocketName());
        
} 

void UCombatComponent::HitCauseDamage(FHitResult& HitResult, ABaseWeapon* Weapon) //내 총 공격력을 계산해서 적용. 대미지 받는 쪽에서 추가 계산
{
	if(HitResult.GetActor() && GetOwner() && Weapon)
    {
        Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;
		Character = Character == nullptr ? Cast<AMeleeCharacter>(GetOwner()) : Character;

		if(Controller && Character && Character->GetStatComp())
		{
			HitFromDirection = GetOwner()->GetActorForwardVector();

			if(Cast<AMeleeCharacter>(HitResult.GetActor())->CanRecieveDamage())
			{
				PlayerATK = Character->GetStatComp()->GetCurrentStatValue(EStats::ATK);
				CalcATK = Cast<AMeleeCharacter>(GetOwner())->GetAttackActionCorrectionValue() * PlayerATK;
				UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), CalcATK, HitFromDirection, HitResult, Controller, Weapon, UDamageType::StaticClass());
				//일단 무기의 기본 공격력과 보정치로 계산.
			}
		}
    }
}