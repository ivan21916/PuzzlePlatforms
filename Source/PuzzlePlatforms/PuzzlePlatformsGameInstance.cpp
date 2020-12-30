// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"
#include "PuzzlePlatforms/PlatformTrigger.h"

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer) 
{   
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
    if(MenuBPClass.Class == nullptr)
    {
        return;
    }
    MenuClass = MenuBPClass.Class;

    ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
    if(InGameMenuBPClass.Class == nullptr)
    {
        return;
    }
    InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init() 
{   
    UE_LOG(LogTemp, Warning, TEXT("Init"));
    UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *MenuClass->GetName());
}

void UPuzzlePlatformsGameInstance::LoadMenu() 
{   
    if(MenuClass == nullptr)
    {
        return;
    }
    Menu = CreateWidget<UMainMenu>(this, MenuClass);
    if(Menu == nullptr)
    {
        return;
    }

    Menu->Setup();
    Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::InGameLoadMenu() 
{
    if(InGameMenuClass == nullptr)
    {
        return;
    }
    UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);
    if(InGameMenu == nullptr)
    {
        return;
    }

    InGameMenu->Setup();
    InGameMenu->SetMenuInterface(this);   
}

void UPuzzlePlatformsGameInstance::Host() 
{   
    if(Menu != nullptr)
    {
        Menu->Teardown();
    }
    
    UEngine* Engine = GetEngine();
    if(Engine == nullptr)
    {
        return;
    }

    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

    UWorld* World = GetWorld();
    if(World == nullptr)
    {
        return;
    }

    World->ServerTravel("/Game/ThirdPersonBP/Maps/ThirdPersonExampleMap?listen");
}

void UPuzzlePlatformsGameInstance::Join(const FString &Address) 
{   
    if(Menu != nullptr)
    {
        Menu->Teardown();
    }

    UEngine* Engine = GetEngine();
    if(Engine == nullptr)
    {
        return;
    }

    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if(PlayerController == nullptr)
    {
        return;
    }

    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformsGameInstance::LoadMainMenu() 
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if(PlayerController == nullptr)
    {
        return;
    }

    PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

