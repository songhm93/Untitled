#include "RampageMonster.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "EnemyAnimInstance.h"
#include "../Component/MonsterStatsComponent.h"
#include "../SkillActor/Rock.h"
#include "../Melee.h"

ARampageMonster::ARampageMonster()
{
    GetMesh()->SetCollisionObjectType(MonsterCapsule);
    
    SkillRangeDestroyTime = 1.0f;

    bPauseTimeTrack = false;

    S3PlayTime = 0.f;

    bPauseS3Montage = false;

    S3RandSkillCount = 0;

}

void ARampageMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(bPauseTimeTrack)
    {
        S3PlayTime += DeltaTime;
        if(S3PlayTime >= 0.5f)
        {
            PauseS3Montage();
        }
    }
    else
    {
        S3PlayTime = 0.f;
    }
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
    
    if(EnemyAnimInst && Special3Montage && !bPauseS3Montage)
    {
        S3RandSkillCount = FMath::RandRange(3, 5);
        EnemyAnimInst->Montage_Play(Special3Montage);
        bPauseTimeTrack = true; //0.5초 후 pause
    }

    if(CurrentSkillCount == S3RandSkillCount)
    {
        //SkillComplete
        ResumeS3Montage();
    }

    FActorSpawnParameters Params; 
	Params.Owner = this;
    Params.Instigator = Cast<APawn>(this);
	
    if(Target && SkillRangeActor)
    {
        FVector SpawnLocation = FindFloor();
        FTransform SpawnTransform = FTransform(GetActorTransform().GetRotation(), SpawnLocation - FVector(0.f, 0.f, 2.f), Target->GetActorScale3D());
        AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(SkillRangeActor, SpawnTransform , Params);
        
        SkillRangeDestroyDeletage.BindUFunction(this, FName("SkillRangeDestroy"), SpawnActor);
        GetWorldTimerManager().SetTimer(SkillRangeDestroyTimerHandle, SkillRangeDestroyDeletage, SkillRangeDestroyTime, false);
        ++CurrentSkillCount;
    }
}

void ARampageMonster::Special4()
{
    FActorSpawnParameters Params; 
	Params.Owner = this;
    Params.Instigator = Cast<APawn>(this);

    if(EnemyAnimInst && Special4Montage)
    {
        EnemyAnimInst->Montage_Play(Special4Montage);
    }

    if(Target && SkillRangeActor)
    {
        for(int i = 0; i < 10; ++i)
        {
            FVector SpawnLocation = FindRandomFloor();
            FTransform SpawnTransform = FTransform(GetActorTransform().GetRotation(), SpawnLocation - FVector(0.f, 0.f, 2.f), Target->GetActorScale3D());
            AActor* SpawnActor = GetWorld()->SpawnActor<AActor>(SkillRangeActor, SpawnTransform , Params);
            
        }
    }
}

void ARampageMonster::Special4Explode()
{

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

FVector ARampageMonster::FindFloor()
{
    if(Target)
    {
        FVector TargetLocation = Target->GetActorLocation();
        FVector FindLoc = TargetLocation + (Target->GetActorForwardVector() * 5.f);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, FindLoc, FindLoc + (Target->GetActorUpVector() * -600.f), ECollisionChannel::ECC_Visibility);
        if(HitResult.bBlockingHit)
        {
            return HitResult.ImpactPoint;
        }
        //DrawDebugLine(GetWorld(), FindLoc, FindLoc + (Target->GetActorUpVector() * -600.f), FColor::Cyan, true, -1, 0, 12.333);
    }

    return FVector::ZeroVector;
}

FVector ARampageMonster::FindRandomFloor()
{
    if(Target)
    {
        float RandValueX = FMath::RandRange(-600.f, 600.f);
        float RandValueY = FMath::RandRange(-600.f, 600.f);
        FVector TargetLocation = GetActorLocation() + FVector(RandValueX, RandValueY, 0.f);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, TargetLocation, TargetLocation + (GetActorUpVector() * -600.f), ECollisionChannel::ECC_Visibility);
        if(HitResult.bBlockingHit)
        {
            return HitResult.ImpactPoint;
        }
        //DrawDebugLine(GetWorld(), FindLoc, FindLoc + (Target->GetActorUpVector() * -600.f), FColor::Cyan, true, -1, 0, 12.333);
    }

    return FVector::ZeroVector;
}

void ARampageMonster::SkillRangeDestroy(AActor* RangeActor)
{
    //파괴하면서 대미지
    UGameplayStatics::ApplyRadialDamage(this, 2000.f, RangeActor->GetActorLocation() + FVector(0.f, 0.f, 20.f), 256.f, UDamageType::StaticClass(), TArray<AActor*>(), this, GetController());
    RangeActor->Destroy();
}

void ARampageMonster::PauseS3Montage()
{
    if(EnemyAnimInst && Special3Montage && EnemyAnimInst->IsAnyMontagePlaying())
    {
        EnemyAnimInst->Montage_Pause(Special3Montage);
        bPauseS3Montage = true; //S3가 아예 끝나면 false로
        bPauseTimeTrack = false;
    }
}

void ARampageMonster::ResumeS3Montage()
{
    if(EnemyAnimInst && Special3Montage)
    {
        EnemyAnimInst->Montage_Resume(Special3Montage);
        bPauseS3Montage = false;
        CurrentSkillCount = 0;
    }
}