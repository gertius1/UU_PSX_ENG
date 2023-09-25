#pragma once
#include <JuceHeader.h>
using namespace juce;


int CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_NPC(int inputNdx, int startAtLine, int endAtLine, int destinationSlot);


int CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_ALL_NPCS();
int CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT_NPC(bool processSingleNPC, int inputNdx);
int CREATE_INDIVIDUAL_BIN_WITH_ATLAS_NPC(int blockIndex);
int PATCH_AVATAR_FROM_BIN_SINGLE_NPC(int blockIndex, int* outNewNPCSize);
int PATCH_AVATAR_COPY_NPC_TO_ADDRESS(int inNPCblockIndex, int inAddress, int inNewNPCSize);


int CREATE_ALL_BINS_WITH_ATLAS_NPC();
int PATCH_AVATAR_FROM_BIN_ALL_EXTENDED_NPCS();