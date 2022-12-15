# Untitled 포트폴리오

포트폴리오 목적으로 제작한 RPG 게임입니다.

엔진 : UnrealEngine 5.0.3 <br>
제작기간 : 약 2달 <br>
개발 규모 : 1인 개발 <br>
DB : MySql(서버 연동으로 멀티 게임을 기반에 둔 것이 아닌 공부 목적으로 DB 사용, 웹서버 언어로는 코틀린을 사용) <br>

### Character
+ Character - Interface 상속 <br>
	+ Interact, 전투에 관련된 함수들은 부모 클래스로 호출하기 위해 인터페이스를 상속, 재정의해서 사용

![Character](https://user-images.githubusercontent.com/27758519/207755957-77522c8a-e46e-48b7-944d-de5fc50b19a7.jpg)





+ EnemyCharacter
	+ 몬스터 범위에 들어가면 선제 공격을 하는 PreemptiveMonster 클래스와 플레이어가 공격을 했을 때 어그로가 끌리는 NonPreemptiveMonster 클래스로 나누어 설계 <br>
	+ 선제 공격을 하는 몬스터 클래스에서 PMMob 클래스와 BossMonster 클래스로 나누어 설계함

![EnemyCharacter](https://user-images.githubusercontent.com/27758519/207756445-a0955fb6-11a3-4963-8a11-300d07b644bb.jpg)



# RampageMonster
+ BossMonster에 필요한 변수들과 필요한 함수들 가상 함수로 선언, 상속받아 재정의해서 사용
+ 스킬들에 필요한 함수들은 추가하여 사용
<br>

```c++
void ARampageMonster::SetSquareArea()
{
    bActiveSquare = false;

    int32 RandXValue = FMath::RandRange(-5, 5);
    int32 RandYValue = FMath::RandRange(-5, 5);

    RandXValue *= SquareAreaXLength;
    RandYValue *= SquareAreaYLength;
    RandXValue += SquareAreaRefLoc.X;
    RandYValue += SquareAreaRefLoc.Y;

    if(IsExistLoc(RandXValue + RandYValue)) //존재하면 다시
    {
        SetSquareArea();
        return;
    }
    else 
    {
        FActorSpawnParameters Params; 
        Params.Owner = this;
        Params.Instigator = Cast<APawn>(this);
        
        FVector ResultLoc = FVector(RandXValue, RandYValue, SquareAreaRefLoc.Z);
        FTransform SpawnTransform = GetActorTransform();
        SpawnTransform.SetLocation(ResultLoc);
        SpawnTransform.SetRotation(FQuat4d::MakeFromRotator(FRotator::ZeroRotator));
        ASquareArea* SquareArea = GetWorld()->SpawnActor<ASquareArea>(SquareAreaClass, SpawnTransform, Params);
        SquareAreaCoord.Add(FSquareArea(RandXValue, RandYValue, RandXValue + RandYValue, SquareArea));
        bActiveSquare = true;
    }
}

```

![바닥패턴](https://user-images.githubusercontent.com/27758519/207754218-651d658c-d093-486e-b26e-1e485cfaad1e.jpg)




# 주요 컴포넌트

+ CombatComponent
  + 전투와 관련된 부분들 처리를 위한 컴포넌트
  + 장착된 장비 관리(장착 장비의 스탯 Delegate로 업데이트)
  + MonsterCombatComponent
  	+ 몬스터 기본 공격 등 추가 기능을 위해 상속받아 사용

+ StateManagerComponent
  + 상태 관리를 위한 컴포넌트

+ StatsComponent
  + 기본 스탯들 관리를 위한 컴포넌트
  + Stat Widget에 Delegate로 값 업데이트 
  ```c++
  void UStatsComponent::PlusCurrentStatValue(EStats Stat, float Value) 
  {
	  if(Value != 0.f)
	  {
		  if(Stat == EStats::ATK || Stat == EStats::DEF)
		  {
			  SetCurrentStatValue(Stat, CurrentStats[Stat] + Value);
		  }
		  else
		  {
			  SetCurrentStatValue(Stat, FMath::Clamp(CurrentStats[Stat] + Value, 0.f, MaxStats[Stat]));
		  }
		
		  OnPlusCurrentStatValue.Broadcast(Stat, CurrentStats[Stat]); // 위젯으로 Broadcast
	  }
  }
  ```
  + DB 연동으로 플레이어 정보 초기화 (웹서버 연동으로 서버쪽에 요청 후 받은 JSON을 파싱하여 구조체 형태로 받아 저장)
 
 + InventoryComponent
    + 인벤토리 관리를 위한 컴포넌트
    + DB 연동으로 인벤토리 관리
    + DB에 저장되는 형태와 동일하게 저장하는 배열과 인벤토리 슬롯 하나하나에 어떤 것이 들어있는지 저장하는 배열
    + InitInventory 함수로 두 배열 정리 후 InventoryWidget에 Delegate Execute
  

# NPC
+ BaseNPC 
  + 상호작용 가능한 클래스들은 Interactable 인터페이스를 상속 
  + 상호작용 버튼을 누르면 SphereTrace를 이용해 HitResult를 받고 액터들 중 Interactable 인터페이스를 상속받은 액터면 해당 함수 호출 
   ```c++
  if(OutHit.GetActor())
	{
		IInteractable* Interactable = Cast<IInteractable>(OutHit.GetActor());
		if(Interactable)
		{
			Interactable->Interact(this);
		}
	}
  
  ```

+ MerchantNPC
  + BaseNPC를 상속, 전용 위젯과 판매하는 아이템 번호를 배열로 가짐 

+ QuestNPC
  + BaseNPC를 상속, 퀘스트 관련 위젯들과 퀘스트 Id를 배열로 가짐

# BaseWeapon
+ 무기 종류를 DualWeapon만 파생시켜 구현했지만, 여러 종류를 편하게 관리할 수 있게 BaseWeapon 클래스 설계
+ 무기마다 다른 스킬을 갖는 컨셉이여서 BaseWeapon 클래스는 스킬 함수, 스킬의 AnimMontage, 스킬 4가지의 Cooldown, 계수, 무기의 공격력, 스킬 정보 구조체를 담는 Map을 가짐
  + DualWeapon
    + BaseWeapon을 상속받아 기본 변수는 그대로 사용, 무기 컨셉에 맞는 스킬에 필요한 변수들과 함수를 추가로 선언하여 사용
    + 스킬을 실행하는 가상 함수 재정의
    ``` c++
    void ADualWeapon::Skill2()
    {
        Super::Skill2();
    
        ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
        if(OwnerCharacter && OwnerCharacter->GetMesh())
        {
            if(OwnerCharacter->GetStateManagerComp())
                OwnerCharacter->GetStateManagerComp()->SetCurrentState(ECurrentState::ATTACKING);
        
            UAnimInstance* AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
            if(AnimInst && Skill2Montage)
            {
                AnimInst->Montage_Play(Skill2Montage);
            }
            Skill2Range();
        }
    }
     ```

# Widget
+ C++로 구현하다가 편의성으로 후반부에는 블루프린트로 구현
+ 주요 위젯 기능
  ```c++
  void UInventoryWidget::GenerateSlotWidgets() //슬롯들 업데이트
  {
      if (SlotPanel && CurrentGold)
      {
          SlotPanel->ClearChildren();
          SlotWidgets.Empty(50);
          if (InventoryComp && InventorySlotWidgetClass)
          {
              CurrentGold->SetText(FText::FromString(FString::FromInt(InventoryComp->GetGold())));
              //슬롯 하나씩 생성
              //슬롯에 아이템 정보와 인덱스를 넘김
              for (int i = 0; i < InventoryComp->GetTotalSlotNum(); ++i)
              {
                  //총 슬롯 갯수만큼 위젯 생성을 해야하고
                  //Init은 아이템 있는 자리만 해야한다.
                  UUserWidget *InventorySlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
                  SlotWidgets.Add(InventorySlotWidget);

                  UUniformGridSlot *GridSlot = SlotPanel->AddChildToUniformGrid(SlotWidgets[i], i / SlotsPerRow, i % SlotsPerRow);

                  if (InventoryComp->GetInventorySlots().IsValidIndex(i))
                  {
                      Cast<UInventorySlotWidget>(InventorySlotWidget)->Init(InventoryComp->GetInventorySlots()[i], i);
                      Cast<UInventorySlotWidget>(InventorySlotWidget)->UpdateSlot(true);
                  }
                  else
                  {
                      Cast<UInventorySlotWidget>(InventorySlotWidget)->UpdateSlot(false);
                  }
              }
          }
      }
  }
  
  ```
  
  ```c++
  void UStatBarWidget::NativeConstruct()
  {
      Super::NativeConstruct();

      if(GetOwningPlayerPawn())
          StatComp = Cast<UStatsComponent>(GetOwningPlayerPawn()->GetComponentByClass(UStatsComponent::StaticClass()));
      if(StatComp)
      {
          StatComp->OnPlusCurrentStatValue.AddDynamic(this, &ThisClass::OnStatValueUpdated);
      }
  }

  void UStatBarWidget::OnStatValueUpdated(EStats Stat, float Value)
  {
      if((StatType == Stat) && StatComp && StatBar)
      {
          float ResultValue = Value / StatComp->GetMaxValue(Stat);
          StatBar->SetPercent(ResultValue);
      }
  }
  
  
  ```

