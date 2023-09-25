#include "utilities.h"
#include "pointerMap.h"

// create and open output file
File logFile;

void initLogFile()
{
	String logPath;

	logPath.append("/logfile.txt", 30);

	String filename = File::getCurrentWorkingDirectory().getFullPathName();
	filename.append(logPath, 40);

	logFile = File::getCurrentWorkingDirectory().getChildFile(filename.getCharPointer());

	if (logFile.existsAsFile())
		logFile.deleteFile();

	logFile.create();

}

void writeToLogFile(String stringToLog)
{
	logFile.appendText(stringToLog);
	logFile.appendText(String("\n"));
}

int copyAVATARFile(String destFileName, String srcFileName)
{
	//copies srcFileName (a blank version) over destFileName (the working copy)
	File AVATAR_dest = File::getCurrentWorkingDirectory().getChildFile(destFileName);
	if (!AVATAR_dest.existsAsFile())
		return 1;  // file doesn't exist

	File AVATAR_src = File::getCurrentWorkingDirectory().getChildFile(srcFileName);
	if (!AVATAR_src.existsAsFile())
		return 1;  // file doesn't exist


	AVATAR_src.copyFileTo(AVATAR_dest);

	return 0;
}

String insertLineBreaks(String stringToInsert, int blockIndex)
{
	

	String stringWithLineBreaks;
	int windowSizeNrOfCharacters = 0, windowSizeNrOfCharactersSecondLine = 0;

	if (blockIndex == 3) // Readables
		windowSizeNrOfCharacters = 26;
	else if (blockIndex == 8) // Signs
		windowSizeNrOfCharacters = 26;
	else if (blockIndex == 9) // World description
		windowSizeNrOfCharacters = 26;
	else if (blockIndex >= 3585) //Dialog
		windowSizeNrOfCharacters = 26;
	else
		return stringToInsert;

	if (blockIndex >= 3585) //Dialog
		windowSizeNrOfCharactersSecondLine = 28;
	else
		windowSizeNrOfCharactersSecondLine = windowSizeNrOfCharacters;

	if (stringToInsert.length() < windowSizeNrOfCharacters)
		return stringToInsert;

	//Algorithm: iterate over string, collect each single character in tempBuf
	// at each empty space, dot or comma check if over windowSizeNrOfCharacters; if so, add \n before adding tempBuf to outString in next line
	// else only add tempBuf

	int prevBreakCharPos = 0, breakCharPos = 0;
	String tempString;

	for (int i = 0; i < stringToInsert.length(); i++)
	{
		String character = stringToInsert.substring(i,i+1);
		tempString.append(character, 3);

		if (character.containsAnyOf(" ,.-?!"))
		{
			if (breakCharPos > windowSizeNrOfCharacters)
			{		
				// remove space when inserting linebreak
				if ((stringWithLineBreaks.getLastCharacters(1)).containsAnyOf(" "))
					stringWithLineBreaks = stringWithLineBreaks.dropLastCharacters(1); 

				// also remove other chars when at end of line when inserting linebreak
				if ((stringWithLineBreaks.getLastCharacters(1)).containsAnyOf(",.-!")
					&&(prevBreakCharPos == windowSizeNrOfCharacters))
					stringWithLineBreaks = stringWithLineBreaks.dropLastCharacters(1); 

						stringWithLineBreaks.append("\\n", 3);
				breakCharPos = -1; //will be incremented to 0 at end
				
				i -= tempString.length();
				windowSizeNrOfCharacters = windowSizeNrOfCharactersSecondLine; //increase line width after first break, bc there is no more numbering in PC window
			}
			else
			{
				//special case for last character being put into new line. 
				//because it´s at the very end it won´t be caught by instrucions above
				if ((i == stringToInsert.length()-1)
					&& (tempString.getLastCharacters(1)).containsAnyOf(",.-!?")
					&& (breakCharPos == windowSizeNrOfCharacters))
					tempString = tempString.dropLastCharacters(1);

				stringWithLineBreaks.append(tempString, tempString.length()); //only append here, for other case will be appended in newline by iteration i
			}
			tempString.clear(); //clear in both cases, will be refilled by setting back i

			prevBreakCharPos = breakCharPos;
		}
		breakCharPos++;

		if (i== stringToInsert.length() - 1) //add last part of string even if there is no breakChar
			stringWithLineBreaks.append(tempString, tempString.length());
	}
	return stringWithLineBreaks;
}


void deleteLastLine(File* file)
{
	//very slow function, avoid if possible 

	StringArray destLines;
	file->readLines(destLines);

	file->replaceWithText(destLines[0]);
	file->appendText("\n");

	for (int i = 1; i < destLines.size() - 2; i++)
	{
		file->appendText(destLines[i]);
		file->appendText("\n");
	}
}


String prevString = " ";

void insertString(File* outputFile, int pointerNr, String stringToInsert)
{
	// strip redundant data to save size:
	// if consecutive lines are the same, only keep most recent one.
	// in practice that means, we have to go back and delete the string we last wrote

	if (pointerNr >= 2)
	{
		if (true) //set true for adding ATLAS pointers
		{
			outputFile->appendText("#W16(");
			outputFile->appendText(String::formatted("%i", pointerNr - 2));
			outputFile->appendText(")\n");
		}

		if (stringToInsert == prevString)
			outputFile->appendText("\n");
		else
			outputFile->appendText(prevString);
	}
	prevString = stringToInsert;
}

int getNPCnrOfStringsFromAvatar(int nrOfNPC)
{
	// CREATE INFILE
	File AVATAR_orig = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR_orig");
	if (!AVATAR_orig.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatarOrig(AVATAR_orig); // [2]
	if (!inStreamAvatarOrig.openedOk())
		return 1;  // failed to open

	const int BASE_ADDRESS_NPC = 0x3A2000;
	const int POINTER_SIZE = 4;

	inStreamAvatarOrig.setPosition(BASE_ADDRESS_NPC);
	int nrNPCs = inStreamAvatarOrig.readInt();

	if (nrOfNPC > nrNPCs)
		return -1;

	int ptr = BASE_ADDRESS_NPC;


	ptr += (POINTER_SIZE * (nrOfNPC + 1));
	inStreamAvatarOrig.setPosition(ptr);
	int NPCaddress = inStreamAvatarOrig.readInt() + BASE_ADDRESS_NPC;

	inStreamAvatarOrig.setPosition(NPCaddress);
	int stringTableAddress = inStreamAvatarOrig.readInt() + NPCaddress;

	inStreamAvatarOrig.setPosition(stringTableAddress);
	short nrOfLines = inStreamAvatarOrig.readShort();

	return nrOfLines;
	

}

int getNPCaddress(int NPCnr, bool absoluteAddress)
{
	//returns address for single NPC struct
	//absoluteAdress adds  BASE_ADDRESS_NPC = 0x3A2000 to result
	if (NPCnr > nrOfNPCs)
		return -1;

	// CREATE INFILE
	File AVATAR_orig = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR_orig");
	if (!AVATAR_orig.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatarOrig(AVATAR_orig); // [2]
	if (!inStreamAvatarOrig.openedOk())
		return 1;  // failed to open

	const int BASE_ADDRESS_NPC = 0x3A2000;
	const int POINTER_SIZE = 4;

	int NPCpos = BASE_ADDRESS_NPC + POINTER_SIZE * (NPCnr + 1);

	inStreamAvatarOrig.setPosition(NPCpos);
	int NPCaddress1 = inStreamAvatarOrig.readInt();

	if (absoluteAddress)
		NPCaddress1 += BASE_ADDRESS_NPC;

	return NPCaddress1;
}

int getNPCStringsaddress(int NPCnr, bool absoluteAddress)
{
	//returns address for single NPC strings position (one short further, not reading nr of pointers, but starting at first pointer)
	//absoluteAdress adds  BASE_ADDRESS_NPC = 0x3A2000 to result

	if (NPCnr > nrOfNPCs)
		return -1;

	// CREATE INFILE
	File AVATAR_orig = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR_orig");
	if (!AVATAR_orig.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatarOrig(AVATAR_orig); // [2]
	if (!inStreamAvatarOrig.openedOk())
		return 1;  // failed to open

	const int BASE_ADDRESS_NPC = 0x3A2000;
	const int POINTER_SIZE = 4;

	int NPCpos = BASE_ADDRESS_NPC + POINTER_SIZE * (NPCnr + 1);

	inStreamAvatarOrig.setPosition(NPCpos);
	int NPCaddress = inStreamAvatarOrig.readInt();

	inStreamAvatarOrig.setPosition(BASE_ADDRESS_NPC + NPCaddress);
	int startOfPointerTable = inStreamAvatarOrig.readInt();
	startOfPointerTable += 2; //start at first pointer, not at size word

	if (absoluteAddress)
		startOfPointerTable += BASE_ADDRESS_NPC + NPCaddress;

	return startOfPointerTable;
}

int getNPCTotalSize(int NPCnr)
{
	//returns total available size for single NPC 
	//NPCaddress1 - NPCaddress2
	//pointer Table has one last entry pointing to next block after NPCs, so that calculation can be used also for last NPC

	if (NPCnr > nrOfNPCs)
		return -1;

	int totalSizeNPC = getNPCaddress(NPCnr+1, false) - getNPCaddress(NPCnr, false);

	return totalSizeNPC;
}

int getNPCStringsSize(int NPCnr)
{
	//returns size of strings section (including pointer table) for single NPC 
	// totalSize = NPCaddress1 - NPCaddress2
	// pointer Table has one last entry pointing to next block after NPCs, so that calculation can be used also for last NPC
	// stringSectionSize = totalSize - startOfPointerTable

	if (NPCnr > nrOfNPCs)
		return -1;

	return getNPCTotalSize(NPCnr) - getNPCStringsaddress(NPCnr, false);
}


int rewriteAvatarTopResourcePointerTable()
{
	/*	This function adds an offset to the memory addressess in the AVATAR file at the very top.
		It´s purpose is to adapt the table, after manually inserting empty ROM space somewhere.
		It did not work to well around the area of the general strings (0xB0000), and led to 
		the fight mode crashing (maybe loading the weapons texture failed?). There seem to be various checks
		in place, that crash the game, when the space between resource pointers is weird.
		Also one resource pointer cannot be to a larger address than the one following it
	*/


	// CREATE INFILE
	File AVATAR_orig = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR_orig");
	if (!AVATAR_orig.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatarOrig(AVATAR_orig); // [2]
	if (!inStreamAvatarOrig.openedOk())
		return 1;  // failed to open

	// CREATE OUTFILE
	File AVATAR = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR");
	if (!AVATAR.existsAsFile())
		return 1;  // file doesn't exist
	FileOutputStream outStreamAvatar(AVATAR); // [2]
	if (!outStreamAvatar.openedOk())
		return 1;  // failed to open

	//write definitions
	const int stringTablePosition = 0x20 / 4;
	const int endOfNPCTablePosition = 0xB0 / 4;

	int positionA = stringTablePosition + 1; //start adding memOffset from this position
	int memOffsetA = 0x2000; //add this memOffset

	int positionB = endOfNPCTablePosition; //start adding memOffset from this position
	int memOffsetB = 0x100000; //add this memOffset

	for (int i = 0; i < 231; i++)
	{
		inStreamAvatarOrig.setPosition(4 * i);
		int address = inStreamAvatarOrig.readInt();

		//if (i >= positionA)
		//	address += memOffsetA;

		if (i >= positionB)
			address += memOffsetB;

		outStreamAvatar.setPosition(4 * i);
		outStreamAvatar.writeInt(address);
	}
	return 0;
}




int getAvatarNPCresourcePointerTable()
{
	// CREATE INFILE
	File AVATAR_orig = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR_orig");
	if (!AVATAR_orig.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatarOrig(AVATAR_orig); // [2]
	if (!inStreamAvatarOrig.openedOk())
		return 1;  // failed to open

	const int BASE_ADDRESS_NPC = 0x3A2000;
	const int POINTER_SIZE = 4;

	inStreamAvatarOrig.setPosition(BASE_ADDRESS_NPC);
	int nrNPCs = inStreamAvatarOrig.readInt();

	int ptr = BASE_ADDRESS_NPC;

	for (int i = 0; i < nrNPCs; i++)
	{
		ptr += POINTER_SIZE;
		inStreamAvatarOrig.setPosition(ptr);
		int NPCaddress = inStreamAvatarOrig.readInt() + BASE_ADDRESS_NPC;

		inStreamAvatarOrig.setPosition(NPCaddress);
		int stringTableAddress = inStreamAvatarOrig.readInt() + NPCaddress;

		inStreamAvatarOrig.setPosition(stringTableAddress);
		short nrOfLines = inStreamAvatarOrig.readShort();

		// Log StringNumber per NPC
		String logMsg;
		logMsg.append(String::formatted("NPC#  %4i ", i), 20);
		logMsg.append(String::formatted("Lines %i", nrOfLines), 20);
		writeToLogFile(logMsg);

	}
	return 0;
}

std::map<String, int> resourceMap
{
	//	filename,				startAddress,
	{	"AVATAR[7].tim",		0x318800},
	{	"AVATAR[9].tim",		0x32E800},
	{	"AVATAR[10].tim",		0x33F000},
	{	"AVATAR[12].tim",		0x358800}
};

int getStartAddressFromResourceName(String resourceName)
{
	return resourceMap.find(resourceName)->second;
}


int PATCH_AVATAR_WITH_IMAGES()
{

	std::cout << "PATCH_AVATAR_WITH_IMAGES\n";

	// CREATE OUTFILE
	File AVATAR = File::getCurrentWorkingDirectory().getChildFile("AVATAR");
	if (!AVATAR.existsAsFile())
		return 1;  // file doesn't exist
	FileOutputStream outStreamAvatar(AVATAR); // [2]
	if (!outStreamAvatar.openedOk())
		return 1;  // failed to open

	// CREATE INFILES

	String directoryName = ("..\\..\\..\\DATA\\Edited TIM\\");
	File directory = File::getCurrentWorkingDirectory().getChildFile(directoryName.getCharPointer());
	Array< File > imageFiles = directory.findChildFiles(2,false,"*.tim");
	int nrOfFiles = directory.getNumberOfChildFiles(2, "*.tim");


	for (int i = 0; i < nrOfFiles; i++)
	{
		int startAddress = getStartAddressFromResourceName(imageFiles[i].getFileName());

		FileInputStream inStream(imageFiles[i]); // [2]
		if (!inStream.openedOk())
			return 1;  // failed to open

		std::cout << "\n" << "InFile: " << imageFiles[i].getFileName() << "\n";

		std::cout << "FileSize: " << imageFiles[i].getSize() << "\n";

		std::cout << "startAddress: " << startAddress << "\n";

		//debug, only show stdout info
		//return 0;

		//write contents of bin file
		outStreamAvatar.setPosition(startAddress);
		outStreamAvatar.writeFromInputStream(inStream, imageFiles[i].getSize());

	}
	return 0;
}