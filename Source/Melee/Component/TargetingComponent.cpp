#include "TargetingComponent.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Type/RotationMode.h"
#include "../Interface/TargetingInterface.h"
#include "CombatComponent.h"
#include "StateManagerComponent.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsTargeting = false;
	TargetingDist = 900.f;
	TargetRotationInterpSpeed = 9.f;
	CurrentRotationMode = ERotationMode::ORIENT_TO_MOVEMENT;
}

void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if(OwnerCharacter)
	{
		OwnerController = OwnerCharacter->GetController();
		if(OwnerController)
		{
			FollowCamera = Cast<UCameraComponent>(OwnerCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
		}
	}

	CombatComp = Cast<UCombatComponent>(OwnerCharacter->GetComponentByClass(UCombatComponent::StaticClass()));
	StateManagerComp = Cast<UStateManagerComponent>(OwnerCharacter->GetComponentByClass(UStateManagerComponent::StaticClass()));
}

void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsTargeting)
		UpdateTargetingControlRotation(DeltaTime);

}

void UTargetingComponent::SetTargeting(AActor* Target)
{
	if(Target)
	{
		TargetActor = Target;
		SetIsTargeting(true);
		UpdateRotationMode();
	}
}

void UTargetingComponent::EnableLockOn(FHitResult HitResult)
{
	if(FindTarget(HitResult))
	{
		TargetActor = HitResult.GetActor();
		SetIsTargeting(true);
		UpdateRotationMode();
	}
	else
	{
		SetIsTargeting(false);
		TargetActor = nullptr;
		UpdateRotationMode();
	}
}

void UTargetingComponent::DisableLockOn()
{
	SetIsTargeting(false);
	UpdateRotationMode();
	TargetActor = nullptr;
}

void UTargetingComponent::ToggleLockOn(FHitResult HitResult)
{
	if(TargetActor && TargetActor == HitResult.GetActor())
	{
		if(bIsTargeting)
		{
			DisableLockOn();
			return;
		}
	}
	if(bIsTargeting)
	{
		DisableLockOn();
	}
	EnableLockOn(HitResult);
}

bool UTargetingComponent::FindTarget(FHitResult Hit)
{
	//const UObject* WorldContextObject, const FVector Start, const FVector End, float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime
	if(FollowCamera && GetOwner())
	{
		if(Hit.bBlockingHit)
		{
			if(Hit.GetActor()->Implements<UTargetingInterface>())
				return true;
		}
	}

	return false;
}

bool UTargetingComponent::IsMonster(AActor* Target)
{
	if(Target && Target->Implements<UTargetingInterface>())
	{
		return true;	
	}
	return false;
}

void UTargetingComponent::UpdateTargetingControlRotation(float DeltaTime)
{
	if(OwnerController && TargetActor && GetOwner())
	{
		if(!Cast<ITargetingInterface>(TargetActor)->CanBeTargeted() || !CanKeepDist(TargetActor))
		{
			DisableLockOn();
			return;
		}
		
		const FVector PlayerActorLocation = GetOwner()->GetActorLocation();
		const FVector TargetActorLocation = TargetActor->GetActorLocation() - FVector(0.f, 0.f, 100.f);
		

		const FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(PlayerActorLocation, TargetActorLocation);

		// const FRotator PlayerControlRotator = OwnerController->GetControlRotation();

		// const FRotator ResultRotator = FMath::RInterpTo(PlayerControlRotator, LookAtRotator, DeltaTime, TargetRotationInterpSpeed);
		
		//OwnerController->SetControlRotation(FRotator(ResultRotator.Pitch, ResultRotator.Yaw, PlayerControlRotator.Roll));
	}
}

bool UTargetingComponent::CanKeepDist(AActor* Target)
{
	if(Target && GetOwner())
	{
		const float Dist = GetOwner()->GetDistanceTo(Target);
		if(Dist < TargetingDist) 
			return true;
	}
	return false;
}

void UTargetingComponent::SetRotationMode(ERotationMode RotationMode)
{
	if(OwnerCharacter && OwnerCharacter->GetCharacterMovement())
	{
		CurrentRotationMode = RotationMode;
		switch(CurrentRotationMode)
		{
			case ERotationMode::ORIENT_TO_CAMERA:
				OwnerCharacter->bUseControllerRotationYaw = false;
				OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
				OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
				break;
			case ERotationMode::ORIENT_TO_MOVEMENT:
				OwnerCharacter->bUseControllerRotationYaw = false;
				OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
				OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
				break;
		}
	}
}

void UTargetingComponent::SetIsTargeting(bool IsTargeting)
{
	bIsTargeting = IsTargeting;
	if(TargetActor)
	{
		Cast<ITargetingInterface>(TargetActor)->OnTargeted(bIsTargeting);
	}
}

void UTargetingComponent::UpdateRotationMode()
{
	if(OwnerCharacter && CombatComp && StateManagerComp)
	{
		
		if (StateManagerComp->GetCurrentCombatState() == ECurrentCombatState::COMBAT_STATE && StateManagerComp->GetMovementType() != EMovementType::SPRINTING && bIsTargeting)
		{
			SetRotationMode(ERotationMode::ORIENT_TO_CAMERA);
		}
		else
		{
			SetRotationMode(ERotationMode::ORIENT_TO_MOVEMENT);
		}
	}
}