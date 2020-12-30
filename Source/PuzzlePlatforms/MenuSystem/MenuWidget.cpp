// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"


void UMenuWidget::Setup() 
{
    this->AddToViewport();
    
    UWorld* World = GetWorld();
    if(World == nullptr)
    {
        return;
    }

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(PlayerController == nullptr)
    {
        return;
    }

    FInputModeUIOnly InputModeData;
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputModeData.SetWidgetToFocus(this->TakeWidget());
    PlayerController->SetInputMode(InputModeData);

    PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::Teardown() 
{
    this->RemoveFromViewport();
    
    UWorld* World = GetWorld();
    if(World == nullptr)
    {
        return;
    }

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(PlayerController == nullptr)
    {
        return;
    }

    FInputModeGameOnly InputModeData;
    PlayerController->SetInputMode(InputModeData);

    PlayerController->bShowMouseCursor = false;   
}

void UMenuWidget::SetMenuInterface(IMenuInterface* MenuInterface) 
{
    Menuinterface = MenuInterface;
}