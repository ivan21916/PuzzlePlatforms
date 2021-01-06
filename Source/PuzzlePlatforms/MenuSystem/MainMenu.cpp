// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"
#include "ServerRow.h"
#include "Components/TextBlock.h"

UMainMenu::UMainMenu() 
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
    if(ServerRowBPClass.Class == nullptr)
    {
        return;
    }
    ServerRowClass = ServerRowBPClass.Class;    
}

bool UMainMenu::Initialize() 
{
    bool Success = Super::Initialize();
    if(!Success)
    {   
        return false;
    }

    if(HostButton == nullptr)
    {   
        return false;
    }  
    HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

    if(CancelHostMenuButton == nullptr)
    {   
        return false;
    }  
    CancelHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

    if(HostGameButton == nullptr)
    {   
        return false;
    }
    HostGameButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

    if(JoinButton == nullptr)
    {   
        return false;
    }   
    JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

    if(QuitButton == nullptr)
    {   
        return false;
    }  
    QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

    if(CancelButton == nullptr)
    {   
        return false;
    }
    CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

    if(JoinGameButton == nullptr)
    {   
        return false;
    }
    JoinGameButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

    return true;
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames) 
{   
    ServerList->ClearChildren();

    uint32 i = 0;
    for(const FServerData& ServerData : ServerNames)
    {
        if(ServerRowClass == nullptr)
        {
            return;
        }

        UServerRow* Row = CreateWidget<UServerRow>(this, ServerRowClass);

        if(Row == nullptr)
        {
            return;
        }

        Row->ServerName->SetText(FText::FromString(ServerData.Name));
        Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
        FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
        Row->ConnectionFraction->SetText(FText::FromString(FractionText));
        Row->Setup(this, i);
        ++i;
        ServerList->AddChild(Row);  
    }
  
}

void UMainMenu::SelecteIndex(uint32 Index) 
{
    SelectedIndex = Index;
    UpdateChildren();
}

void UMainMenu::UpdateChildren() 
{
    for(int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
    {
        UServerRow* Row = Cast<UServerRow>(ServerList->GetChildAt(i));
        if(Row != nullptr)
        {
            Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
        }
    }
}

void UMainMenu::HostServer() 
{
    if(Menuinterface != nullptr)
    {   
        FString ServerName = ServerHostName->GetText().ToString();
        
        Menuinterface->Host(ServerName);
    }
}

void UMainMenu::JoinServer() 
{   
    if(SelectedIndex.IsSet() && Menuinterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
        Menuinterface->Join(SelectedIndex.GetValue());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));
    }

    // if(IPAdressField == nullptr)
    // {
    //     return;
    // }

    // const FString Address = IPAdressField->GetText().ToString();
}

void UMainMenu::OpenJoinMenu() 
{   
    if(MenuSwitcher == nullptr)
    {
        return;
    }

    if(JoinMenu == nullptr)
    {
        return;
    }

    MenuSwitcher->SetActiveWidget(JoinMenu);

    if(Menuinterface != nullptr)
    {
        Menuinterface->RefreshServerList();
    }
}

void UMainMenu::OpenMainMenu() 
{
    if(MenuSwitcher == nullptr)
    {
        return;
    }

    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenHostMenu() 
{
    MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::QuitPressed() 
{
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

    PlayerController->ConsoleCommand("quit");
}









