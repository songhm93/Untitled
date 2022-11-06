#include "Portal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Blueprint/UserWidget.h"

#include "Widget/FadeWidget.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = false;
	SM1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM1"));
	SM2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM2"));
	SM1->SetupAttachment(RootComponent);
	SM2->SetupAttachment(SM1);

	FadeTime = 1.5f;

}

void APortal::BeginPlay()
{
	Super::BeginPlay();
	
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::Interact(AActor* Caller)
{
	TArray<AActor*> OutActor;
	UGameplayStatics::GetAllActorsOfClass(this, APortal::StaticClass(), OutActor);
	if(!OutActor.IsEmpty())
	{
		if(FadeWidgetClass)
			FadeWidget = CreateWidget<UFadeWidget>(GetWorld(), FadeWidgetClass);
		if(FadeWidget)
		{
			FadeWidget->AddToViewport();
			FadeWidget->Fade(false); //fade out
		}

		SwitchingDelegate.BindUFunction(this, FName("SwitchingLocation"), Caller, OutActor);
		GetWorld()->GetTimerManager().SetTimer(SwitchingTimerHandle, SwitchingDelegate, FadeTime, false);
	}
}

void APortal::SwitchingLocation(AActor* Caller, TArray<AActor*> OutActor)
{
	if(FadeWidget)
	{
		FadeWidget->Fade(true); //fade in
	}
	switch(Identify)
	{
		case EPortalIdentify::FIRST:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::SECOND)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::SECOND:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::FIRST)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::THIRD:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::FOURTH)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::FOURTH:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::THIRD)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::FIFTH:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::SIXTH)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::SIXTH:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::FIFTH)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::SEVENTH:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::EIGHTH)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
		case EPortalIdentify::EIGHTH:
			for(auto Portal : OutActor)
			{
				if(Cast<APortal>(Portal)->GetIdentify() == EPortalIdentify::SEVENTH)
				{
					Caller->SetActorLocation(Portal->GetActorLocation() + (Portal->GetActorRightVector() * 200.f) + FVector(0.f, 0.f, 200.f));
					return;
				}
			}
	}
}
