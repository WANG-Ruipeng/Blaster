// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();

}

void ABlasterGameMode::PrintAllPlayerState() 
{
	for (auto& It : PlayerStates)
	{
		if (It != nullptr)
		{
			FString PName = It->GetPlayerName();
			if (It->bIsMutant == true)
			{
				PName += FString("Mutant");
			}
			else 
			{
				PName += FString("Normal");
			}
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, PName);
		}
	}
}

void ABlasterGameMode::SetInitialMutant() 
{
	bInitialMutantSet = true;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* thisPlayerController = Cast<ABlasterPlayerController>(*It);
		ABlasterPlayerState* thisPlayerState = thisPlayerController->GetPlayerState<ABlasterPlayerState>();
		PlayerStates.Add(thisPlayerState);
	}

	int32 PlayerNum = PlayerStates.Num();
	int32 initialMutantIndex = FMath::RandHelper(PlayerNum);
	if (PlayerStates[initialMutantIndex]) 
	{
		PlayerStates[initialMutantIndex]->bIsMutant = true;
		ABlasterPlayerController* PlayerController = PlayerStates[initialMutantIndex]->Controller;
		if (PlayerController) 
		{
			PlayerController->SetHUDMutant(true);
		}
	}

}

void ABlasterGameMode::AddPointForMutants() 
{
	for (auto& It: PlayerStates)
	{
		if (It != nullptr) 
		{
			if (It->bIsMutant == true)
			{
				It->AddToScore(1.f);
			}
		}
	}
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
		if(HasAuthority() && !bInitialMutantSet)
			SetInitialMutant();
		//PrintAllPlayerState();
		if (HasAuthority() && FMath::Abs(lastAddCountdownTime - CountdownTime) >= 1) {
			AddPointForMutants();
			lastAddCountdownTime = CountdownTime;
		}
			
		
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}


void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState);
		}
	}
}


void ABlasterGameMode::PlayerEliminated(class ABlasterCharacter* ElimmedCharacter,class ABlasterPlayerController* VictimController,class ABlasterPlayerController* AttacterController)
{
	ABlasterPlayerState* AttackerPlayerState =
		AttacterController ?
		Cast<ABlasterPlayerState>(AttacterController->PlayerState) : 
		nullptr;

	ABlasterPlayerState* VictimPlayerState =
		VictimController ?
		Cast<ABlasterPlayerState>(VictimController->PlayerState) :
		nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		//AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);
		AttackerPlayerState->bIsMutant = false;
		AttacterController->SetHUDMutant(false);
	}

	if (VictimPlayerState) 
	{
		if(!VictimPlayerState->bIsMutant)
			VictimPlayerState->AddToDefeats(1);
		VictimPlayerState->bIsMutant = true;
		AttacterController->SetHUDMutant(true);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter) 
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*>PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController,PlayerStarts[Selection]);
	}
}
