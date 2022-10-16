#include "MeleeCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interface/Interactable.h"
#include "MeleeAnimInstance.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Component/CombatComponent.h"
#include "Component/StateManagerComponent.h"
#include "Component/StatsComponent.h"
#include "Component/TargetingComponent.h"
#include "Components/WidgetComponent.h"
#include "AttackDamageType.h"
#include "Type/Stats.h"
#include "Type/Types.h"

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
	StatComp = CreateDefaultSubobject<UStatsComponent>(TEXT("StatComp"));

	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::NOTHING);

	TargetingComp = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComp"));
	LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(GetMesh());
	PelvisBoneName = TEXT("pelvis");
	DestroyDeadTime = 4.f;

	CurrentMovementType = EMovementType::JOGGING;
	WalkSpeed = 300.f;
	JogSpeed = 500.f;
	SprintSpeed = 700;
	bHeavyAttack = false;
	DodgeStaminaCost = 10.f;
	SprintStaminaCost = 0.2f;
	bSprintKeyPressed = false;
	AttackActionCorrectionValue = 1.f;
	MouseSensitivity = 25.f;
	bHitFront = false;


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
		StateManagerComp->OnActionBegin.AddDynamic(this, &ThisClass::CharacterActionBegin);
		StateManagerComp->OnActionEnd.AddDynamic(this, &ThisClass::CharacterActionEnd);
	}
	
	if(StatComp)
	{
		StatComp->InitStats();
		StatComp->PlusCurrentStatValue(EStats::HP, 0.00000001f); //위젯 띄우는거, 스탯 초기화, 테이블 초기화 순서때문에 게임 시작시 스탯 비어보임을 가리는 꼼수
		StatComp->PlusCurrentStatValue(EStats::STAMINA, 0.00000001f);
	}

	if(LockOnWidget)
	{
		LockOnWidget->SetVisibility(false);
		LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
		LockOnWidget->SetDrawSize(FVector2D(14.f, 14.f));
	}
	
}

void AMeleeCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("ToggleCombat", IE_Pressed, this, &ThisClass::ToggleCombat);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ThisClass::InteractButtonPressed);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ThisClass::Dodge);
	PlayerInputComponent->BindAction("ToggleWalk", IE_Pressed, this, &ThisClass::ToggleWalk);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ThisClass::SprintButtonPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ThisClass::SprintButtonReleased);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Released, this, &ThisClass::HeavyAttack);
	PlayerInputComponent->BindAction("ToggleLockOn", IE_Released, this, &ThisClass::ToggleLockOn);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ThisClass::Test);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMeleeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMeleeCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AMeleeCharacter::TurnRight);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AMeleeCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AMeleeCharacter::LookUp);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AMeleeCharacter::LookUpAtRate);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMeleeCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMeleeCharacter::TouchStopped);
}

void AMeleeCharacter::TurnRight(float Rate)
{
	//if(TargetingComp && TargetingComp->GetIsTargeting()) return;
	AddControllerYawInput(Rate * MouseSensitivity * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::LookUp(float Rate)
{
	//if(TargetingComp && TargetingComp->GetIsTargeting()) return;
	AddControllerPitchInput(Rate * MouseSensitivity * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::Jump()
{
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DISABLED);
	
	if(StateManagerComp && StateManagerComp->IsCurrentStateEqualToThis(CharacterStates) && StateManagerComp->GetCurrentAction() == ECharacterAction::CHARGED_ATTACK) return;
	StopAnimMontage();
	Super::Jump();
	ResetCombat();
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
	if(TargetingComp && TargetingComp->GetIsTargeting()) return;
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::LookUpAtRate(float Rate)
{
	if(TargetingComp && TargetingComp->GetIsTargeting()) return;
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMeleeCharacter::MoveForward(float Value)
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() == ECharacterState::ATTACKING) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		if(bSprintKeyPressed) SetMovementType(EMovementType::SPRINTING);
		AddMovementInput(Direction, Value);
	}
}

void AMeleeCharacter::MoveRight(float Value)
{
	if(StateManagerComp && StateManagerComp->GetCurrentState() == ECharacterState::ATTACKING) return;
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
	if(!CanToggleCombat() || (CombatComp && !CombatComp->GetEquippedWeapon())) return;

	EWeaponType WeaponType = CombatComp->GetEquippedWeapon()->GetWeaponType();

	if(StateManagerComp)
	{
		StateManagerComp->SetCurrentState(ECharacterState::GENERAL_STATE);
		
		UAnimMontage* EnterCombatMontage = nullptr;
		UAnimMontage* ExitCombatMontage = nullptr;
		TArray<UAnimMontage*> TempArray;
		switch (WeaponType)
		{
		case EWeaponType::LIGHT_SWORD:
			EnterCombatMontage = LSEnterCombatMontage;
			ExitCombatMontage = LSExitCombatMontage;
			break;
		case EWeaponType::GREAT_SWORD:
			EnterCombatMontage = GSEnterCombatMontage;
			ExitCombatMontage = GSExitCombatMontage;
			break;
		case EWeaponType::DUAL_SWORD:
			EnterCombatMontage = DSEnterCombatMontage;
			ExitCombatMontage = DSExitCombatMontage;
			break;
		}
		if(!CombatComp->GetCombatState()) //손에 장착 상태가 아니면
		{
			TempArray.Add(EnterCombatMontage);
			PerformAction(0, TempArray, ECharacterState::GENERAL_STATE, ECharacterAction::ENTER_COMBAT);
			CombatComp->SetCombatState(true);
			if(GetMesh() && GetMesh()->GetAnimInstance())
			{
				Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetCombatState(true);
			}
			if(TargetingComp) TargetingComp->UpdateRotationMode();
		}
		else
		{
			TempArray.Add(ExitCombatMontage);
			PerformAction(0, TempArray, ECharacterState::GENERAL_STATE, ECharacterAction::EXIT_COMBAT);
			CombatComp->ResetAttackCount();
			CombatComp->SetCombatState(false);
			if(GetMesh() && GetMesh()->GetAnimInstance())
			{
				Cast<UMeleeAnimInstance>(GetMesh()->GetAnimInstance())->SetCombatState(false);
			}
			if(TargetingComp)
			{
				TargetingComp->DisableLockOn();
				TargetingComp->UpdateRotationMode();
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
		EDrawDebugTrace::None, 
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
	bHeavyAttack = false;
	if(!CanAttack()) return;
	if(CombatComp && StateManagerComp && (StateManagerComp->GetCurrentState() == ECharacterState::NOTHING))
	{
		if(StateManagerComp->GetCurrentState() == ECharacterState::ATTACKING)
		{
			CombatComp->SetIsAttackSaved(true);
		}
		else
		{
			PerformLightAttack(CombatComp->GetAttackCount());
		}
	}
}

void AMeleeCharacter::HeavyAttack()
{
	bHeavyAttack = true;
	if(!CanAttack()) return;
	if(CombatComp && StateManagerComp && (StateManagerComp->GetCurrentState() == ECharacterState::NOTHING))
	{
		if(StateManagerComp->GetCurrentState() == ECharacterState::ATTACKING)
		{
			CombatComp->SetIsAttackSaved(true);
		}
		else
		{
			PerformAttack(ECharacterAction::HEAVY_ATTACK);
		}
	}
}

void AMeleeCharacter::ChargedAttack()
{
	bHeavyAttack = false;
	if(!CanAttack()) return;
	if(CombatComp && StateManagerComp && (StateManagerComp->GetCurrentState() == ECharacterState::NOTHING))
	{
		if(StateManagerComp->GetCurrentState() == ECharacterState::ATTACKING)
		{
			CombatComp->SetIsAttackSaved(true);
		}
		else
		{
			PerformAttack(ECharacterAction::CHARGED_ATTACK);
		}
	}
}

void AMeleeCharacter::PerformLightAttack(int32 AttackCount)
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	UAnimMontage* TempLightAttackMontage = nullptr;
	UAnimMontage* TempSprintAttackMontage = nullptr;
	FName SectionName = TEXT("");

	if(StateManagerComp && CombatComp && AnimInst && CombatComp->GetEquippedWeapon())
	{
		SectionName = GetLightAttackSectionName(AttackCount);

		if(CombatComp->GetEquippedWeapon()->GetWeaponType() == EWeaponType::LIGHT_SWORD)
		{
			TempLightAttackMontage = LSLightAttackMontage;
			TempSprintAttackMontage = LSSprintAttackMontage;
		}
		else if(CombatComp->GetEquippedWeapon()->GetWeaponType() == EWeaponType::DUAL_SWORD)
		{
			TempLightAttackMontage = DSLightAttackMontage;
			TempSprintAttackMontage = DSSprintAttackMontage;
		}
		else if(CombatComp->GetEquippedWeapon()->GetWeaponType() == EWeaponType::GREAT_SWORD)
		{
			TempLightAttackMontage = GSLightAttackMontage;
		}
	
		if(TempLightAttackMontage && CurrentMovementType != EMovementType::SPRINTING)
		{
			AnimInst->Montage_Play(TempLightAttackMontage);
			AnimInst->Montage_JumpToSection(SectionName, TempLightAttackMontage);
			CombatComp->IncrementAttackCount();
		}
		else if(TempSprintAttackMontage && CurrentMovementType == EMovementType::SPRINTING)
		{
			AnimInst->Montage_Play(TempSprintAttackMontage);
		}

		StateManagerComp->SetCurrentState(ECharacterState::ATTACKING);
		StateManagerComp->SetCurrentAction(ECharacterAction::LIGHT_ATTACK);
	}
}

FName AMeleeCharacter::GetLightAttackSectionName(int32 AttackCount)
{
	if(AttackCount == 0) 
	{
		AttackActionCorrectionValue = 1.f;
		return TEXT("First");
	}
	else if(AttackCount == 1) 
	{
		AttackActionCorrectionValue = 2.f;
		return TEXT("Second");
	}
	else if(AttackCount == 2) 
	{
		AttackActionCorrectionValue = 3.f;
		return TEXT("Third");
	}
	else
	{
		CombatComp->ResetAttackCount();
		AttackActionCorrectionValue = 1.f;
		return TEXT("First");
	}
}

void AMeleeCharacter::PerformAttack(ECharacterAction Action)
{
	UAnimMontage* TempMontage = nullptr;
	if(CombatComp && CombatComp->GetEquippedWeapon() && StateManagerComp && StatComp)
	{
		EWeaponType WeaponType = CombatComp->GetEquippedWeapon()->GetWeaponType();
		if(WeaponType == EWeaponType::LIGHT_SWORD)
		{
			switch(Action)
			{
				case ECharacterAction::HEAVY_ATTACK:
					TempMontage = LSHeavyAttackMontage;
					AttackActionCorrectionValue = 3.f;
				break;
				case ECharacterAction::CHARGED_ATTACK:
					TempMontage = LSChargedAttackMontage;
					AttackActionCorrectionValue = 4.f;
				break;
			}
		}
		else if(WeaponType == EWeaponType::GREAT_SWORD)
		{
			switch(Action)
			{
				case ECharacterAction::HEAVY_ATTACK:
					TempMontage = GSHeavyAttackMontage;
				case ECharacterAction::CHARGED_ATTACK:
					TempMontage = GSChargedAttackMontage;
				break;
			}
		}else if(WeaponType == EWeaponType::DUAL_SWORD)
		{
			switch(Action)
			{
				case ECharacterAction::HEAVY_ATTACK:
					TempMontage = DSHeavyAttackMontage;
					AttackActionCorrectionValue = 3.f;
				break;
				case ECharacterAction::CHARGED_ATTACK:
					TempMontage = DSChargedAttackMontage;
					AttackActionCorrectionValue = 4.f;
				break;
			}
		}
		StateManagerComp->SetCurrentState(ECharacterState::ATTACKING);
		StateManagerComp->SetCurrentAction(Action);
		
		
		PlayAnimMontage(TempMontage);
	}
}

void AMeleeCharacter::ContinueAttack() //애님 노티파이로 호출될 함수. 공격중에 또 공격 버튼을 눌렀을 때 바로 다음 공격이 발생하는 것을 막기 위함.
{
	if(CombatComp && StateManagerComp)
	{
		StateManagerComp->SetCurrentState(ECharacterState::NOTHING);
		if(CombatComp->GetIsAttackSaved())
		{
			CombatComp->SetIsAttackSaved(false);
			PerformAttack(ECharacterAction::LIGHT_ATTACK);
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
	if(!CanDodge() || (CombatComp && !CombatComp->GetEquippedWeapon())) return;
	
	PerformDodge();
}

void AMeleeCharacter::PerformDodge()
{
	if(StateManagerComp && DodgeMontage && StatComp)
	{
		StateManagerComp->SetCurrentState(ECharacterState::DODGING);
		StateManagerComp->SetCurrentAction(ECharacterAction::DODGE);
		PlayAnimMontage(DodgeMontage);
		StatComp->PlusCurrentStatValue(EStats::STAMINA, -(DodgeStaminaCost));
	}
}

bool AMeleeCharacter::CanAttack()
{
	bool Condition = CombatComp && (!CombatComp->GetEquippedWeapon() || !CombatComp->GetCombatState());
	if(Condition) return false;
	
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::ATTACKING);
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DEAD);
	CharacterStates.Add(ECharacterState::DISABLED);
	CharacterStates.Add(ECharacterState::GENERAL_STATE);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = (!StateManagerComp->IsCurrentStateEqualToThis(CharacterStates)) && (!GetCharacterMovement()->IsFalling());
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
	return ReturnValue;
}

bool AMeleeCharacter::CanDodge()
{
	if(StatComp && (StatComp->GetCurrentStatValue(EStats::STAMINA) < DodgeStaminaCost)) return false;
	TArray<ECharacterState> CharacterStates;
	CharacterStates.Add(ECharacterState::DODGING);
	CharacterStates.Add(ECharacterState::DEAD);
	CharacterStates.Add(ECharacterState::DISABLED);
	CharacterStates.Add(ECharacterState::GENERAL_STATE);
	bool ReturnValue = false;
	if(StateManagerComp && GetCharacterMovement())
		ReturnValue = (!StateManagerComp->IsCurrentStateEqualToThis(CharacterStates))  && (!GetCharacterMovement()->IsFalling());
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
	{
		StateManagerComp->ResetState();
		StateManagerComp->ResetAction();
	}
}

void AMeleeCharacter::ReceiveDamage(
	AActor* DamagedActor, 
	float EnemyATK,
	AController* InstigatedBy, 
	FVector HitLocation, 
	UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const UDamageType* DamageType, 
	AActor* DamageCauser)
{
	if(InstigatedBy)
	{
		float DotProductResult = GetDotProductTo(InstigatedBy->GetPawn()); //맞은 캐릭터와 때린 캐릭터간의 내적
		bHitFront = FMath::IsWithin(DotProductResult, -0.1f, 1.f);
	}

	if(DamageType)
	{
		ApplyHitReaction(Cast<UAttackDamageType>(DamageType)->GetDamageType());
		ApplyImpactEffect(Cast<UAttackDamageType>(DamageType)->GetDamageType(), HitLocation);
	}
		

	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::DISABLED);
	
	CalcReceiveDamage(EnemyATK);
}

void AMeleeCharacter::CalcReceiveDamage(float EnemyATK) //받는 총 대미지 계산
{
	//대미지 계산
	if(StatComp)
	{
		const float Def = StatComp->GetCurrentStatValue(EStats::DEF);
		const float Result = FMath::Clamp((EnemyATK * FMath::RandRange(0.8, 1.2)) * (1 - (Def / (100 + Def))), 0, INT_MAX);
		StatComp->PlusCurrentStatValue(EStats::HP, Result * -1); //HP 적용
		if(StatComp->GetCurrentStatValue(EStats::HP) <= 0)
		{
			if(StateManagerComp)
				StateManagerComp->SetCurrentState(ECharacterState::DEAD);
		}
	}
}

void AMeleeCharacter::Dead()
{
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
	const FVector NewVel = GetActorForwardVector() * (InitSpeed * -1.f);
	
	GetMesh()->SetPhysicsLinearVelocity(NewVel, false, PelvisBoneName);
}

void AMeleeCharacter::Test()
{	
	//테스트할 함수 넣기. Key Mapping : 5
	
	if(StatComp)
	{
		StatComp->PlusCurrentStatValue(EStats::HP, -50);
	
	}	
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
	case ECharacterState::GENERAL_STATE:
		
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
	case ECharacterState::GENERAL_STATE:
		
		break;
	case ECharacterState::DEAD:
			
		break;
	case ECharacterState::DISABLED:
		
		break;
	}
}

void AMeleeCharacter::CharacterActionBegin(ECharacterAction Action)
{
	switch(Action)
	{
		case ECharacterAction::NOTHING:

			break;
		case ECharacterAction::LIGHT_ATTACK:

			break;
		case ECharacterAction::HEAVY_ATTACK:

			break;
		case ECharacterAction::CHARGED_ATTACK:

			break;
		case ECharacterAction::DODGE:

			break;
		case ECharacterAction::ENTER_COMBAT:

			break;
		case ECharacterAction::EXIT_COMBAT:

			break;
	}
}

void AMeleeCharacter::CharacterActionEnd(ECharacterAction Action)
{
	switch(Action)
	{
		case ECharacterAction::NOTHING:

			break;
		case ECharacterAction::LIGHT_ATTACK:

			break;
		case ECharacterAction::HEAVY_ATTACK:

			break;
		case ECharacterAction::CHARGED_ATTACK:

			break;
		case ECharacterAction::DODGE:

			break;
		case ECharacterAction::ENTER_COMBAT:

			break;
		case ECharacterAction::EXIT_COMBAT:

			break;
	}
}

void AMeleeCharacter::PerformAction(int32 MontageIdx, TArray<UAnimMontage*> Montage, ECharacterState State, ECharacterAction Action)
{
	int32 Idx = MontageIdx;
	if(CombatComp && CombatComp->GetEquippedWeapon() && StateManagerComp)
	{
		StateManagerComp->SetCurrentState(State);
		StateManagerComp->SetCurrentAction(Action);

		if(!Montage.IsEmpty())	PlayAnimMontage(Montage[0]);
		
		//배열로 사용할만큼인지
	}
}

void AMeleeCharacter::ToggleWalk()
{
	if(CurrentMovementType != EMovementType::WALKING)
		SetMovementType(EMovementType::WALKING);
	else
		SetMovementType(EMovementType::JOGGING);
	
}

void AMeleeCharacter::SprintButtonPressed()
{
	bSprintKeyPressed = true;
	if(GetVelocity().Size() <= 0.f) return;
	SetMovementType(EMovementType::SPRINTING);
}

void AMeleeCharacter::SprintButtonReleased()
{
	bSprintKeyPressed = false;
	SetMovementType(EMovementType::JOGGING);
}

void AMeleeCharacter::SetMovementType(EMovementType Type)
{
	if(CurrentMovementType != Type)
		CurrentMovementType = Type;
	
	if(TargetingComp)
		TargetingComp->UpdateRotationMode();
	if(GetCharacterMovement())
	{
		switch(CurrentMovementType)
		{
			case EMovementType::WALKING:
				OnSprintState.Execute(true);
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
				break;
			case EMovementType::JOGGING:
				OnSprintState.Execute(true);
				GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
				break;
			case EMovementType::SPRINTING:
				OnSprintState.Execute(false);
				GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
				break;
		}
	}
}

void AMeleeCharacter::Equip(ABaseEquippable* Equipment)
{
	if(CombatComp && Equipment)
	{
		CombatComp->OnEquipped(Equipment);
	}
}

void AMeleeCharacter::ToggleLockOn()
{
	if(CombatComp && !CombatComp->GetCombatState()) return;
	if(TargetingComp) TargetingComp->ToggleLockOn();
}

bool AMeleeCharacter::CanBeTargeted()
{
	if(StateManagerComp)
	{
		TArray<ECharacterState> State;
		State.Add(ECharacterState::DEAD);
		return !StateManagerComp->IsCurrentStateEqualToThis(State);
	}
	return false;
}

void AMeleeCharacter::OnTargeted(bool IsTargeted)
{
	if(LockOnWidget)
	{
		LockOnWidget->SetVisibility(IsTargeted);
	}
}

void AMeleeCharacter::ApplyHitReaction(EDamageType DamageType)
{
	switch (DamageType)
	{
	case EDamageType::MELEE_DAMAGE:
		PerformHitReact();
		break;
	case EDamageType::KNOCKDOWN_DAMAGE:
		PerformKnockdown();
		break;
	}
}

void AMeleeCharacter::ApplyImpactEffect(EDamageType DamageType, FVector HitLocation)
{
	if(ImpactSound && ImpactParticle)
	{
		switch (DamageType)
		{
		case EDamageType::MELEE_DAMAGE:
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitLocation); 
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitLocation);
			break;
		case EDamageType::KNOCKDOWN_DAMAGE:
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitLocation); //나중에 다른걸로 추가
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitLocation);
			break;
		}
	}	
}

void AMeleeCharacter::PerformHitReact()
{
	if(bHitFront)
	{
		if(HitReactFrontMontage)
			PlayAnimMontage(HitReactFrontMontage);
	}
	else
	{
		if(HitReactBackMontage)
			PlayAnimMontage(HitReactBackMontage);
	}
	
}

void AMeleeCharacter::PerformKnockdown()
{
	if(bHitFront)
	{
		if(KnockdownFrontMontage)
			PlayAnimMontage(KnockdownFrontMontage);
	}
	else
	{
		if(KnockdownBackMontage)
			PlayAnimMontage(KnockdownBackMontage);
	}
}

float AMeleeCharacter::PerformCombatAction(ECharacterAction Action, ECharacterState State)
{
	return 0.f;
}


