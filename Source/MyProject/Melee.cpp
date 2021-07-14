// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee.h"
#include "Components/BoxComponent.h"

void AMelee::OnOverlap()
{
	box->DestroyComponent(true);
}

// Sets default values
AMelee::AMelee()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	box->SetBoxExtent({ 16.0f, 16.0f , 16.0f });
	box->bHiddenInGame = false;
	box->SetGenerateOverlapEvents(true);

	SetRootComponent(box);
}

// Called when the game starts or when spawned
void AMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_life_time += DeltaTime;

	if (m_life_time >= 0.2f || box->OnComponentBeginOverlap.IsBound())
		this->Destroy(true);
}

