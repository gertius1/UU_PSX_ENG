/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <string>
#include <iostream>

#include "stringsGeneral.h"
#include "stringsNPC.h"
#include "utilities.h"
#include "pointerMap.h"

using namespace juce;

int main(int argc, char* argv[])
{
	std::cout << "\n\nUnderworld PSX File Tools\n\n";
	std::cout << "Created with JUCE 6.1.2.\n\n\n";

	//initLogFile();

	int retVal = 0;

	//replace AVATAR with original version as a fresh start

	//retVal |= copyAVATARFile("AVATAR", "AVATAR_orig");

	retVal |= copyAVATARFile("AVATAR","AVATAR_extended46E800");
	retVal |= rewriteAvatarTopResourcePointerTable();	//adapt for ROM extension

	
	//////////////////////////////////////////////
	//////// General string blocks (1-24)
	//////////////////////////////////////////////

	//retVal |= CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN(); //Japanese
	
	retVal |= CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT();
	retVal |= CREATE_INDIVIDUAL_BIN_WITH_ATLAS();
	retVal |= PATCH_AVATAR_FROM_BIN_MANUALLY();
		
	//////////////////////////////////////
	////// NPC Blocks (3585 - 3898)
	//////////////////////////////////////
	
	////////////DEVELOPMENT/DEBUG
	/* 
	int NPC_BlockIndex = 3651;
	int JAP_FROM_BIN = true; //1: JAP from bin, 0: ENG from txt
	int startAtLineJAP = 0; //offset for Japanese strings, if ROM size not big enough, or later strings want to be rechecked quickly
	int endAtLineJAP = 0;
	int destinationSlotJAP = 0;
	int newNPCSize = 0;

	if (argc < 6)
	{
		return 1;
	}
	else
	{
		NPC_BlockIndex = atoi(argv[1]);
		JAP_FROM_BIN = atoi(argv[2]);
		startAtLineJAP = atoi(argv[3]);
		endAtLineJAP = atoi(argv[4]);
		destinationSlotJAP = atoi(argv[5]);
	}

	if (JAP_FROM_BIN != 0)
		retVal |= CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_NPC(NPC_BlockIndex, startAtLineJAP, endAtLineJAP, destinationSlotJAP);
	else
		retVal |= CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT_NPC(true, NPC_BlockIndex);
	retVal |= CREATE_INDIVIDUAL_BIN_WITH_ATLAS_NPC(NPC_BlockIndex);
	retVal |= PATCH_AVATAR_FROM_BIN_SINGLE_NPC(NPC_BlockIndex, &newNPCSize);

	int debugAddress = getNPCaddress(1, true);
	retVal |= PATCH_AVATAR_COPY_NPC_TO_ADDRESS(NPC_BlockIndex, debugAddress, newNPCSize);
	*/ 
	/////////////////END DEVELOPMENT/DEBUG




	/////////////// FINAL ASSEMBLY, ALL NPCS

	//CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_ALL_NPCS(); //Japanese

	CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT_NPC(false, 0);
	CREATE_ALL_BINS_WITH_ATLAS_NPC();
	PATCH_AVATAR_FROM_BIN_ALL_EXTENDED_NPCS();

	/////////////// END FINAL ASSEMBLY, ALL NPCS



	//////////////////////////////////////////
	////// Misc resources (images)
	//////////////////////////////////////////

	retVal |= PATCH_AVATAR_WITH_IMAGES();

    return retVal;
}
