// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize() 
{
    bool Success = Super::Initialize();
    if(!Success)
    {   
        return false;
    }

    if(CancelButton == nullptr)
    {   
        return false;
    }
    
    CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelPressed);

    if(QuitButton == nullptr)
    {   
        return false;
    }
    
    QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);


    return true;
}

void UInGameMenu::CancelPressed() 
{
    Teardown();
}

void UInGameMenu::QuitPressed() 
{
    if(Menuinterface != nullptr)
    {   
        // Teardown();
        Menuinterface->LoadMainMenu();
    }
}