// Fill out your copyright notice in the Description page of Project Settings.


#include "Bat.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "StateComponent.h"

void ABat::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CharacterState->Update(this, DeltaSeconds);
}

ABat::ABat()
{
	IdleAnimation = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/Enemies/bat_sleeping.bat_sleeping'"), NULL, LOAD_None, NULL);
	DeadAnimation = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/Enemies/Bat_Hurt.Bat_Hurt'"), NULL, LOAD_None, NULL);
	WalkAnimation = LoadObject<UPaperFlipbook>(NULL, TEXT("PaperFlipbook'/Game/Enemies/Bat_fly.Bat_fly'"), NULL, LOAD_None, NULL);

	CharacterState->SetFlipbook(IdleAnimation);
	CharacterState->ChangeState(FString(TEXT("Walk")));
}

void ABat::Update(float delta_time)
{
	Super::Update(delta_time);

	if (CharacterState->GetName() == "Idle")
	{
		
	}
	else if (CharacterState->GetName() == "Dead")
	{
		if (CharacterState->IsAnimationEnds())
			this->Destroy(true);
	}
	else if (CharacterState->GetName() == "Walk")
	{
		const FVector velocity = GetVelocity();
		float TravelDirection = velocity.X;
		// Set the rotation so that the character faces his direction of travel.
		if (Controller != nullptr)
		{
			if (TravelDirection < 0.0f)
			{
				Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
			}
			else if (TravelDirection > 0.0f)
			{
				Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
			}
		}
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), TravelDirection);
	}
}
