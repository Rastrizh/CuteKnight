// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()

protected:
	virtual void Tick( float DeltaSeconds ) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* DeadAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* WalkAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = States)
	class UStateComponent* CharacterState;


	void Update(float delta_time) override;

public:
	AEnemy();
};
