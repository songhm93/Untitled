#include "DualWeapon.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

#include "../Type/Types.h"
#include "../PlayerCharacter/MeleeAnimInstance.h"



ADualWeapon::ADualWeapon()
{
    PrimaryActorTick.bCanEverTick = true;
    DualSwordStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DualSwordStaticMeshComp"));
    DualSwordStaticMeshComp->SetupAttachment(RootComponent);
    SetWeaponType(EWeaponType::DUAL_SWORD);
    GetItemStaticMeshComp()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DualSwordStaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
    SecondWeaponAttachSocket = TEXT("DualSword_L_Back");
    SecondWeaponHandSocket = TEXT("SecondWeaponSocket");
    SetAttachSocketName(TEXT("DualSword_R_Back"));
    SetHandSocketName(TEXT("WeaponSocket"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SecondSword(TEXT("/Game/CombatSystem/Resource/Meshes/Weapons/SM_ToughSword"));
    if(SecondSword.Succeeded())
        DualSwordStaticMeshComp->SetStaticMesh(SecondSword.Object);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sword(TEXT("/Game/CombatSystem/Resource/Meshes/Weapons/SM_ToughSword"));
    if(Sword.Succeeded())
        GetItemStaticMeshComp()->SetStaticMesh(Sword.Object);

    WeaponATK = 15.f;

    Skill1Cooldown = 5.f;
    Skill2Cooldown = 6.f;
    Skill3Cooldown = 22.f;
    SkillUltimateCooldown = 30.f;

    FirstUltimateSocket = TEXT("DSUltimateSocket_01");
    SecondUltimateSocket = TEXT("DSUltimateSocket_02");

    bPlayCurve = false;
    CurveTime = 1.8f;
    BlinkReturnCooldown = 5.f;
    bBlinkReturnTimerRunning = false;
    bReturnComplete = false;
}

void ADualWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(bPlayCurve)
    {
        SetPlayerHeight(DeltaTime);
    }

    if(bBlinkReturnTimerRunning)
    {
        float BlinkReturnRemaning = GetWorld()->GetTimerManager().GetTimerRemaining(BlinkReturnTimerHandle);
		if(BlinkReturnRemaning <= 0)
		{
			if(BlinkShadowParticleComponent)
                BlinkShadowParticleComponent->DestroyComponent();
            bBlinkReturnTimerRunning = false;
		}
    }

}

void ADualWeapon::BeginPlay() 
{
    Super::BeginPlay();

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        Cast<UMeleeAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance())->OnUltimateComplete.BindUObject(this, &ThisClass::UltimateComplete);
    }
}

void ADualWeapon::Skill1()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && Skill1Montage)
        {
            AnimInst->Montage_Play(Skill1Montage);
        }
    }
}

void ADualWeapon::Skill2()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && Skill2Montage)
        {
            AnimInst->Montage_Play(Skill2Montage);
        }
    }
}

void ADualWeapon::Skill3() //블링크
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

    bool SkillTimerRunning = GetSkillTimerRunning.Execute();
    
    if(bBlinkReturnTimerRunning && !bReturnComplete)
    {
        //원래자리로 이동
        GetOwner()->SetActorLocation(BlinkCurrentLocation);
        bReturnComplete = true;
        if(BlinkShadowParticleComponent)
                BlinkShadowParticleComponent->DestroyComponent();

        return;
    }
    if(SkillTimerRunning) return; //3번째 스킬 쿨다운상태인지. 블링크 리턴 쿨다운X

    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        BlinkCurrentLocation = GetOwner()->GetActorLocation();
        GetWorld()->GetTimerManager().SetTimer(BlinkReturnTimerHandle, BlinkReturnCooldown, false);
        bBlinkReturnTimerRunning = true;
        if(BlinkShadowParticle)
        BlinkShadowParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkShadowParticle, GetActorTransform());
        //앞으로 순간이동
        GetOwner()->SetActorLocation(BlinkCurrentLocation + GetOwner()->GetActorForwardVector() * 300.f);
        bReturnComplete = false;

        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && Skill3Montage)
        {
            AnimInst->Montage_Play(Skill3Montage);
        }
    }
}

void ADualWeapon::SkillUltimate()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && SkillUltimateMontage)
        {
            UltimateSocketChange(true);
            GetWorldTimerManager().SetTimer(HeightCurveTimerHandle, this, &ThisClass::FinishSetHeight, CurveTime);
            CurrentLocation = GetOwner()->GetActorLocation();
            bPlayCurve = true;
            AnimInst->Montage_Play(SkillUltimateMontage);
        }
    }
}

void ADualWeapon::SetPlayerHeight(float DeltaTime)
{
    if(PlayerHeightCurve)
    {
        FVector PlayerLocation = CurrentLocation;
        const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(HeightCurveTimerHandle); 
        const float HeightCurveValue = PlayerHeightCurve->GetFloatValue(ElapsedTime);
        PlayerLocation.Z += HeightCurveValue * 150;

        GetOwner()->SetActorLocation(PlayerLocation);

    }
}

void ADualWeapon::FinishSetHeight()
{
    bPlayCurve = false;
}

void ADualWeapon::UltimateSocketChange(bool Start)
{
    if(Start) //원래 소켓에서 궁 소켓으로 변경
    {
        FDetachmentTransformRules DeatachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
        DualSwordStaticMeshComp->DetachFromComponent(DeatachRules);
        DetachRootComponentFromParent();

        FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
        DualSwordStaticMeshComp->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), AttachRules, SecondUltimateSocket);
        AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), AttachRules, FirstUltimateSocket);
    }
    else //궁 소켓에서 원래 소켓으로 변경
    {
        FDetachmentTransformRules DeatachRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
        DualSwordStaticMeshComp->DetachFromComponent(DeatachRules);
        DetachRootComponentFromParent();

        FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
        DualSwordStaticMeshComp->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), AttachRules, SecondWeaponHandSocket);
        AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), AttachRules, GetHandSocketName());
    }
}

void ADualWeapon::UltimateComplete()
{
    UltimateSocketChange(false);
}

