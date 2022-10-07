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
#include "Types.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "StateManagerComponent.h"

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
	StateManagerComp = CreateDefaultSubobject<UStateManagerComponent>(TEXT("StateManagerComp"));

	// bTogglingCombat = false;
	// bDodging = false;
	// bIsDisabled = false;
	// bIsDead = false;
	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::NOTHING);
	HP = 100.f;
	
	PelvisBoneName = TEXT("pelvis");
	DestroyDeadTime = 4.f;

	// static ConstructorHelpers::FObjectFinder<UAnimMontage> DodgeAM(TEXT("/Game/CombatSystem/AnimMontage/AM_Dodge"));
    // if(DodgeAM.Succeeded())
    //     DodgeMontage.Add(DodgeAM.Object);

	// static ConstructorHelpers::FObjectFinder<UAnimMontage> LightAttackAM3(TEXT("/Game/CombatSystem/AnimMontage/AM_LightAttack03"));
    // if(LightAttackAM3.Succeeded())
    //     LightAttackMontage.Add(LightAttackAM3.Object);

	// static ConstructorHelpers::FObjectFinder<UAnimMontage> LightAttackAM2(TEXT("/Game/CombatSystem/AnimMontage/AM_LightAttack02"));
    // if(LightAttackAM2.Succeeded())
    //     LightAttackMontage.Add(LightAttackAM2.Object);
	
	// static ConstructorHelpers::FObjectFinder<UAnimMontage> LightAttackAM1(TEXT("/Game/CombatSystem/AnimMontage/AM_LightAttack01"));
    // if(LightAttackAM1.Succeeded())
    // 	LightAttackMontage.Add(LightAttackAM1.Object);
    
	FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/ResourceTable"));
	UDataTable* TableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
	if(TableObject)
	{
		FResourceTable* Row = TableObject->FindRow<FResourceTable>(FName("Resource"), TEXT(""));
		if(Row)
		{
			for(auto Montage : Row->LightAttackMontage)
			{
				LightAttackMontage.Add(Montage);
			}
			for(auto Montage : Row->GreatAttackMontage)
			{
				GreatAttackMontage.Add(Montage);
			}
			DodgeMontage = Row->DodgeMontage;
			ImpactSound = Row->ImpactSound;
			ImpactParticle = Row->ImpactParticle;
			HitReactMontage = Row->HitReactMontage;
		}
	}
    ResetCombat();
}

void AMeleeCharacter::BeginPlay()
{
	Super::BeginPlay();
	ResetCombat();
	OnTakePointDamage.AddDynamic(this, &AMeleeCharacter::ReceiveDamage);
	if(StateManagerComp)
	{
		StateManagerComp->OnStateBegin.AddDynamic(this, &ThisClass::CharacterStateBegin);
		StateManagerComp->OnStateEnd.AddDynamic(this, &ThisClass::CharacterStateEnd);
	}
}

void AMeleeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("ToggleCombat", IE_Pressed, this, &ThisClass::ToggleCombat);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ThisClass::InteractButtonPressed);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ThisClass::LightAttack);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ThisClass::Dodge);
	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ThisClass::Test);
	

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMeleeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMeleeCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMeleeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMeleeCharacter::LookUpAtRate);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMeleeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMeleeCharacter::TouchStopped);
}

void AMeleeCharacter::Jump()
{
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::ATTACKING);
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DISABLED);
	
	if(StateManagerComp && StateManagerComp->IsCurrentStateEqualToThis(CharacterStates)) return;
	//if(bIsDisabled || bDodging || (CombatComp && CombatComp->GetIsAttacking())) return;
	Super::Jump();
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
	//bTogglingCombat = true;
	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::GENERAL_ACTION_STATE);
	
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

void AMeleeCharacter::LightAttack()
{
	if(!CanAttack()) return;
	if(CombatComp && CombatComp->GetCombatState() && StateManagerComp)
	{
		if(StateManagerComp->GetCurrentState() == ECharacterState::ATTACKING)
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
		TArray<UAnimMontage*> TempArray;
		ECombatType WeaponType = CombatComp->GetEquippedWeapon()->GetCombatType();
		switch (WeaponType)
		{
		case ECombatType::LIGHT_SWORD:
			TempArray = LightAttackMontage;
			break;
		case ECombatType::GREAT_SWORD:
			TempArray = GreatAttackMontage;
			break;
		}
		if(bRandomIdx)
		{
			int32 ArrayCount = TempArray.Num();
			if(ArrayCount == 0)
				Idx = 0;
			else
				Idx = FMath::RandRange(0, ArrayCount - 1);
		}

		//CombatComp->SetIsAttacking(true);
		StateManagerComp->SetCurrentState(ECharacterState::ATTACKING);
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
	if(CombatComp && StateManagerComp)
	{
		//CombatComp->SetIsAttacking(false);
		StateManagerComp->SetCurrentState(ECharacterState::NOTHING);
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
	//bDodging = true;
	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::DODGING);
	if(DodgeMontage)
		PlayAnimMontage(DodgeMontage);
}

bool AMeleeCharacter::CanAttack()
{
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::ATTACKING);
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DEAD);
	CharacterStates.Add(ECharacterState::DISABLED);
	CharacterStates.Add(ECharacterState::GENERAL_ACTION_STATE);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = (!StateManagerComp->IsCurrentStateEqualToThis(CharacterStates))  && (!GetCharacterMovement()->IsFalling());
	//return (!bTogglingCombat && !bDodging && !bIsDisabled && !bIsDead && !GetCharacterMovement()->IsFalling());
	return ReturnValue;
}

bool AMeleeCharacter::CanToggleCombat()
{
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::ATTACKING);
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DEAD);
	CharacterStates.Add(ECharacterState::DISABLED);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = !StateManagerComp->IsCurrentStateEqualToThis(CharacterStates);
	//return (!CombatComp->GetIsAttacking() && !bDodging && !bIsDisabled && !bIsDead);
	return ReturnValue;
}

bool AMeleeCharacter::CanDodge()
{
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::ATTACKING);
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DEAD);
	CharacterStates.Add(ECharacterState::DISABLED);
	CharacterStates.Add(ECharacterState::GENERAL_ACTION_STATE);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = (!StateManagerComp->IsCurrentStateEqualToThis(CharacterStates))  && (!GetCharacterMovement()->IsFalling());
	//return (!CombatComp->GetIsAttacking() && !bTogglingCombat && !bDodging && !bIsDisabled && !bIsDead && !GetCharacterMovement()->IsFalling());
	return ReturnValue;
}

FRotator AMeleeCharacter::GetDesiredRotation() //구르기시 캐릭터가 움직이고 있는 방향의 회전값을 반환
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

void AMeleeCharacter::ResetCombat()
{
	ResetAttack();
	if(StateManagerComp)
		StateManagerComp->ResetState();
	
	// bTogglingCombat = false;
	// bDodging = false;
	// bIsDisabled = false;
}

void AMeleeCharacter::SetLightAttackMontage(UAnimMontage* Montage)
{
    if(Montage)
        LightAttackMontage.Add(Montage);
}

void AMeleeCharacter::SetGreatAttackMontage(UAnimMontage* Montage)
{
    if(Montage)
        GreatAttackMontage.Add(Montage);
}

void AMeleeCharacter::SetDodgeMontage(UAnimMontage* Montage)
{
    if(Montage)
        DodgeMontage = Montage;
}

void AMeleeCharacter::ReceiveDamage(
	AActor* DamagedActor, 
	float Damage,
	AController* InstigatedBy, 
	FVector HitLocation, 
	UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const UDamageType* DamageType, 
	AActor* DamageCauser)
{
	if(ImpactSound)
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitLocation);

	if(ImpactParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitLocation);

	if(HitReactMontage)
		PlayAnimMontage(HitReactMontage);

	//bIsDisabled = true; //맞았을 때 아무것도 못하게. 변수만 추가한 상태. 아무 영향 없음
	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::DISABLED);
	

	CauseDamage(Damage); //대미지 적용
}

void AMeleeCharacter::CauseDamage(float Damage)
{
	HP = FMath::Clamp(HP - Damage, 0.f, HP - Damage);
	if(HP <= 0)
	{
		if(StateManagerComp)
			StateManagerComp->SetCurrentState(ECharacterState::DEAD);
	}
}

void AMeleeCharacter::Dead()
{
	//bIsDead = true;
	EnableRagdoll();
	ApplyHitReactionPhysicsVelocity(2000.f);
	if(CombatComp && CombatComp->GetEquippedWeapon())
	{
		CombatComp->GetEquippedWeapon()->SimulateWeaponPhysics();
	}
	GetWorldTimerManager().SetTimer(DestroyDeadTimerHandle, this, &ThisClass::DestroyDead, DestroyDeadTime);
}

void AMeleeCharacter::EnableRagdoll()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	FAttachmentTransformRules Rules = FAttachmentTransformRules::KeepWorldTransform;
	CameraBoom->AttachToComponent(GetMesh(), Rules, PelvisBoneName);
	CameraBoom->bDoCollisionTest = false;
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PelvisBoneName, 1.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
}

void AMeleeCharacter::ApplyHitReactionPhysicsVelocity(float InitSpeed)
{
	FVector NewVel = GetActorForwardVector() * (InitSpeed * -1.f);
	
	GetMesh()->SetPhysicsLinearVelocity(NewVel, false, PelvisBoneName);
}

void AMeleeCharacter::Test()
{	
	//테스트할 함수 넣기. Key Mapping : Tab
	Dead();

}

bool AMeleeCharacter::CanRecieveDamage()
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() != ECharacterState::DEAD)
		return true;
	else 
		return false;
}

void AMeleeCharacter::DestroyDead()
{
	if(CombatComp && CombatComp->GetEquippedWeapon())
	{
		CombatComp->GetEquippedWeapon()->Destroy();
	}
	Destroy();
}

void AMeleeCharacter::CharacterStateBegin(ECharacterState State)
{
	switch (State)
	{
	case ECharacterState::NOTHING:
		
		break;
	case ECharacterState::ATTACKING:
		
		break;
	case ECharacterState::DODGING:
		
		break;
	case ECharacterState::GENERAL_ACTION_STATE:
		
		break;
	case ECharacterState::DEAD:
			Dead();
		break;
	case ECharacterState::DISABLED:
		
		break;
	}
}

void AMeleeCharacter::CharacterStateEnd(ECharacterState State)
{
	switch (State)
	{
	case ECharacterState::NOTHING:
		
		break;
	case ECharacterState::ATTACKING:
		
		break;
	case ECharacterState::DODGING:
		
		break;
	case ECharacterState::GENERAL_ACTION_STATE:
		
		break;
	case ECharacterState::DEAD:
			
		break;
	case ECharacterState::DISABLED:
		
		break;
	}
}