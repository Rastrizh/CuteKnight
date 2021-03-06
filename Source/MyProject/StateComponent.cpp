// Fill out your copyright notice in the Description page of Project Settings.

#include "StateComponent.h"
#include "PaperFlipbook.h"
#include "PaperCharacter.h"

// Sets default values for this component's properties
UStateComponent::UStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UStateComponent::SetFlipbook(class UPaperFlipbook* flipbook)
{
	SourceFlipbook = flipbook;
}

void UStateComponent::SetPreviousState(UStateComponent* prev_state)
{
	PreviousState = prev_state;
}

float UStateComponent::GetDuration() const
{
	return SourceFlipbook->GetTotalDuration() - SourceFlipbook->GetTotalDuration() / SourceFlipbook->GetFramesPerSecond();
}

void UStateComponent::SetElapsed(float e)
{
	elapsed = e;
}

void UStateComponent::ChangeState(FString state_name)
{
	SetName(state_name);
	SetElapsed(0);
}

// Called when the game starts
void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

