// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Bat.generated.h"

class UPaperFlipbook;

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABat : public AEnemy
{
	GENERATED_BODY()
	
	virtual void Tick(float DeltaSeconds) override;

public:
	ABat();

	void Update(float delta_time) override;

};
