#include "RampageMonster.h"
#include "../Melee.h"
#include "Components/CapsuleComponent.h"

#include "EnemyAnimInstance.h"
#include "../Rock.h"

ARampageMonster::ARampageMonster()
{
    GetMesh()->SetCollisionObjectType(MonsterCapsule);
}

void ARampageMonster::BeginPlay()
{
    Super::BeginPlay();

    EnemyAnimInst = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
    if(EnemyAnimInst)
    {
        EnemyAnimInst->OnDeattachRock.BindUObject(this, &ThisClass::DeattachRock);
    }
}

void ARampageMonster::Special1()
{
    if(EnemyAnimInst && Special1Montage)
    {
        EnemyAnimInst->Montage_Play(Special1Montage);
    }
}

void ARampageMonster::Special2()
{
    FActorSpawnParameters Params; 
	Params.Owner = this;
	Params.Instigator = Cast<APawn>(this);
    Rock = GetWorld()->SpawnActor<ARock>(RockActor, GetActorTransform(), Params);

    FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
    if(Rock)
        Rock->AttachToComponent(GetMesh(), Rules, TEXT("RockAttachPoint")); //RootComponent를 붙임. 

    if(EnemyAnimInst && Special2Montage)
    {
        EnemyAnimInst->Montage_Play(Special2Montage);
    }
}

void ARampageMonster::Special3()
{
    if(EnemyAnimInst && Special3Montage)
    {
        EnemyAnimInst->Montage_Play(Special3Montage);
    }
}

void ARampageMonster::DeattachRock()
{
    if(Rock && Target)
    {
        FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		Rock->GetCapsuleComp()->DetachFromComponent(Rules);
        Rock->GetCapsuleComp()->SetCollisionProfileName(TEXT("RockIgnoreMonster"));
        
        Rock->GetCapsuleComp()->SetSimulatePhysics(true);
        Rock->GetCapsuleComp()->SetMassScale(NAME_None, 50.f);
        FVector ThrowVector = (GetActorForwardVector()) * 10'500.f;
        
        Rock->GetCapsuleComp()->AddImpulse(ThrowVector,NAME_None, true);
        Rock->GetCapsuleComp()->SetNotifyRigidBodyCollision(true);
     
    } 
		
}