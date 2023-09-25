#include "stringsGeneral.h"
#include "stringsNPC.h"
#include "pointerMap.h"
#include "utilities.h"

void insertStringNPC(File* outputFile, int pointerNr, String stringToInsert)
{
	   	outputFile->appendText("#W16(");
		outputFile->appendText(String::formatted("%i", pointerNr));
		outputFile->appendText(")\n");
		
		outputFile->appendText(stringToInsert);

}

int CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_ALL_NPCS()
{
	for (int i = 0; i < nrOfNPCs; i++)
	{
		int blockIndex = getBlockIndexFromNPCnr(i);
		CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_NPC(blockIndex, 0, 255, 0);
	}

	return 0;
}

int CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN_NPC(int blockIndex, int startAtLine, int endAtLine, int destinationSlot)
{
	//int blockIndex: NPC Index
	//int startAtLine, int endAtLin: Strings to be included
	//int destinationSlot: where to place all strings (not all NPC begin conversiation in slot 1)

	std::cout << "CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN\n";



	// CREATE INFILE
	File AVATAR_orig = File::getCurrentWorkingDirectory()
		.getChildFile("AVATAR_orig");
	if (!AVATAR_orig.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatar(AVATAR_orig); // [2]
	if (!inStreamAvatar.openedOk())
		return 1;  // failed to open

	// CREATE OUTFILE
	// create and open output file
	String destination = "/outputNPC/UU_Block_";
	destination.append(String(blockIndex),4);
	destination.append(".txt",20);


	String filename = File::getCurrentWorkingDirectory().getFullPathName();
	filename.append(destination, 40);

	const int POINTER_TABLE_START = getNPCStringsaddress(getNPCnrFromBlockIndex(blockIndex), true) - 2;
	
	const int POINTER_SIZE = 0x02;

	int ptr = POINTER_TABLE_START + 2; //move to table length info at start
	inStreamAvatar.setPosition(ptr);
	int OFFSET = inStreamAvatar.readShort();

	const int POINTER_TABLE_STOP = POINTER_TABLE_START + OFFSET;
	const int BASE_POINTER = POINTER_TABLE_STOP;


	File outputFile;
	outputFile = File::getCurrentWorkingDirectory().getChildFile(filename.getCharPointer());
	outputFile.create();
	// write atlas header
	outputFile.replaceWithText("#VAR(dialogue, TABLE)\n");
	outputFile.appendText("#ADDTBL(\"ascii.tbl\", dialogue)\n");
	outputFile.appendText("#ACTIVETBL(dialogue)\n");
	outputFile.appendText(String::formatted("#HDR($%08X)\n", -2));
	outputFile.appendText(String::formatted("#JMP($%08X)\n\n", POINTER_TABLE_STOP- POINTER_TABLE_START - 2));
	
	for (int i = 0; i < destinationSlot+1; i++)
		outputFile.appendText(String::formatted("#W16(%i)\n",i*2)); //strings will land at destinationSlot entry

	std::cout << "InFile: " << "AVATAR_orig\n";
	std::cout << "OutFile: " << destination << "\n";

	inStreamAvatar.setPosition(ptr);
	int ptrAddress = inStreamAvatar.readShort() - OFFSET + BASE_POINTER;
	int ptrAddressPrev = 0;
	int bytesToRead = 0;
	const int READ_BUFFER_SIZE = 1024;
	unsigned char readBuffer[READ_BUFFER_SIZE];
	int stringCounter = 0;
	String inString;
	int readBytes = 0;

	std::cout << "Processing... \n";
	
	while (ptr < POINTER_TABLE_STOP - POINTER_SIZE)
	{
		ptrAddressPrev = ptrAddress;
		ptr += POINTER_SIZE;

		
		inStreamAvatar.setPosition(ptr);
		
		ptrAddress = inStreamAvatar.readShort();
		ptrAddress -= OFFSET;
		ptrAddress += BASE_POINTER;
		bytesToRead = ptrAddress - ptrAddressPrev;

		if ((stringCounter < startAtLine - 1) || (stringCounter > endAtLine -1))
		{
			stringCounter++;
			continue;
		}
		
		if (bytesToRead > READ_BUFFER_SIZE)
		{
			std::cout << "ERROR: readBuffer too small, aborting... \n";
			return 1;
		}

		inStreamAvatar.setPosition(ptrAddressPrev);
		readBytes = inStreamAvatar.read(readBuffer, bytesToRead);
		

		inString.append(String::formatted("%03i: ", stringCounter+1), 8);

		//drop 1F at end for translation
		readBytes = readBytes - 1;

		//append bytes in Atlas format
		for (int i = 0; i < readBytes; i++)
		{	
			if (readBuffer[i] == 0x40)//replace "@" with " "
				readBuffer[i] = 0x20;
			inString.append(String::formatted("<$%02X>", readBuffer[i]), 8);
		}
		//append \n at end
		inString.append("\\n", 10);


		inString.append("\n", 10);
		
		outputFile.appendText(inString);

		inString.clear();

		stringCounter++;
		
	}
	
	inString.append("<$1F>\n", 10);
	
	//write empty strings for the rest of the file
	for (int i = 1 + destinationSlot; i < stringCounter + 1; i++)
	{
		inString.append(String::formatted("#W16(%i)\n", i * 2),10);
		inString.append(String("<$1F>\n"),10);
	}
	outputFile.appendText(inString);
	

	std::cout << "Done! \n";
	return 0;
}

void warningNrOfStrings(int lineCount, int nrOfStrings)
{
	std::cout << "Warning, nr of strings different: \n";
	std::cout << "ENG: " << lineCount << "\n";
	std::cout << "JAP: " << nrOfStrings << "\n";
	//std::cin.get();
	return;
}

int CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT_NPC(bool processSingleNPC, int inputNdx)
{

	std::cout << "CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT_NPC\n";


	File uw_strings_clean = File::getCurrentWorkingDirectory()
		.getChildFile("../../uw1_strings_clean.txt");
	if (!uw_strings_clean.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inputStream(uw_strings_clean); // [2]
	if (!inputStream.openedOk())
		return 1;  // failed to open

	File myfile;
	File* outputFile = 0;
	int lineIndex = 0, blockIndex = 0, lineCount = 0;
	int nrOfStrings = 0;


	std::cout << "InFile: " << "uw1_strings_clean.txt\n";

	std::cout << "Processing... \n";

	while (!inputStream.isExhausted()) // [3]
	{
		auto line = inputStream.readNextLine();

		if (line.isEmpty())
			continue;

		if (line.startsWith("+="))
			continue;

		if (line.startsWith("Block"))
		{
			//////////////////////////////////
			// FINISH UP OLD BLOCK
			/////////////////////////////////
			// at the end of file, finish up created file:

			if (lineIndex != 0)
			{

				// line count warner, alerts to differences in amount of strings between ENG and JAP version
				if (lineCount != nrOfStrings)
					warningNrOfStrings(lineCount, nrOfStrings);

				// add <1F> for strings missing in english version, compared to jap
				if (lineCount < nrOfStrings+1)
				{
					for (int i = lineCount; i < nrOfStrings+1; i++)
						insertStringNPC(outputFile, 2 * i, "<$1F>\n");
				}


				int nrPointers = lineCount + 1;
				std::cout << "nrPointers: 0x" << std::hex << nrPointers << " \n";
				std::cout << "offset: 0x" << std::hex << 2 * (nrPointers + 2) << " \n";
			}
			lineIndex = 0;







			//////////////////////////////////////////
			// NEW BLOCK FILE STARTS FROM HERE
			/////////////////////////////////////////

			// find block number
			blockIndex = (line.substring(12)).getIntValue();

			// only process NPC blocks here
			if (blockIndex < 3585)
				continue;

			//only process this NPC block
			if (processSingleNPC && (blockIndex != inputNdx))
				continue;


			lineCount = 0;

			// create and open output file
			String destination;
			destination.append("/outputNPC/UU_Block_", 30);
			destination.append(String(blockIndex), 4);
			destination.append(".txt", 4);

			std::cout << "OutFile: " << destination << "\n";

			String filename = File::getCurrentWorkingDirectory().getFullPathName();
			filename.append(destination, 40);

			myfile = File::getCurrentWorkingDirectory().getChildFile(filename.getCharPointer());

			myfile.create();
			outputFile = &myfile;

			int npcIndex = getNPCnrFromBlockIndex(blockIndex);

			nrOfStrings = getNPCnrOfStringsFromAvatar(npcIndex);

			// write atlas header
			outputFile->replaceWithText("#VAR(dialogue, TABLE)\n");
			outputFile->appendText("#ADDTBL(\"ascii.tbl\", dialogue)\n");
			outputFile->appendText("#ACTIVETBL(dialogue)\n");
			outputFile->appendText(String::formatted("#HDR($%08X)\n", -2));
			outputFile->appendText(String::formatted("#JMP($%08X)\n\n", 2*(nrOfStrings+1)));

	
		}
		else // line entry
		{
			// only process NPC blocks here
			if (blockIndex < 3585)
				continue;

			//only process this NPC block
			if (processSingleNPC && (blockIndex != inputNdx))
				continue;

			if (lineCount+1 > nrOfStrings)
			{
				//warningNrOfStrings(lineCount+1, nrOfStrings);
				lineCount++; //only count here, warn at end of block (above), so warning is only given once
				continue;
			}
			// Text string
			// get string number
			lineIndex = (line.substring(0, 3)).getIntValue();

			//drop string number and "=" , only keep string
			line = line.substring(4);

			String stringWithLineBreaks = insertLineBreaks(line, blockIndex);

			stringWithLineBreaks.append("<$1F>\n", 10);
			insertStringNPC(outputFile, 2 * lineCount, stringWithLineBreaks);

			lineCount++;
		}

	}
	


	std::cout << "Done! \n";
	return 0;
}


int CREATE_INDIVIDUAL_BIN_WITH_ATLAS_NPC(int blockIndex)
{

	if ((blockIndex < 3585) || (blockIndex > 3898))
		return 1;

	String binFilename = ("outputNPC\\");
	binFilename.append(String::formatted("UU_Block_%i", blockIndex), 20);
	binFilename.append(".bin", 20);
	File binOutFile = File::getCurrentWorkingDirectory().getChildFile(binFilename.getCharPointer());
	if (binOutFile.existsAsFile())
		binOutFile.deleteFile();
	binOutFile.create();

	String txtFilename = ("outputNPC\\");
	txtFilename.append(String::formatted("UU_Block_%i", blockIndex), 20);
	txtFilename.append(".txt", 20);

	//invoke atlas
	char command[256];
	sprintf(command, "atlas %s %s", binFilename.toRawUTF8(), txtFilename.toRawUTF8());

	std::cout << "\n\n" << command << "\n\n";

	system(command);

	std::cout << "Done! \n";

	
	return 0;
};


int PATCH_AVATAR_FROM_BIN_SINGLE_NPC(int blockIndex, int* outNewNPCSize)
{
	int NPCnr = getNPCnrFromBlockIndex(blockIndex);

	std::cout << std::hex;
	std::cout << "NPCnr: " << NPCnr << "\n";
	std::cout << "getNPCaddress: " << getNPCaddress(NPCnr, true) << "\n";
	std::cout << "getNPCaddressRel: " << getNPCaddress(NPCnr, false) << "\n";

	std::cout << "getNPCStringsaddress: " << getNPCStringsaddress(NPCnr, true) << "\n";
	std::cout << "getNPCStringsaddressRel: " << getNPCStringsaddress(NPCnr, false) << "\n";

	std::cout << "getNPCTotalSize: " << getNPCTotalSize(NPCnr) << "\n";
	std::cout << "getNPCStringsSize: " << getNPCStringsSize (NPCnr) << "\n";

	// CREATE INFILE
	String binFilename = ("outputNPC\\");
	binFilename.append(String::formatted("UU_Block_%i", blockIndex), 20);
	binFilename.append(".bin", 4);
	File binInFile = File::getCurrentWorkingDirectory().getChildFile(binFilename.getCharPointer());
	FileInputStream inStreamBin(binInFile); // [2]
	if (!inStreamBin.openedOk())
		return 1;  // failed to open

	std::cout << "\n" << "InFile: " << binFilename << "\n";

	std::cout << "FileSize: " << binInFile.getSize() << "\n";

	

	if (getNPCStringsSize(NPCnr) < binInFile.getSize())
	{
		std::cout << "Missing space: " << binInFile.getSize() - getNPCStringsSize(NPCnr) << "\n";
		std::cout << "ERROR - Available space exceeded";
		//std::cin.get();
	}
	else 
		std::cout << "Available space: " << getNPCStringsSize(NPCnr) - binInFile.getSize() << "\n";

	copyAVATARFile("AVATAR_temp", "AVATAR_orig");

	// CREATE OUTFILE
	File AVATAR_temp = File::getCurrentWorkingDirectory().getChildFile("AVATAR_temp");
	if (!AVATAR_temp.existsAsFile())
		return 1;  // file doesn't exist
	FileOutputStream outStreamAvatarTemp(AVATAR_temp); // [2]
	if (!outStreamAvatarTemp.openedOk())
		return 1;  // failed to open

	//debug, only show stdout info
	//return 0;

	int NPCStringAddress = getNPCStringsaddress(NPCnr, true);

	std::cout << "Writing bin at address: " << NPCStringAddress << "\n";

	//write contents of bin file
	outStreamAvatarTemp.setPosition(NPCStringAddress);
	outStreamAvatarTemp.writeFromInputStream(inStreamBin, binInFile.getSize());

	int newNPCsize = getNPCStringsaddress(NPCnr, false) + binInFile.getSize();

	//padding, align to paddingValue nr of byte adresses (PSX CD Blocks)
	int paddingValue = 2048;
	int nrOfBytesToFill = paddingValue - (newNPCsize % paddingValue);
	for (int i = 0; i < nrOfBytesToFill; i++)
	{
		outStreamAvatarTemp.setPosition(NPCStringAddress+ binInFile.getSize()+i);
		outStreamAvatarTemp.writeByte(0);
		newNPCsize++;
	}



	std::cout << "newNPCsize: " << newNPCsize << "\n";

	*outNewNPCSize = newNPCsize;

	return 0;
};

int PATCH_AVATAR_COPY_NPC_TO_ADDRESS(int inNPCblockIndex, int inAddress, int inNewNPCSize)
{
	// this function copies a complete NPC structure to a predefined NPC Address, so it can be debugged in-game.
	// the predefined address is Shak in Level 2, because he provides the most space (every other NPC fits in his memory space),
	// and he is easy to reach on level 2

	int NPCnr = getNPCnrFromBlockIndex(inNPCblockIndex);
	int NPCaddress = getNPCaddress(NPCnr, true);
	int NPCtotalSize = getNPCTotalSize(NPCnr);

	//Shak
	//int NPCDebugAddress = getNPCaddress(1, true);
	//int NPCDebugTotalSize = getNPCTotalSize(1);

	std::cout << std::hex;
	std::cout << "Copy NPC to Destination\n";
	std::cout << "NPC Index: " << inNPCblockIndex << "\n";
	std::cout << "NPCnr: " << NPCnr << "\n";
	std::cout << "getNPCaddress: " << NPCaddress << "\n";
	std::cout << "getNPCTotalSize: " << inNewNPCSize << "\n";

	std::cout << "getNPCDestinationAddress: " << inAddress << "\n";
	//std::cout << "getNPCDebugTotalSize: " << NPCDebugTotalSize << "\n";

	File AVATAR_temp = File::getCurrentWorkingDirectory().getChildFile("AVATAR_temp");
	if (!AVATAR_temp.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inStreamAvatarTemp(AVATAR_temp); // [2]
	if (!inStreamAvatarTemp.openedOk())
		return 1;  // failed to open

	// CREATE IN/OUTFILE
	File AVATAR = File::getCurrentWorkingDirectory().getChildFile("AVATAR");
	if (!AVATAR.existsAsFile())
		return 1;  // file doesn't exist
	FileOutputStream outStreamAvatar(AVATAR); // [2]
	if (!outStreamAvatar.openedOk())
		return 1;  // failed to open

	std::cout << "Writing NPC to destination address: " << inAddress << "\n";

	// copy NPC
	inStreamAvatarTemp.setPosition(NPCaddress);
	outStreamAvatar.setPosition(inAddress);
	outStreamAvatar.writeFromInputStream(inStreamAvatarTemp, inNewNPCSize);


	//update NPC pointer table
	//pointer of this NPC
	const int BASE_ADDRESS_NPC = 0x3A2000;
	outStreamAvatar.setPosition(BASE_ADDRESS_NPC + (4 * (NPCnr + 1)));
	outStreamAvatar.writeInt(inAddress - BASE_ADDRESS_NPC);

	//pointer of end (needed for last NPC, write one more pointer)
	if (inNPCblockIndex == 3898)
	{
		outStreamAvatar.setPosition(BASE_ADDRESS_NPC + (4 * (NPCnr + 2)));
		outStreamAvatar.writeInt(inAddress - BASE_ADDRESS_NPC + inNewNPCSize);
	}	

	return 0;
};


int CREATE_ALL_BINS_WITH_ATLAS_NPC()
{
	for (int i = 0; i < nrOfNPCs; i++)
	{
		int blockIndex = getBlockIndexFromNPCnr(i);
		CREATE_INDIVIDUAL_BIN_WITH_ATLAS_NPC(blockIndex);
	}

	return 0;
}

int PATCH_AVATAR_FROM_BIN_ALL_EXTENDED_NPCS()
{
	//patches all NPCs into extended ROM. NPC structs in ENG are larger than original JAP sizes.
	//need to get first part of NPC struct from orig AVATAR, second from bin and write it into extended AVATAR

	//because the NPC structs are larger, following structs will be overwritten.
	//need to work with temp file, that is being reset after each NPC

	//plan: use existing function PATCH_AVATAR_FROM_BIN_SINGLE_NPC() to patch NPC into temp file, 
	//then adapt PATCH_AVATAR_COPY_NPC_TO_DEBUG_ADDRESS to copy temp file NPC to extended final ROM and update pointer table
	

	const int BASE_ADDRESS_NPC = 0x3A2000;
	const int ADDRESS_FIRST_NPC = 0x3A2800;
	int address = ADDRESS_FIRST_NPC;

	int newNPCSize = 0;

	for (int i = 0; i < nrOfNPCs; i++)
	{
		int blockIndex = getBlockIndexFromNPCnr(i);
		PATCH_AVATAR_FROM_BIN_SINGLE_NPC(blockIndex, &newNPCSize);
		PATCH_AVATAR_COPY_NPC_TO_ADDRESS(blockIndex, address, newNPCSize);
		address += newNPCSize;
	}

	return 0;
}