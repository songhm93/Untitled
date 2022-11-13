#include "MasterItem.h"


AMasterItem::AMasterItem()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMasterItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMasterItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMasterItem::UseItem(int32 ItemId, int32 SlotIndex)
{
	//사운드
	//이펙트 실행
	//이것도 테이블 만들어서 아이템 아이디에 맞는 사운드랑 이펙트 넣어두고 불러와서
	//실행시켜야할듯
}
