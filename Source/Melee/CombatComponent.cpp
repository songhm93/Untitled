#include "CombatComponent.h"
#include "MeleeCharacter.h"
#include "MeleeAnimInstance.h"
#include "BaseWeapon.h"
#include "CollisionComponent.h"
#include "Kismet/GameplayStatics.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquippedWeapon = nullptr;
	AttackCount = 0;
	
	HitFromDirection = FVector::ZeroVector;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner())
		Controller = Cast<APawn>(GetOwner())->GetController();
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::OnEquipped(ABaseWeapon* Weapon)
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = Weapon;
	
	AMeleeCharacter* Character =  Cast<AMeleeCharacter>(GetOwner());
	if(EquippedWeapon && Character)
	{
		AttachWeapon(Character);
		
		if(Character->GetMesh() &&  Character->GetMesh()->GetAnimInstance())
		{
			Cast<UMeleeAnimInstance>(Character->GetMesh()->GetAnimInstance())->SetWeaponType(EquippedWeapon->GetWeaponType());
		}
		EquippedWeapon->GetCollisionComp()->SetCollisionMeshComponent(EquippedWeapon->GetItemMesh());
		
	}
}

void UCombatComponent::AttachActor(FName SocketName)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	if(EquippedWeapon)
		EquippedWeapon->AttachToComponent(Cast<AMeleeCharacter>(GetOwner())->GetMesh(), Rules, SocketName);
}

void UCombatComponent::AttachWeapon(AMeleeCharacter* Character)
{
	if(GetCombatState())
		AttachActor(EquippedWeapon->GetHandSocketName());
    else
        AttachActor(EquippedWeapon->GetAttachSocketName());
        
} 

void UCombatComponent::HitCauseDamage(FHitResult& HitResult, ABaseWeapon* Weapon) //내 총 공격력을 계산해서 적용. 대미지 받는 쪽에서 추가 계산
{
	if(HitResult.GetActor() && GetOwner() && Weapon)
    {
        Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;
        HitFromDirection = GetOwner()->GetActorForwardVector();

        if(Cast<AMeleeCharacter>(HitResult.GetActor())->CanRecieveDamage())
        {
            float CalcATK = Cast<AMeleeCharacter>(GetOwner())->GetAttackActionCorrectionValue() * Weapon->GetATK(); //일단 액션 보정치, 무기 공격력만 적용
            UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), CalcATK, HitFromDirection, HitResult, Controller, Weapon, UDamageType::StaticClass());
            //일단 무기의 기본 공격력와 보정치로 계산.
        }
    }
}