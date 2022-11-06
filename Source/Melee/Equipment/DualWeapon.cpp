#include "DualWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Blueprint/UserWidget.h"

#include "../Type/Types.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "../PlayerCharacter/MeleeAnimInstance.h"
#include "../Component/StateManagerComponent.h"
#include "../FlameSkill.h"



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

    Skill1ExpodeTime = 1.3f;

    Skill1ATK = 10.f;
    Skill2ATK = 3.5f;
    Skill3ATK = 0.f;

    BlinkShowCharacterTime = 1.5f; //1.5초 후 캐릭터, 무기 보이게.
    BlinkMoveTime = 1.f; //1초 후 순간이동
}

void ADualWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(bPlayCurve)
    {
        SetPlayerHeight();
    }

    if(bBlinkReturnTimerRunning)
    {
        float BlinkReturnRemaning = GetWorld()->GetTimerManager().GetTimerRemaining(BlinkReturnTimerHandle);
		if(BlinkReturnRemaning <= 0)
		{
			BlinkDestroyShadow();
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
    Super::Skill1();

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        if(Cast<ABaseCharacter>(OwnerCharacter) && Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp())
            Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp()->SetCurrentState(ECurrentState::ATTACKING);
        
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && Skill1Montage)
        {
            AnimInst->Montage_Play(Skill1Montage);
        }
        GetWorld()->GetTimerManager().SetTimer(Skill1ExpodeTimerHandle, this, &ThisClass::Skill1Explode, Skill1ExpodeTime);
    }
}

void ADualWeapon::Skill2()
{
    Super::Skill2();
    
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        if(Cast<ABaseCharacter>(OwnerCharacter) && Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp())
            Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp()->SetCurrentState(ECurrentState::ATTACKING);
        
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && Skill2Montage)
        {
            AnimInst->Montage_Play(Skill2Montage);
        }
        Skill2Range();
    }
}

void ADualWeapon::Skill3() //블링크
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

    bool SkillTimerRunning = GetSkillTimerRunning.Execute();
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        
        if(bBlinkReturnTimerRunning && !bReturnComplete)
        {
            if(AnimInst->IsAnyMontagePlaying()) return;
            
            //블링크 두번째 시전. 원래자리로 이동
            if(BlinkReturnParticle)
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkReturnParticle, GetActorTransform());

            GetOwner()->SetActorLocation(BlinkCurrentLocation);
            bReturnComplete = true;
            BlinkDestroyShadow();

            return;
        }
        if(SkillTimerRunning) return; //3번째 스킬 쿨다운상태인지. 블링크 리턴 쿨다운X

        //블링크 첫 사전.
        if(Cast<ABaseCharacter>(OwnerCharacter) && Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp())
            Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp()->SetCurrentState(ECurrentState::ATTACKING);
        
        BlinkCurrentLocation = GetOwner()->GetActorLocation();
        GetWorld()->GetTimerManager().SetTimer(BlinkReturnTimerHandle, BlinkReturnCooldown, false);
        bBlinkReturnTimerRunning = true;

        if(BlinkShadowParticle)
            BlinkShadowParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkShadowParticle, GetActorTransform());
        if(BlinkSupplementalParticle)
            BlinkSupplementalParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkSupplementalParticle, GetActorTransform());
        if(BlinkBeginParticle)
            BlinkBeginParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkBeginParticle, GetActorTransform());
        ShadowTransform = GetActorTransform();
            
        {   //캐릭터, 무기 Hide 후 일정시간 후 나타나기 위한 타이머
            ShowHideCharacter(false); //Hide
            bool Show = true;
            BlinkShowCharacterDelegate.BindUFunction(this, FName("ShowHideCharacter"), Show);
            GetWorld()->GetTimerManager().SetTimer(BlinkShowCharacterTimerHandle, BlinkShowCharacterDelegate, BlinkShowCharacterTime, false);
        }
        //앞으로 바로 순간이동 하지 않고 일정시간 후 순간이동
        GetWorld()->GetTimerManager().SetTimer(BlinkMoveTimerHandle, this, &ThisClass::BlinkMove, BlinkMoveTime);
    }
}

void ADualWeapon::SkillUltimate()
{
    Super::SkillUltimate();

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        if(Cast<ABaseCharacter>(OwnerCharacter) && Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp())
            Cast<ABaseCharacter>(OwnerCharacter)->GetStateManagerComp()->SetCurrentState(ECurrentState::ATTACKING);
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && SkillUltimateMontage)
        {
            UltimateSocketChange(true);
            GetWorld()->GetTimerManager().SetTimer(HeightCurveTimerHandle, this, &ThisClass::FinishSetHeight, CurveTime);
            CurrentLocation = GetOwner()->GetActorLocation();
            bPlayCurve = true;
            AnimInst->Montage_Play(SkillUltimateMontage);
        }
    }
}

void ADualWeapon::SetPlayerHeight()
{
    if(PlayerHeightCurve)
    {
        FVector PlayerLocation = CurrentLocation;
        const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(HeightCurveTimerHandle); 
        const float HeightCurveValue = PlayerHeightCurve->GetFloatValue(ElapsedTime);
        PlayerLocation.Z += HeightCurveValue * 150;
        if(GetOwner())
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

void ADualWeapon::Skill1Explode()
{
    const FVector Start = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 150.f;
	const FVector End = Start + GetOwner()->GetActorForwardVector() * 100.f;
	const FVector HalfSize = FVector(150.f, 150.f, 100.f);
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
		EDrawDebugTrace::ForDuration,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.f
	);
	
	FHitResult LastHit;
	if(!OutHitResult.IsEmpty())
	{
		for(auto HitResult : OutHitResult)
		{
			LastHit = HitResult;
		}
	}
    
    SetSkillATK.ExecuteIfBound(Skill1ATK, LastHit);

    if(Skill1ExplodeParticle && LastHit.GetActor())
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Skill1ExplodeParticle, LastHit.GetActor()->GetActorTransform());
    }
    else if(Skill1ExplodeParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Skill1ExplodeParticle, GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 150.f);
    }
}

void ADualWeapon::Skill2Range()
{
    //Spawn
    const FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 300.f;
    const FVector SpawnScale = FVector(30.f, 30.f, 30.f);
    FTransform SpawnTransform = FTransform(GetOwner()->GetActorRotation(), SpawnLocation, GetOwner()->GetActorScale3D());
    FActorSpawnParameters Params;
    Params.Owner = GetOwner();
    Params.Instigator = Cast<APawn>(GetOwner());

    GetWorld()->SpawnActor<AFlameSkill>(FlameActor, SpawnTransform, Params);

    FHitResult HitResult;
    GetWorld()->LineTraceSingleByChannel(
        HitResult,
        SpawnLocation,
        SpawnLocation + FVector(0.f, 0.f, -300.f),
        ECollisionChannel::ECC_Visibility
    );

    FVector ImpactLocation;
    if(HitResult.bBlockingHit)
    {
        ImpactLocation = HitResult.ImpactPoint;
        SpawnTransform = FTransform(GetOwner()->GetActorRotation(), ImpactLocation, GetOwner()->GetActorScale3D());
    }

    if(Skill2FlameParticle)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Skill2FlameParticle, SpawnTransform);
}

void ADualWeapon::ShowHideCharacter(bool Hide)
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetMesh())
        OwnerCharacter->GetMesh()->SetVisibility(Hide);
    if(DualSwordStaticMeshComp)
        DualSwordStaticMeshComp->SetVisibility(Hide);
    if(GetItemStaticMeshComp())
        GetItemStaticMeshComp()->SetVisibility(Hide);
}

void ADualWeapon::BlinkMove()
{
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    bReturnComplete = false;
    //앞으로 순간이동
    if(OwnerCharacter && OwnerCharacter->GetMesh())
    {
        OwnerCharacter->SetActorLocation(BlinkCurrentLocation + GetOwner()->GetActorForwardVector() * 500.f);
        UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
        if(AnimInst && Skill3Montage)
            AnimInst->Montage_Play(Skill3Montage);
    }
}

void ADualWeapon::BlinkDestroyShadow()
{
    if(BlinkShadowParticleComponent)
        BlinkShadowParticleComponent->DestroyComponent();
    if(BlinkSupplementalParticleComponent)
        BlinkSupplementalParticleComponent->DestroyComponent();
    if(BlinkBeginParticleComponent)
        BlinkBeginParticleComponent->DestroyComponent();
    if(BlinkShadowBurstParticle)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlinkShadowBurstParticle, ShadowTransform);
    bBlinkReturnTimerRunning = false;
}