// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS(Abstract)
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* WalkAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = States)
	class UStateComponent* CharacterState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UPawnSensingComponent *Senses;

protected:
	UPROPERTY()
	FVector PlayerLocation;

public:
	AEnemy(const class FObjectInitializer& ObjectInitalizer = FObjectInitializer::Get());
		
	UFUNCTION()
	void OnOverlap(AActor* SelfActor, AActor* OtherActor);

	UFUNCTION()
	void OnSeePawn(APawn* pawn);
};
