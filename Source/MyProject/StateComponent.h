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
	
	UPROPERTY(Category = State, EditAnywhere, meta = (DisplayThumbnail = "true"))
	FString StateName;

	UPROPERTY()
	float elapsed;

	UPROPERTY()
	UStateComponent* PreviousState;

public:	
	// Sets default values for this component's properties
	UStateComponent();

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	void SetName(FString name) { StateName = name; }
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	FString GetName() const { return StateName; }
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	void SetFlipbook(UPaperFlipbook* flipbook);

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	UPaperFlipbook* GetFlipbook() const { return SourceFlipbook; }

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	void SetPreviousState(UStateComponent* prev_state);
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	UStateComponent* GetPreviousState() const { return PreviousState; }

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	float GetDuration() const;

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	void SetElapsed(float e);

	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	float GetElapsed() const { return elapsed; }
	
	UFUNCTION(BlueprintCallable, Category = "Components|Flipbook")
	bool IsAnimationEnds() const { return elapsed >= GetDuration(); }

	bool operator==(const UStateComponent& rhs) const { return SourceFlipbook == rhs.GetFlipbook(); }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void Update(ACharacter&, float DeltaTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
