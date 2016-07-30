/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "RookUtils.h"
#include "Runtime/Engine/Public/AudioEffect.h"
#include "Runtime/Engine/Public/AudioDevice.h"

RookUtils::RookUtils(void) {
	SetUpEAXReverbMap();
	SetUpAcusticsMap();
}

RookUtils& RookUtils::Instance() {
	static RookUtils Utils;
	return Utils;
}

void RookUtils::SetUpEAXReverbMap() {
	EAXReverb.Add( EEAX::None, { 1.0f, 1.0f, 0.36f, 0.89f, 0.0f, 1.49f, 0.83f, 1.00f, 0.05f, 0.07f,{ 0.0f, 0.0f, 0.0f }, 1.26f, 0.011f,{ 0.0f, 0.0f, 0.0f }, 0.25f, 0.0f, 0.25f, 0.0f, 0.994f, 5000.0f, 250.0f, 0.0f, 0x1 } );
	EAXReverb.Add( EEAX::GenericEffect, EFX_REVERB_PRESET_GENERIC );
	EAXReverb.Add( EEAX::PaddedCell, EFX_REVERB_PRESET_PADDEDCELL );
	EAXReverb.Add( EEAX::Room, EFX_REVERB_PRESET_ROOM );
	EAXReverb.Add( EEAX::Bathroom, EFX_REVERB_PRESET_BATHROOM );
	EAXReverb.Add( EEAX::Livingroom, EFX_REVERB_PRESET_LIVINGROOM );
	EAXReverb.Add( EEAX::StoneRoom, EFX_REVERB_PRESET_STONEROOM );
	EAXReverb.Add( EEAX::Auditorium, EFX_REVERB_PRESET_AUDITORIUM );
	EAXReverb.Add( EEAX::ConcertHall, EFX_REVERB_PRESET_CONCERTHALL );
	EAXReverb.Add( EEAX::Cave, EFX_REVERB_PRESET_CAVE );
	EAXReverb.Add( EEAX::Arena, EFX_REVERB_PRESET_ARENA );
	EAXReverb.Add( EEAX::Hangar, EFX_REVERB_PRESET_HANGAR );
	EAXReverb.Add( EEAX::CarpetedHallway, EFX_REVERB_PRESET_CARPETEDHALLWAY );
	EAXReverb.Add( EEAX::Hallway, EFX_REVERB_PRESET_HALLWAY );
	EAXReverb.Add( EEAX::StoneCorridor, EFX_REVERB_PRESET_STONECORRIDOR );
	EAXReverb.Add( EEAX::Alley, EFX_REVERB_PRESET_ALLEY );
	EAXReverb.Add( EEAX::Forest, EFX_REVERB_PRESET_FOREST );
	EAXReverb.Add( EEAX::City, EFX_REVERB_PRESET_CITY );
	EAXReverb.Add( EEAX::Mountains, EFX_REVERB_PRESET_MOUNTAINS );
	EAXReverb.Add( EEAX::Quarry, EFX_REVERB_PRESET_QUARRY );
	EAXReverb.Add( EEAX::Plain, EFX_REVERB_PRESET_PLAIN );
	EAXReverb.Add( EEAX::ParkingLot, EFX_REVERB_PRESET_PARKINGLOT );
	EAXReverb.Add( EEAX::SewerPipe, EFX_REVERB_PRESET_SEWERPIPE );
	EAXReverb.Add( EEAX::Underwater, EFX_REVERB_PRESET_UNDERWATER );
	EAXReverb.Add( EEAX::Drugged, EFX_REVERB_PRESET_DRUGGED );
	EAXReverb.Add( EEAX::Dizzy, EFX_REVERB_PRESET_DIZZY );
	EAXReverb.Add( EEAX::Psychotic, EFX_REVERB_PRESET_PSYCHOTIC );
	EAXReverb.Add( EEAX::DustyRoom, EFX_REVERB_PRESET_DUSTYROOM );
	EAXReverb.Add( EEAX::Chapel, EFX_REVERB_PRESET_CHAPEL );
	EAXReverb.Add( EEAX::SmallWaterRoom, EFX_REVERB_PRESET_SMALLWATERROOM );
	EAXReverb.Add( EEAX::CastleSmallRoom, EFX_REVERB_PRESET_CASTLE_SMALLROOM );
	EAXReverb.Add( EEAX::CastleMediumRoom, EFX_REVERB_PRESET_CASTLE_MEDIUMROOM );
	EAXReverb.Add( EEAX::CastleLargeRoom, EFX_REVERB_PRESET_CASTLE_LARGEROOM );
	EAXReverb.Add( EEAX::CastleShortPassage, EFX_REVERB_PRESET_CASTLE_SHORTPASSAGE );
	EAXReverb.Add( EEAX::CastleLongPassage, EFX_REVERB_PRESET_CASTLE_LONGPASSAGE );
	EAXReverb.Add( EEAX::CastleHall, EFX_REVERB_PRESET_CASTLE_HALL );
	EAXReverb.Add( EEAX::CastleCupBoard, EFX_REVERB_PRESET_CASTLE_CUPBOARD );
	EAXReverb.Add( EEAX::CastleCourtyard, EFX_REVERB_PRESET_CASTLE_COURTYARD );
	EAXReverb.Add( EEAX::CastleAlcove, EFX_REVERB_PRESET_CASTLE_ALCOVE );
	EAXReverb.Add( EEAX::FactorySmallRoom, EFX_REVERB_PRESET_FACTORY_SMALLROOM );
	EAXReverb.Add( EEAX::FactoryMediumRoom, EFX_REVERB_PRESET_FACTORY_MEDIUMROOM );
	EAXReverb.Add( EEAX::FactoryLargeRoom, EFX_REVERB_PRESET_FACTORY_LARGEROOM );
	EAXReverb.Add( EEAX::FactoryShortPassage, EFX_REVERB_PRESET_FACTORY_SHORTPASSAGE );
	EAXReverb.Add( EEAX::FactoryLongPassage, EFX_REVERB_PRESET_FACTORY_LONGPASSAGE );
	EAXReverb.Add( EEAX::FactoryHall, EFX_REVERB_PRESET_FACTORY_HALL );
	EAXReverb.Add( EEAX::FactoryCupBoard, EFX_REVERB_PRESET_FACTORY_CUPBOARD );
	EAXReverb.Add( EEAX::FactoryCourtyard, EFX_REVERB_PRESET_FACTORY_COURTYARD );
	EAXReverb.Add( EEAX::FactoryAlcove, EFX_REVERB_PRESET_FACTORY_ALCOVE );
	EAXReverb.Add( EEAX::IcePalaceSmallRoom, EFX_REVERB_PRESET_ICEPALACE_SMALLROOM );
	EAXReverb.Add( EEAX::IcePalaceMediumRoom, EFX_REVERB_PRESET_ICEPALACE_MEDIUMROOM );
	EAXReverb.Add( EEAX::IcePalaceLargeRoom, EFX_REVERB_PRESET_ICEPALACE_LARGEROOM );
	EAXReverb.Add( EEAX::IcePalaceShortPassage, EFX_REVERB_PRESET_ICEPALACE_SHORTPASSAGE );
	EAXReverb.Add( EEAX::IcePalaceLongPassage, EFX_REVERB_PRESET_ICEPALACE_LONGPASSAGE );
	EAXReverb.Add( EEAX::IcePalaceHall, EFX_REVERB_PRESET_ICEPALACE_HALL );
	EAXReverb.Add( EEAX::IcePalaceCupBoard, EFX_REVERB_PRESET_ICEPALACE_CUPBOARD );
	EAXReverb.Add( EEAX::IcePalaceCourtyard, EFX_REVERB_PRESET_ICEPALACE_COURTYARD );
	EAXReverb.Add( EEAX::IcePalaceAlcove, EFX_REVERB_PRESET_ICEPALACE_ALCOVE );
	EAXReverb.Add( EEAX::SpaceStationSmallRoom, EFX_REVERB_PRESET_SPACESTATION_SMALLROOM );
	EAXReverb.Add( EEAX::SpaceStationMediumRoom, EFX_REVERB_PRESET_SPACESTATION_MEDIUMROOM );
	EAXReverb.Add( EEAX::SpaceStationLargeRoom, EFX_REVERB_PRESET_SPACESTATION_LARGEROOM );
	EAXReverb.Add( EEAX::SpaceStationShortPassage, EFX_REVERB_PRESET_SPACESTATION_SHORTPASSAGE );
	EAXReverb.Add( EEAX::SpaceStationLongPassage, EFX_REVERB_PRESET_SPACESTATION_LONGPASSAGE );
	EAXReverb.Add( EEAX::SpaceStationHall, EFX_REVERB_PRESET_SPACESTATION_HALL );
	EAXReverb.Add( EEAX::SpaceStationCupBoard, EFX_REVERB_PRESET_SPACESTATION_CUPBOARD );
	EAXReverb.Add( EEAX::SpaceStationAlcove, EFX_REVERB_PRESET_SPACESTATION_ALCOVE );
	EAXReverb.Add( EEAX::WoodenSmallRoom, EFX_REVERB_PRESET_WOODEN_SMALLROOM );
	EAXReverb.Add( EEAX::WoodenMediumRoom, EFX_REVERB_PRESET_WOODEN_MEDIUMROOM );
	EAXReverb.Add( EEAX::WoodenLargeRoom, EFX_REVERB_PRESET_WOODEN_LARGEROOM );
	EAXReverb.Add( EEAX::WoodenShortPassage, EFX_REVERB_PRESET_WOODEN_SHORTPASSAGE );
	EAXReverb.Add( EEAX::WoodenLongPassage, EFX_REVERB_PRESET_WOODEN_LONGPASSAGE );
	EAXReverb.Add( EEAX::WoodenHall, EFX_REVERB_PRESET_WOODEN_HALL );
	EAXReverb.Add( EEAX::WoodenCupBoard, EFX_REVERB_PRESET_WOODEN_CUPBOARD );
	EAXReverb.Add( EEAX::WoodenCourtyard, EFX_REVERB_PRESET_WOODEN_COURTYARD );
	EAXReverb.Add( EEAX::WoodenAlcove, EFX_REVERB_PRESET_WOODEN_ALCOVE );
	EAXReverb.Add( EEAX::SportEmptyStadium, EFX_REVERB_PRESET_SPORT_EMPTYSTADIUM );
	EAXReverb.Add( EEAX::SportFullStadium, EFX_REVERB_PRESET_SPORT_FULLSTADIUM );
	EAXReverb.Add( EEAX::SportStadiumTannoySpeaker, EFX_REVERB_PRESET_SPORT_STADIUMTANNOY );
	EAXReverb.Add( EEAX::SportSquashCourt, EFX_REVERB_PRESET_SPORT_SQUASHCOURT );
	EAXReverb.Add( EEAX::SportSmallSwimmingPool, EFX_REVERB_PRESET_SPORT_SMALLSWIMMINGPOOL );
	EAXReverb.Add( EEAX::SportLargeSwimmingPool, EFX_REVERB_PRESET_SPORT_LARGESWIMMINGPOOL );
	EAXReverb.Add( EEAX::SportGymnasium, EFX_REVERB_PRESET_SPORT_GYMNASIUM );
	EAXReverb.Add( EEAX::DomeTomb, EFX_REVERB_PRESET_DOME_TOMB );
	EAXReverb.Add( EEAX::DomeSaintPauls, EFX_REVERB_PRESET_DOME_SAINTPAULS );
	EAXReverb.Add( EEAX::PipeSmall, EFX_REVERB_PRESET_PIPE_SMALL );
	EAXReverb.Add( EEAX::PipeLarge, EFX_REVERB_PRESET_PIPE_LARGE );
	EAXReverb.Add( EEAX::PipeLongThin, EFX_REVERB_PRESET_PIPE_LONGTHIN );
	EAXReverb.Add( EEAX::PipeResonant, EFX_REVERB_PRESET_PIPE_RESONANT );
	EAXReverb.Add( EEAX::OutdoorsBackyard, EFX_REVERB_PRESET_OUTDOORS_BACKYARD );
	EAXReverb.Add( EEAX::OutdoorsRollingPlains, EFX_REVERB_PRESET_OUTDOORS_ROLLINGPLAINS );
	EAXReverb.Add( EEAX::OutdoorsDeepCanyon, EFX_REVERB_PRESET_OUTDOORS_DEEPCANYON );
	EAXReverb.Add( EEAX::OutdoorsCreek, EFX_REVERB_PRESET_OUTDOORS_CREEK );
	EAXReverb.Add( EEAX::OutdoorsValley, EFX_REVERB_PRESET_OUTDOORS_VALLEY );
	EAXReverb.Add( EEAX::MoodHeaven, EFX_REVERB_PRESET_MOOD_HEAVEN );
	EAXReverb.Add( EEAX::MoodHell, EFX_REVERB_PRESET_MOOD_HELL );
	EAXReverb.Add( EEAX::MoodMemory, EFX_REVERB_PRESET_MOOD_MEMORY );
	EAXReverb.Add( EEAX::DrivingCommentator, EFX_REVERB_PRESET_DRIVING_COMMENTATOR );
	EAXReverb.Add( EEAX::DrivingPitGarage, EFX_REVERB_PRESET_DRIVING_PITGARAGE );
	EAXReverb.Add( EEAX::DrivingInCarRacer, EFX_REVERB_PRESET_DRIVING_INCAR_RACER );
	EAXReverb.Add( EEAX::DrivingInCarSports, EFX_REVERB_PRESET_DRIVING_INCAR_SPORTS );
	EAXReverb.Add( EEAX::DrivingInCarLuxury, EFX_REVERB_PRESET_DRIVING_INCAR_LUXURY );
	EAXReverb.Add( EEAX::DrivingFullGrandstand, EFX_REVERB_PRESET_DRIVING_FULLGRANDSTAND );
	EAXReverb.Add( EEAX::DrivingEmptyGrandstand, EFX_REVERB_PRESET_DRIVING_EMPTYGRANDSTAND );
	EAXReverb.Add( EEAX::DrivingTunnel, EFX_REVERB_PRESET_DRIVING_TUNNEL );
	EAXReverb.Add( EEAX::CityStreet, EFX_REVERB_PRESET_CITY_STREETS );
	EAXReverb.Add( EEAX::CitySubway, EFX_REVERB_PRESET_CITY_SUBWAY );
	EAXReverb.Add( EEAX::CityMuseum, EFX_REVERB_PRESET_CITY_MUSEUM );
	EAXReverb.Add( EEAX::CityLibrary, EFX_REVERB_PRESET_CITY_LIBRARY );
	EAXReverb.Add( EEAX::CityUnderpass, EFX_REVERB_PRESET_CITY_UNDERPASS );
	EAXReverb.Add( EEAX::CityAbandoned, EFX_REVERB_PRESET_CITY_ABANDONED );
}

void RookUtils::SetUpAcusticsMap() {
	STC.Add( "PHM_Gypsum", 25 );
	STC.Add( "PHM_Glass", 27 );
	STC.Add( "PHM_Wood", 30 );
	STC.Add( "PHM_Masonry", 40 );
	STC.Add( "PHM_Clay", 45 );
	STC.Add( "PHM_Brick", 51 );
	STC.Add( "PHM_Concrete", 55 );
	STC.Add( "PHM_Steel", 57 );

	SAC.Add( "PHM_Gypsum", 0.05f );
	SAC.Add( "PHM_Glass", 0.02f );
	SAC.Add( "PHM_Wood", 0.1f );
	SAC.Add( "PHM_Masonry", 0.05f );
	SAC.Add( "PHM_Brick", 0.07f );
	SAC.Add( "PHM_Concrete", 0.1f );
}

float RookUtils::CalculateSTC( float MeshDepth, float MeshSize ) {
	return 13.4f + 11.4* ( log10( MeshSize ) ) + 0.0826*MeshDepth;
}

float RookUtils::AvarageSAC( float MeshDepth ) {
	return 0.073f + 0.0001*MeshDepth;
}

bool RookUtils::InSpehereRadius( FVector SourceLocation, FVector TargetLocation, float Radius ) const {
	return ( FMath::Pow( ( SourceLocation.X - TargetLocation.X ), 2.0f ) + FMath::Pow( ( SourceLocation.Y - TargetLocation.Y ), 2.0f ) + FMath::Pow( ( SourceLocation.Z - TargetLocation.Z ), 2.0f ) ) <= Radius * Radius;
}

UWorld* RookUtils::GetWorld( TEnumAsByte<EWorldType::Type> WorldType ) const {
	if ( !GEngine )
		return nullptr;

	const TIndirectArray<FWorldContext>& Contextes = GEngine->GetWorldContexts();

	for ( FWorldContext TemporaryContext : Contextes ) {
		if ( TemporaryContext.WorldType == WorldType ) {
			return TemporaryContext.World();
		}		
	}

	//If previouse world type was not catched we are returning Play in Editor or Editor UWorld
	for ( FWorldContext TemporaryContext : Contextes ) {
		if ( TemporaryContext.WorldType == EWorldType::PIE || TemporaryContext.WorldType == EWorldType::Editor ) {
			return TemporaryContext.World();
		}
	}
	return nullptr;
}

uint32 RookUtils::GetUniqueID() {
	CurrentUniqeID++;
	return CurrentUniqeID;
}

void RookUtils::SetReverbInUnreal( EEAX NewReverb ) {
	FAudioDevice* AudioDevice = GEngine->GetMainAudioDevice();

	if ( AudioDevice ) {
		if ( NewReverb != EEAX::None ) {			
			RemoveUnrealReverb();
			PreviouseUnrealReverb = static_cast<int>( NewReverb );
			EFXEAXREVERBPROPERTIES* OALReverbProperites = &EAXReverb[NewReverb];

			if ( !UnrealReverb.IsValid() ) {
				UnrealReverb = NewObject<UReverbEffect>( GetWorld() );
			}

			UnrealReverb->AirAbsorptionGainHF = OALReverbProperites->flAirAbsorptionGainHF;
			UnrealReverb->DecayHFRatio = OALReverbProperites->flDecayHFRatio;
			UnrealReverb->DecayTime = OALReverbProperites->flDecayTime;
			UnrealReverb->Density = OALReverbProperites->flDensity;
			UnrealReverb->Diffusion = OALReverbProperites->flDiffusion;
			//TODO: make parameter
			UnrealReverb->Gain = 1.0f;
			UnrealReverb->GainHF = OALReverbProperites->flGainHF;
			UnrealReverb->LateDelay = OALReverbProperites->flLateReverbDelay;
			UnrealReverb->LateGain = OALReverbProperites->flLateReverbGain;
			UnrealReverb->ReflectionsDelay = OALReverbProperites->flReflectionsDelay;
			UnrealReverb->ReflectionsGain = OALReverbProperites->flReflectionsGain;
			UnrealReverb->RoomRolloffFactor = OALReverbProperites->flRoomRolloffFactor;

			FActivatedReverb UnrealActiveReverb;
			UnrealActiveReverb.Priority = 10.0f;
			UnrealActiveReverb.ReverbSettings.bApplyReverb = true;
			UnrealActiveReverb.ReverbSettings.ReverbEffect = UnrealReverb.Get();

			AudioDevice->ActivatedReverbs.Add( *( FString::FromInt( PreviouseUnrealReverb ) ), UnrealActiveReverb );
			AudioDevice->ActivateReverbEffect( UnrealReverb.Get(), *( FString::FromInt(PreviouseUnrealReverb ) ), 11.0f, 0.25f, 0.0f );

			OALReverbProperites = nullptr;
		} else {
			RemoveUnrealReverb();			
		}
		AudioDevice = nullptr;
	}
}

void RookUtils::RemoveUnrealReverb() {
	FAudioDevice* AudioDevice = GEngine->GetMainAudioDevice();
	AudioDevice->DeactivateReverbEffect( *( FString::FromInt( PreviouseUnrealReverb ) ) );
	if ( AudioDevice->ActivatedReverbs.Contains( *( FString::FromInt(PreviouseUnrealReverb ) ) ) )
		AudioDevice->ActivatedReverbs.Remove( *( FString::FromInt(PreviouseUnrealReverb ) ) );
}

float RookUtils::DecibelsToVolume( float dB ) {
	return FMath::Pow( 10, dB / 20 ) *100.0f;
}

float RookUtils::VolumeToDecibels( float Volume ) {
	return 20 * FMath::LogX( 10, Volume );
}

void RookUtils::CleanData() {
	UnrealReverb = nullptr;
	EAXReverb.Empty();
	SAC.Empty();
	STC.Empty();
}