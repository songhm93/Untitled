#include "MeleePlayerController.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "../Type/Types.h."
#include "../Type/Stats.h"
#include "../Component/StatsComponent.h"
#include "../Component/CombatComponent.h"
#include "../Component/StateManagerComponent.h"

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
    BaseCharacter = Cast<ABaseCharacter>(InPawn);

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
    InputComponent->BindAction("Attack", IE_Pressed, this, &ThisClass::AttackButtonPressed);
    InputComponent->BindAction("Attack", IE_Released, this, &ThisClass::AttackButtonReleased);
}
 
void AMeleePlayerController::AttackButtonPressed() 
{
    bLeftClickIsPressed = true;
}

void AMeleePlayerController::AttackButtonReleased() 
{
    bLeftClickIsPressed = false; 
    //if(BaseCharacter && !bCharged)
        //BaseCharacter->LightAttack();
    if(!bCharged)
        OnLightAttack.ExecuteIfBound();
    bCharged = false;
}

void AMeleePlayerController::TrackingChargedAttack(float DeltaTime)
{
    if(bLeftClickIsPressed)
    {
        LeftClickTime += DeltaTime;
        if(LeftClickTime >= ChargedTime)
        {
            OnChargedAttack.ExecuteIfBound();
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
    bool Condition = 
        BaseCharacter && 
        BaseCharacter->GetStatComp() && 
        BaseCharacter->GetStateManagerComp() && 
        BaseCharacter->GetStateManagerComp()->GetMovementType() == EMovementType::SPRINTING;
    if(Condition)
    {   //현재 스태미너가 소모량보다 적은 경우, 달리지 않는데 쉬프트가 눌려 있는 경우
        if((BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::STAMINA) < BaseCharacter->GetSprintStaminaCost()) || BaseCharacter->GetVelocity().Size() <= 0.f)
        {
            BaseCharacter->SetMovementType(EMovementType::JOGGING);
        }
        BaseCharacter->GetStatComp()->PlusCurrentStatValue(EStats::STAMINA, -(BaseCharacter->GetSprintStaminaCost()));
    }
}
