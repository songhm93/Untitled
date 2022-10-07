#include "MeleePlayerController.h"
#include "MeleeCharacter.h"


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
    Character = Cast<AMeleeCharacter>(InPawn);
}

void AMeleePlayerController::Tick(float DeltaTime)
{
   TrackChargedAttack(DeltaTime);
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
    Character = Character == nullptr ? Cast<AMeleeCharacter>(GetPawn()) : Character;
    if(Character && !bCharged)
        Character->LightAttack();
    bCharged = false;
}

void AMeleePlayerController::TrackChargedAttack(float DeltaTime)
{
    if(bLeftClickIsPressed)
    {
        LeftClickTime += DeltaTime;
        if(LeftClickTime >= ChargedTime)
        {
            if(Character)
                Character->ChargedAttack();
            bCharged = true;
            bLeftClickIsPressed = false;
        }
    }
    else
    {
        LeftClickTime = 0.f;
    }  
}
