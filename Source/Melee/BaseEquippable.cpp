#include "BaseEquippable.h"
#include "GameFramework/Character.h"

ABaseEquippable::ABaseEquippable()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	AttachSocketName = TEXT("");
}

void ABaseEquippable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UPrimitiveComponent* ABaseEquippable::GetItemMesh()
{
	if(ItemStaticMesh)
		return ItemStaticMesh;
	else 
		return ItemSkeletalMesh;
	
}

void ABaseEquippable::OnEquipped()
{
	bIsEquipped = true;
	AttachActor(AttachSocketName);
}

void ABaseEquippable::OnUnequipped()
{
	if(bIsEquipped)
		bIsEquipped = false;
}

void ABaseEquippable::AttachActor(FName ScoketName)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	Cast<ACharacter>(GetOwner())->GetMesh()->AttachToComponent(GetItemMesh(), Rules, ScoketName);
	
}