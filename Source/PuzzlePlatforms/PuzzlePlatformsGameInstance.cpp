// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"
#include "PuzzlePlatforms/PlatformTrigger.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

const static FName SESSION_NAME = TEXT("GameSession");
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

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

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if(Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if(SessionInterface.IsValid())
        {   
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
    }
    
    if(GEngine != nullptr)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);
    }
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

void UPuzzlePlatformsGameInstance::Host(FString ServerName) 
{   
    DesiredServerName = ServerName;
    if(SessionInterface.IsValid())
    {   
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if(ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else
        {
           CreateSession();
        }       
    }
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index) 
{   
    if(!SessionInterface.IsValid())
    {
        return;
    }

    if(!SessionSearch.IsValid())
    {
        return;
    }

    if(Menu != nullptr)
    {
        // Menu->SetServerList({"Test1", "Test2"});
        Menu->Teardown();
    }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

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

void UPuzzlePlatformsGameInstance::RefreshServerList() 
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if(SessionSearch.IsValid())
    {   
        // SessionSearch->bIsLanQuery = true;
        SessionSearch->MaxSearchResults = 100;
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
        UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
    }   
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success) 
{   
    if(!Success)
    {   
        UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
        return;
    }
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

    World->ServerTravel("/Game/Map/Lobby?listen");    
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success) 
{
    if(Success)
    {
        CreateSession();
    }
}


void UPuzzlePlatformsGameInstance::OnFindSessionComplete(bool Success) 
{   
    if(Success && SessionSearch.IsValid() && Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

        TArray<FServerData> ServerNames;

        for(const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found session names : %s"), *SearchResult.GetSessionIdStr());
            FServerData Data;
            Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
            Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
            Data.HostUsername = SearchResult.Session.OwningUserName;
            FString ServerName;
            if(SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
            {
                Data.Name = ServerName;
            }
            else
            {
                Data.Name = "Could not find name";
            }
            ServerNames.Add(Data);
        }
        Menu->SetServerList(ServerNames);
    }
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) 
{   
    if(!SessionInterface.IsValid())
    {
        return;
    }

    FString Address;
    if(!SessionInterface->GetResolvedConnectString(SessionName, Address))
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
        return;
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

void UPuzzlePlatformsGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString) 
{
    LoadMenu();
}



void UPuzzlePlatformsGameInstance::CreateSession() 
{   
    if(SessionInterface.IsValid())
    {
        FOnlineSessionSettings SessionSettings;
        // if(IOnlineSubsystem::Get()->GetSubsystemName().ToString() == TEXT("NULL"))
        // {
        //     SessionSettings.bIsLANMatch = true;
        // }
        // else
        // {
        SessionSettings.bIsLANMatch = true;
        SessionSettings.NumPublicConnections = 5;
        SessionSettings.bShouldAdvertise = true;
        SessionSettings.bUsesPresence = true;
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
        // }
    }
}

void UPuzzlePlatformsGameInstance::StartSession() 
{
    if(SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME);
    }
}
