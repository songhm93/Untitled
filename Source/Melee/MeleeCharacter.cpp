#include "MeleeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BaseWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interactable.h"
#include "MeleeAnimInstance.h"
#include "CombatComponent.h"

//////////////////////////////////////////////////////////////////////////
// AMeleeCharacter

AMeleeCharacter::AMeleeCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	TurnRateGamepad = 50.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComp"));

	bTogglingCombat = false;
	bDodging = false;
}

void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMeleeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("ToggleCombat", IE_Pressed, this, &ThisClass::ToggleCombat);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ThisClass::InteractButtonPressed);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ThisClass::LightAttack);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ThisClass::Dodge);
	

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMeleeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMeleeCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMeleeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMeleeCharacter::LookUpAtRate);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMeleeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMeleeCharacter::TouchStopped);
}

void AMeleeCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMeleeCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMeleeCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMeleeCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMeleeCharacter::ToggleCombat()
{
	if(!CanToggleCombat()) return;
	bTogglingCombat = true;
	if(CombatComp && CombatComp->GetEquippedWeapon())
	{
		if(CombatComp->GetEquippedWeapon()->GetEnterCombatAM() && CombatComp->GetEquippedWeapon()->GetExitCombatAM())
		{
			if(!CombatComp->GetCombatState()) //손에 장착 상태가 아니면
			{
				PlayAnimMontage(CombatComp->GetEquippedWeapon()->GetEnterCombatAM());
				CombatComp->SetCombatState(true);
				if(GetMesh() && GetMesh()->GetAnimInstance())
				{
					Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetCombatState(true);
				}
			}
			else
			{
				PlayAnimMontage(CombatComp->GetEquippedWeapon()->GetExitCombatAM());
				CombatComp->ResetAttackCount();
				CombatComp->SetCombatState(false);
				if(GetMesh() && GetMesh()->GetAnimInstance())
				{
					Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetCombatState(false);
				}
			}
		}
	}
	bTogglingCombat = false;
}

void AMeleeCharacter::InteractButtonPressed()
{
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 100.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Reserve(1);
	ObjectTypesArray.Emplace(ECollisionChannel::ECC_GameTraceChannel1);
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		this, 
		Start, 
		End, 
		30.f, 
		ObjectTypesArray, 
		false, 
		IgnoredActors, 
		EDrawDebugTrace::ForDuration, 
		OutHit, 
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		10.f);

	if(OutHit.GetActor())
	{
		IInteractable* Interactable = Cast<IInteractable>(OutHit.GetActor());
		if(Interactable)
		{
			Interactable->Interact(this);
		}
	}
}

void AMeleeCharacter::LightAttack() //todo:공격중에 회피하면 콤보가 초기화가 안됨.
{
	if(!CanAttack()) return;
	if(CombatComp && CombatComp->GetCombatState())
	{
		if(CombatComp->GetIsAttacking())
		{
			CombatComp->SetIsAttackSaved(true);
		}
		else
		{
			PerformAttack(CombatComp->GetAttackCount(), false);
		}
	}
}

void AMeleeCharacter::PerformAttack(int32 AttackIdx, bool bRandomIdx)
{
	if(!CanAttack()) return;
	int32 Idx = AttackIdx;
	if(CombatComp && CombatComp->GetEquippedWeapon())
	{
		TArray<UAnimMontage*> TempArray = CombatComp->GetEquippedWeapon()->GetAttackMontage();

		if(bRandomIdx)
		{
			int32 ArrayCount = TempArray.Num();
			if(ArrayCount == 0)
				Idx = 0;
			else
				Idx = FMath::RandRange(0, ArrayCount - 1);
		}

		CombatComp->SetIsAttacking(true);
		if(TempArray.Num() != Idx)
		{
			PlayAnimMontage(TempArray[Idx]);
		}
		else
		{
			CombatComp->ResetAttackCount();
			PlayAnimMontage(TempArray[CombatComp->GetAttackCount()]);
		}
		CombatComp->IncrementAttackCount();
	}
}

void AMeleeCharacter::ContinueAttack() //애님 노티파이로 호출될 함수. 공격중에 또 공격 버튼을 눌렀을 때 바로 다음 공격이 발생하는 것을 막기 위함.
{
	if(CombatComp)
	{
		CombatComp->SetIsAttacking(false);
		if(CombatComp->GetIsAttackSaved())
		{
			CombatComp->SetIsAttackSaved(false);
			PerformAttack(CombatComp->GetAttackCount(), false);
		}
	}
}

void AMeleeCharacter::ResetAttack() //애님 노티파이로 호출될 함수.
{
	if(CombatComp)
	{
		CombatComp->ResetAttackCount();
	}
}
void AMeleeCharacter::Dodge()
{
	if(!CanDodge()) return;
	PerformDodge(0, false);
}

void AMeleeCharacter::PerformDodge(int32 MontageIdx, bool bRandomIdx)
{
	bDodging = true;
	int32 Idx = MontageIdx;
	if(CombatComp && CombatComp->GetEquippedWeapon())
	{
		TArray<UAnimMontage*> TempArray = CombatComp->GetEquippedWeapon()->GetDodgeMontage();

		if(bRandomIdx)
		{
			int32 ArrayCount = TempArray.Num();
			if(ArrayCount == 0)
				Idx = 0;
			else
				Idx = FMath::RandRange(0, ArrayCount - 1);
		}

		//CombatComp->SetIsAttacking(true);
		if(TempArray.Num() != Idx)
		{
			PlayAnimMontage(TempArray[Idx]);
		}
		else
		{
			//CombatComp->ResetAttackCount();
			//PlayAnimMontage(TempArray[CombatComp->GetAttackCount()]);
		}
		//CombatComp->IncrementAttackCount();
	}
}

bool AMeleeCharacter::CanAttack()
{
	return (!bTogglingCombat && !bDodging);
}

bool AMeleeCharacter::CanToggleCombat()
{
	return (!CombatComp->GetIsAttacking() && !bDodging);
}

bool AMeleeCharacter::CanDodge()
{
	return (!CombatComp->GetIsAttacking() && !bTogglingCombat && !bDodging);
}

FRotator AMeleeCharacter::GetDesiredRotation()
{
	if(GetCharacterMovement())
	{
		FVector LastVector = GetCharacterMovement()->GetLastInputVector();
		bool VectorResult = UKismetMathLibrary::NotEqual_VectorVector(LastVector, FVector(0.f, 0.f, 0.f), 0.001f);
		if(VectorResult)
		{
			return UKismetMathLibrary::MakeRotFromX(GetLastMovementInputVector());
		}
	}
	
	
	return GetActorRotation();
}