// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "PuzzlePlatforms/PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) 
{
    Super::PostLogin(NewPlayer);
    ++NumberOfPlayers;

    if(NumberOfPlayers >= 2)
    {         
        GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 5, false);
    }
}

void ALobbyGameMode::Logout(AController* Exiting) 
{
    Super::Logout(Exiting);
    --NumberOfPlayers;
}

void ALobbyGameMode::StartGame() 
{   
    UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());
    if(GameInstance == nullptr)
    {
        return;
    }
    GameInstance->StartSession();

    UWorld* World = GetWorld();  
    if(World == nullptr)
    {
        return;
    }
    bUseSeamlessTravel = true;
    World->ServerTravel("/Game/Map/Game?listen");    
}