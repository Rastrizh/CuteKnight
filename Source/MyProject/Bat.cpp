// Fill out your copyright notice in the Description page of Project Settings.


#include "Bat.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateComponent.h"

void ABat::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	this->Update(DeltaSeconds);
	GetSprite()->SetFlipbook(CharacterState->GetFlipbook());
}

ABat::ABat(const FObjectInitializer& ObjectInitializer)
	:
	Super(ObjectInitializer)
{
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation0;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation1;
		ConstructorHelpers::FObjectFinder<UPaperFlipbook> Animation2;

		FConstructorStatics()
			:
			Animation0(TEXT("PaperFlipbook'/Game/Enemies/bat_sleeping.bat_sleeping'")),
			Animation1(TEXT("PaperFlipbook'/Game/Enemies/Bat_Hurt.Bat_Hurt'")),
			Animation2(TEXT("PaperFlipbook'/Game/Enemies/Bat_fly.Bat_fly'"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	IdleAnimation = ConstructorStatics.Animation0.Object;
	DeadAnimation = ConstructorStatics.Animation1.Object;
	WalkAnimation = ConstructorStatics.Animation2.Object;

	GetSprite()->SetFlipbook(IdleAnimation);

	Cast<UCharacterMovementComponent>(GetMovementComponent())->SetMovementMode(MOVE_Flying, 0);

	CharacterState->SetFlipbook(IdleAnimation);
	CharacterState->ChangeState(FString(TEXT("Idle")));
}

void ABat::Update(float delta_time)
{
	if (CharacterState->GetName() == "Idle")
	{
		CharacterState->SetFlipbook(IdleAnimation);
	}
	else if (CharacterState->GetName() == "Dead")
	{
		CharacterState->SetElapsed(CharacterState->GetElapsed() + delta_time);

		if (CharacterState->IsAnimationEnds())
		{
			this->Destroy(true);
			CharacterState->SetElapsed(0.0f);
		}

		CharacterState->SetFlipbook(DeadAnimation);
	}
	else if (CharacterState->GetName() == "Walk")
	{
		
		auto direction = PlayerLocation - GetActorLocation();
		direction.Normalize();
		
		// Set the rotation so that the character faces his direction of travel.
		if (direction.X < 0.0f)
		{
			SetActorRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (direction.X > 0.0f)
		{
			SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), direction.X);
		CharacterState->SetFlipbook(WalkAnimation);
	}
}
