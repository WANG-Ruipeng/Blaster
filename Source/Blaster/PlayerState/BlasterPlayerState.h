// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Defeats();
	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	
	UPROPERTY(Replicated)
	bool bIsMutant = false;

	UPROPERTY()
	class ABlasterPlayerController* Controller;

private:
	UPROPERTY()
	class ABlasterCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

};
