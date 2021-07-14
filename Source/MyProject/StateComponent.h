// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

class UPaperFlipbook;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()
		
protected:

	UPROPERTY(Category = Sprite, EditAnywhere, meta = (DisplayThumbnail = "true"))
	class UPaperFlipbook* SourceFlipbook;

	UPROPERTY()
	float elapsed;

	UPROPERTY()
	UStateComponent* PreviousState;

public:	
	// Sets default values for this component's properties
	UStateComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual void SetFlipbook(UPaperFlipbook* flipbook);

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual UPaperFlipbook* GetFlipbook() const { return SourceFlipbook; }

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual void SetPreviousState(UStateComponent* prev_state);
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual UStateComponent* GetPreviousState() const { return PreviousState; }

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual float GetDuration() const;

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual void SetElapsed(float e);

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual float GetElapsed() const { return elapsed; }
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	virtual bool IsAnimationEnds() const { return elapsed >= GetDuration(); }

	bool operator==(const UStateComponent& rhs) const { return SourceFlipbook == rhs.GetFlipbook(); }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Update(ACharacter&, float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
