#include "MeleePlayerController.h"
#include "MeleeCharacter.h"
//#include "Blueprint/UserWidget.h"

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
   TrackChargedAttack(DeltaTime);
}

void AMeleePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // if(MainHUDClass) //위젯은 블루프린트로 하는걸로
    // {
    //     MainHUD = CreateWidget<UUserWidget>(this, MainHUDClass);
    //     if(MainHUD)
    //     {
    //         //
    //     }
    // }
    UE_LOG(LogTemp, Warning, TEXT("플컨"));
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

void AMeleePlayerController::TrackChargedAttack(float DeltaTime)
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
