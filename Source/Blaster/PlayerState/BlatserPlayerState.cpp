// Fill out your copyright notice in the Description page of Project Settings.


#include "BlatserPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"

void ABlatserPlayerState::AddToScore(float ScoreAmount)
{
	//Score += ScoreAmount;
	SetScore(GetScore() + ScoreAmount);

	Character =
		Character == nullptr ?
		Cast<ABlasterCharacter>(GetPawn()) :
		Character;

	if (Character)
	{
		Controller =
			Controller == nullptr ?
			Cast<ABlasterPlayerController>(Character->Controller) :
			Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}


void ABlatserPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character =
		Character == nullptr ?
		Cast<ABlasterCharacter>(GetPawn()) :
		Character;

	if (Character) 
	{
		Controller =
			Controller == nullptr ?
			Cast<ABlasterPlayerController>(Character->Controller) :
			Controller;
		if (Controller) 
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}
