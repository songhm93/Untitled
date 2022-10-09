#include "MeleePlayerController.h"
#include "MeleeCharacter.h"
#include "Types.h."
#include "Stats.h"
#include "StatsComponent.h"

AMeleePlayerController::AMeleePlayerController()
{
    bLeftClickIsPressed = false;
    LeftClickTime = 0.f;
    ChargedTime = 0.3f;
    bCharged = false;


}

void AMeleePlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    MeleeCharacter = Cast<AMeleeCharacter>(InPawn);

}

void AMeleePlayerController::Tick(float DeltaTime)
{
    TrackingChargedAttack(DeltaTime);
    TrackingSprint();
}

void AMeleePlayerController::BeginPlay()
{
    Super::BeginPlay();
    
}

void AMeleePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if(nullptr == InputComponent) return; 
    InputComponent->BindAction("LightAttack", IE_Pressed, this, &ThisClass::LightAttackPressed);
    InputComponent->BindAction("LightAttack", IE_Released, this, &ThisClass::LightAttackReleased);
}
 
void AMeleePlayerController::LightAttackPressed() 
{
    bLeftClickIsPressed = true;
}

void AMeleePlayerController::LightAttackReleased() 
{
    bLeftClickIsPressed = false; 
    MeleeCharacter = MeleeCharacter == nullptr ? Cast<AMeleeCharacter>(GetPawn()) : MeleeCharacter;
    if(MeleeCharacter && !bCharged)
        MeleeCharacter->LightAttack();
    bCharged = false;
}

void AMeleePlayerController::TrackingChargedAttack(float DeltaTime)
{
    if(bLeftClickIsPressed)
    {
        LeftClickTime += DeltaTime;
        if(LeftClickTime >= ChargedTime)
        {
            if(MeleeCharacter)
                MeleeCharacter->ChargedAttack();
            bCharged = true;
            bLeftClickIsPressed = false;
        }
    }
    else
    {
        LeftClickTime = 0.f;
    }  
}

void AMeleePlayerController::TrackingSprint()
{
    if(MeleeCharacter && MeleeCharacter->GetStatComp() && MeleeCharacter->GetMovementType() == EMovementType::SPRINTING)
    {   //현재 스태미너가 소모량보다 적은 경우, 달리지 않는데 쉬프트가 눌려 있는 경우
        if((MeleeCharacter->GetStatComp()->GetCurrentStatValue(EStats::STAMINA) < MeleeCharacter->GetSprintStaminaCost()) || MeleeCharacter->GetVelocity().Size() <= 0.f)
        {
            MeleeCharacter->SetMovementType(EMovementType::JOGGING);
        }
        MeleeCharacter->GetStatComp()->PlusCurrentStatValue(EStats::STAMINA, -(MeleeCharacter->GetSprintStaminaCost()));
    }
}
