#pragma once
#include <JuceHeader.h>
using namespace juce;


void initLogFile();
void writeToLogFile(String stringToLog);

String insertLineBreaks(String stringToInsert, int blockIndex);
void deleteLastLine(File* file);
void insertString(File* outputFile, int pointerNr, String stringToInsert);

int getNPCnrOfStringsFromAvatar(int pointerMapIdx);

int getNPCaddress(int NPCnr, bool absoluteAddress);
int getNPCStringsaddress(int NPCnr, bool absoluteAddress);

int getNPCTotalSize(int NPCnr);
int getNPCStringsSize(int NPCnr);


int getAvatarNPCresourcePointerTable();
int rewriteAvatarTopResourcePointerTable();

int copyAVATARFile(String destFileName, String srcFileName);

int PATCH_AVATAR_WITH_IMAGES();

