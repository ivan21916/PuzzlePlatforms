// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "Components/BoxComponent.h"
#include "Engine/TriggerVolume.h"
#include "MovingPlatform.h"

// Sets default values
APlatformTrigger::APlatformTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if(TriggerVolume != nullptr)
	{
		RootComponent = TriggerVolume;
		// TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnOverlapBegin);
	}
}

// Called when the game starts or when spawned
void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlatformTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if(TriggerVolume->IsOverlappingActor(ActorThatOpens))
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Activated"));
	// }

	TArray<AActor*> OverlappingActors;
	TriggerVolume->GetOverlappingActors(OverlappingActors);

	for(AActor* Actor : OverlappingActors)
	{
		if(IsOverlappingActor(Actor) && (Actor->GetName() == (TEXT("ThirdPersonCharacter_C_0"))))
		{	
			for(AMovingPlatform* Platform : PlatformsToTrigger)
			{
				Platform->AddActiveTrigger();
				UE_LOG(LogTemp, Error, TEXT("ActiveTrigger"));
			}
			UE_LOG(LogTemp, Warning, TEXT("%s is on the TriggerVolume!"), *Actor->GetName());
			
		}
		else if(IsOverlappingActor(Actor) && (Actor->GetName() == (TEXT("ThirdPersonCharacter_C_1"))))
		{
			for(AMovingPlatform* Platform : PlatformsToTrigger)
			{
				Platform->AddActiveTrigger();
				UE_LOG(LogTemp, Error, TEXT("ActiveTrigger"));
			}
			UE_LOG(LogTemp, Warning, TEXT("%s is on the TriggerVolume!"), *Actor->GetName());
		}
		else
		{
			for(AMovingPlatform* Platform : PlatformsToTrigger)
			{
				Platform->RemoveActiveTrigger();
				// UE_LOG(LogTemp, Warning, TEXT("DeActiveTrigger"));
			}			
		}
	}
// void APlatformTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Activated"));
}


