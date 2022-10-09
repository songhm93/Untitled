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
#include "StatsComponent.h"
#include "Stats.h"

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

	FString Path = FString(TEXT("/Game/CombatSystem/DataTable/CommonTable"));
	InitDataTable(Path, EDataTableType::COMMON_TABLE);

	Path = FString(TEXT("/Game/CombatSystem/DataTable/LightSwordTable"));
	InitDataTable(Path, EDataTableType::LIGHT_SWORD_TABLE);

	Path = FString(TEXT("/Game/CombatSystem/DataTable/GreatSwordTable"));
	InitDataTable(Path, EDataTableType::GREAT_SWORD_TABLE);

	Path = FString(TEXT("/Game/CombatSystem/DataTable/DualSwordTable"));
	InitDataTable(Path, EDataTableType::DUAL_SWORD_TABLE);
	

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
		if(bSprintKeyPressed) SetMovementType(EMovementType::SPRINTING);
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

void AMeleeCharacter::InitDataTable(FString Path, EDataTableType TableType)
{
	
	UDataTable* TableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *Path));
	if(TableObject)
	{
		if(TableType == EDataTableType::COMMON_TABLE)
		{
			FCommonTable* CommonRow = TableObject->FindRow<FCommonTable>(FName("Common"), TEXT(""));
			if(CommonRow)
			{
				DodgeMontage = CommonRow->DodgeMontage;
				HitReactMontage = CommonRow->HitReactMontage;
				ChargedAttackMontage = CommonRow->ChargedAttackMontage;
				ImpactSound = CommonRow->ImpactSound;
				ImpactParticle = CommonRow->ImpactParticle;
			}
		}
		else if(TableType == EDataTableType::LIGHT_SWORD_TABLE)
		{
			FLightSwordTable* LightSwordRow = TableObject->FindRow<FLightSwordTable>(FName("LightSword"), TEXT(""));
			if(LightSwordRow)
			{
				for(auto Montage : LightSwordRow->LightAttackMontage)
				{
					LSLightAttackMontages.Add(Montage);
				}
				LSHeavyAttackMontage = LightSwordRow->HeavyAttackMontage;
				LSEnterCombatMontage = LightSwordRow->EnterCombatMontage;
				LSExitCombatMontage = LightSwordRow->ExitCombatMontage;
				LSSprintAttackMontage = LightSwordRow->SprintAttackMontage;
			}
		}
		else if(TableType == EDataTableType::GREAT_SWORD_TABLE)
		{
			FGreatSwordTable* GreatSwordRow = TableObject->FindRow<FGreatSwordTable>(FName("GreatSword"), TEXT(""));
			if(GreatSwordRow)
			{
				for(auto Montage : GreatSwordRow->LightAttackMontage)
				{
					GSLightAttackMontages.Add(Montage);
				}
				GSHeavyAttackMontage = GreatSwordRow->HeavyAttackMontage;
				GSEnterCombatMontage = GreatSwordRow->EnterCombatMontage;
				GSExitCombatMontage = GreatSwordRow->ExitCombatMontage;
			}
		}
		else if(TableType == EDataTableType::DUAL_SWORD_TABLE)
		{
			FDualSwordTable* DualSwordRow = TableObject->FindRow<FDualSwordTable>(FName("DualSword"), TEXT(""));
			if(DualSwordRow)
			{
				for(auto Montage : DualSwordRow->LightAttackMontage)
				{
					DSLightAttackMontages.Add(Montage);
				}
				DSHeavyAttackMontage = DualSwordRow->HeavyAttackMontage;
				DSEnterCombatMontage = DualSwordRow->EnterCombatMontage;
				DSExitCombatMontage = DualSwordRow->ExitCombatMontage;	
			}
		}
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
			PerformAttack(CombatComp->GetAttackCount(), false, ECharacterAction::LIGHT_ATTACK);
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
			PerformAttack(CombatComp->GetAttackCount(), false, ECharacterAction::HEAVY_ATTACK);
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
			PerformAttack(CombatComp->GetAttackCount(), false, ECharacterAction::CHARGED_ATTACK);
		}
	}
}

void AMeleeCharacter::PerformAttack(int32 AttackIdx, bool bRandomIdx, ECharacterAction Action)
{
	if(!CanAttack()) return;
	int32 Idx = AttackIdx;
	if(CombatComp && CombatComp->GetEquippedWeapon() && StateManagerComp && StatComp)
	{
		TArray<UAnimMontage*> TempArray;
		EWeaponType WeaponType = CombatComp->GetEquippedWeapon()->GetWeaponType();
		if(WeaponType == EWeaponType::LIGHT_SWORD)
		{
			switch(Action)
			{
				case ECharacterAction::LIGHT_ATTACK:
					TempArray = LSLightAttackMontages;
					if(Idx == 0)
						AttackActionCorrectionValue = 1.f;
					else if(Idx == 1)
						AttackActionCorrectionValue = 1.5f;
					else 
						AttackActionCorrectionValue = 2.f;
				break;
				case ECharacterAction::HEAVY_ATTACK:
					TempArray.Add(LSHeavyAttackMontage);
					Idx = 0;
					AttackActionCorrectionValue = 2.f;
				break;
				case ECharacterAction::CHARGED_ATTACK:
					TempArray.Add(ChargedAttackMontage);
					Idx = 0;
					AttackActionCorrectionValue = 2.5f;
				break;
			}
			if(CurrentMovementType == EMovementType::SPRINTING)
			{
				TempArray.Empty();
				TempArray.Add(LSSprintAttackMontage);
				Idx = 0;
			}
		}
		else if(WeaponType == EWeaponType::GREAT_SWORD)
		{
			switch(Action)
			{
				case ECharacterAction::LIGHT_ATTACK:
					TempArray = GSLightAttackMontages;
				break;
				case ECharacterAction::HEAVY_ATTACK:
					TempArray.Add(GSHeavyAttackMontage);
					Idx = 0;
				case ECharacterAction::CHARGED_ATTACK:
					TempArray.Add(ChargedAttackMontage);
					Idx = 0;
				break;
			}
		}
		if(TempArray.IsEmpty()) return;
		StateManagerComp->SetCurrentState(ECharacterState::ATTACKING);
		StateManagerComp->SetCurrentAction(Action);
		// if(bRandomIdx)
		// {
		// 	int32 ArrayCount = TempArray.Num();
		// 	if(ArrayCount == 0)
		// 		Idx = 0;
		// 	else
		// 		Idx = FMath::RandRange(0, ArrayCount - 1);
		// }
		if(TempArray.Num() > Idx)
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
		StateManagerComp->SetCurrentState(ECharacterState::NOTHING);
		if(CombatComp->GetIsAttackSaved())
		{
			CombatComp->SetIsAttackSaved(false);
			PerformAttack(CombatComp->GetAttackCount(), false, ECharacterAction::LIGHT_ATTACK);
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

	if(StateManagerComp)
		StateManagerComp->SetCurrentState(ECharacterState::DISABLED);
	
	CauseDamage(Damage); //대미지 적용
}

void AMeleeCharacter::CauseDamage(float Damage)
{
	if(StatComp)
	{
		StatComp->PlusCurrentStatValue(EStats::HP, -Damage); //HP계산
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
	FVector NewVel = GetActorForwardVector() * (InitSpeed * -1.f);
	
	GetMesh()->SetPhysicsLinearVelocity(NewVel, false, PelvisBoneName);
}

void AMeleeCharacter::Test()
{	
	//테스트할 함수 넣기. Key Mapping : 5
	
	if(StatComp)
	{
		StatComp->PlusCurrentStatValue(EStats::HP, -50);
		
		// TMap<EStats, FBaseStat> BaseStat = StatComp->GetBaseStats();
		// UE_LOG(LogTemp, Warning, TEXT("BaseStatHP:%f"), BaseStat[EStats::HP].BaseValue);
		// UE_LOG(LogTemp, Warning, TEXT("BaseStatSTAM:%f"), BaseStat[EStats::STAMINA].BaseValue);
		// UE_LOG(LogTemp, Warning, TEXT("CurrentStatHP:%f"), StatComp->GetCurrentStatValue(EStats::HP));
		// UE_LOG(LogTemp, Warning, TEXT("CurrentStatSTAM:%f"), StatComp->GetCurrentStatValue(EStats::STAMINA));
		// StatComp->PlusCurrentStatValue(EStats::HP, -15);

		// UE_LOG(LogTemp, Warning, TEXT("더한 후CurrentStatHP:%f"), StatComp->GetCurrentStatValue(EStats::HP));
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
	
	if(GetCharacterMovement())
	{
		switch(CurrentMovementType)
		{
			case EMovementType::WALKING:
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
				break;
			case EMovementType::JOGGING:
				GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
				break;
			case EMovementType::SPRINTING:
				GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
				break;
		}
	}
}

