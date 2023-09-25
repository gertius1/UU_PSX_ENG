#include "stringsGeneral.h"
#include "utilities.h"

int CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN()
{
	std::cout << "CREATE_ATLAS_INPUT_TXT_FILES_FROM_BIN\n";

	const int POINTER_TABLE_START = 0xB8D8E;
	const int POINTER_TABLE_STOP = 0xB918E;
	const int POINTER_SIZE = 0x02;
	const int BASE_POINTER = 0xB918E;


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
	String destination = "/output/UU_Block_11_orig";
	destination.append(".txt", 4);

	String filename = File::getCurrentWorkingDirectory().getFullPathName();
	filename.append(destination, 40);

	File outputFile;
	outputFile = File::getCurrentWorkingDirectory().getChildFile(filename.getCharPointer());
	outputFile.create();
	// write atlas header
	outputFile.replaceWithText("#VAR(dialogue, TABLE)\n");
	outputFile.appendText("#ADDTBL(\"ascii.tbl\", dialogue)\n");
	outputFile.appendText("#ACTIVETBL(dialogue)\n");
	outputFile.appendText(String::formatted("#HDR($%08X)\n", 0x400));
	outputFile.appendText(String::formatted("#JMP($%08X)\n\n", 0x400));

	std::cout << "InFile: " << "AVATAR_orig\n";
	std::cout << "OutFile: " << destination << "\n";


	int POINTER = POINTER_TABLE_START;
	inStreamAvatar.setPosition(POINTER);

	int ptrAddress = inStreamAvatar.readShort() + BASE_POINTER;
	int ptrAddressPrev = 0;
	int bytesToRead = 0;
	unsigned char readBuffer[256];
	int stringCounter = 0;

	std::cout << "Processing... \n";

	while (POINTER < POINTER_TABLE_STOP)
	{
		ptrAddressPrev = ptrAddress;
		POINTER += POINTER_SIZE;
		inStreamAvatar.setPosition(POINTER);
		ptrAddress = inStreamAvatar.readShort() + BASE_POINTER;
		bytesToRead = ptrAddress - ptrAddressPrev;

		inStreamAvatar.setPosition(ptrAddressPrev);
		int readBytes = inStreamAvatar.read(readBuffer, bytesToRead);
		String inString;

		inString.append(String::formatted("%03i=", stringCounter), 8);

		//single large string, drop 1F at end
		//readBytes = readBytes - 1;

		for (int i = 0; i < readBytes; i++)
			inString.append(String::formatted("<$%02X>", readBuffer[i]), 8);

		//single large string,append \n at end
		//inString.append("\\n", 10);

		if (inString.length() == 9)
		{
			//if only endchar 1F, drop numbering, so it will be redundancy-compressed
			inString = inString.fromFirstOccurrenceOf("=", false, false);
		}



		inString.append("\n", 10);
		insertString(&outputFile, 2 * stringCounter, inString);
		stringCounter++;
	}

	std::cout << "Done! \n";
	return 0;
}


int CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT()
{
	std::cout << "CREATE_ATLAS_INPUT_TXT_FILES_FROM_TXT\n";


	File uw_strings_clean = File::getCurrentWorkingDirectory()
		.getChildFile("../../uw1_strings_clean.txt");
	if (!uw_strings_clean.existsAsFile())
		return 1;  // file doesn't exist
	FileInputStream inputStream(uw_strings_clean); // [2]
	if (!inputStream.openedOk())
		return 1;  // failed to open

	File myfile;
	File* outputFile = 0;
	int lineIndex = 0, lineIndexPrev = 0, blockIndex = 0;


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
			// FINISH UP OLD BLOCK

			// at the end of file, finish up created file:
			//- fill with empty strings up to 512

			if (lineIndexPrev != 0)
			{
				for (int i = 0; i < 512 - lineIndexPrev - 1; i++)
					insertString(outputFile, 2 * (lineIndexPrev + i + 1), "<$1F>\n");

				//add one last write command to push out previous string
				insertString(outputFile, 2 * (512), "FF\n");
			}
/*
			// log NPC blocks nrOfLines
			if (blockIndex >= 3585)
			{
				// Log StringNumber per Block
				String logMsg;
				logMsg.append(String::formatted("Block %i ", blockIndex), 20);
				logMsg.append(String::formatted("Lines %i", lineIndexPrev), 20);
				writeToLogFile(logMsg);
			}
*/

			// NEW BLOCK FILE STARTS FROM HERE

			// find block number
			blockIndex = (line.substring(12)).getIntValue();

			if (blockIndex > 11)
			{
				std::cout << "Done early! \n";
				return 0;
			}

			// create and open output file
			String destination;
			destination.append("/output/UU_Block_", 30);
			destination.append(String(blockIndex), 4);
			destination.append(".txt", 4);

			std::cout << "OutFile: " << destination << "\n";

			String filename = File::getCurrentWorkingDirectory().getFullPathName();
			filename.append(destination, 40);

			myfile = File::getCurrentWorkingDirectory().getChildFile(filename.getCharPointer());

			myfile.create();
			outputFile = &myfile;


			// write atlas header
			outputFile->replaceWithText("#VAR(dialogue, TABLE)\n");
			outputFile->appendText("#ADDTBL(\"ascii.tbl\", dialogue)\n");
			outputFile->appendText("#ACTIVETBL(dialogue)\n");
			outputFile->appendText(String::formatted("#HDR($%08X)\n", 0x400));
			outputFile->appendText(String::formatted("#JMP($%08X)\n\n", 0x400));

			lineIndex = 0;
			lineIndexPrev = -1; //because of first line can be index 001 sometimes

		}
		else
		{
			// Text string
			// get string number
			lineIndex = (line.substring(0, 3)).getIntValue();

			//drop string number and "=" , only keep string
			line = line.substring(4);

			// check if string doesn´t exist, still needs atlas line
			for (int i = 0; i < lineIndex - lineIndexPrev - 1; i++)
				insertString(outputFile, 2 * (lineIndexPrev + i + 1), "<$1F>\n");

			//drop plurals for readability and filesize
			line = line.upToFirstOccurrenceOf("&", false, false);

			//drop articles, not used in the jap version that way, and saves size
			line = line.replace("a_", " ", false);
			line = line.replace("an_", " ", false);
			line = line.replace("some_", " ", false);
			line = line.replace("the_", " ", false);

			//replace number words with numbers for filesize reduction
			if ((blockIndex == 1) && (lineIndex > 410) && (lineIndex < 511))
			{
				int number = lineIndex - 410;
				line = String::formatted("%i", number);

				/* //no ROM space left for counting endings
				//append counting endings to number
				if ((number > 10) && (number < 20)) //10th...19th
					line.append("th",2);
				else if ((number%10) == 1)
					line.append("st", 2);
				else if ((number % 10) == 2)
					line.append("nd", 2);
				else if ((number % 10) == 3)
					line.append("rd", 2);
				else
					line.append("th", 2);
				*/
			}
			String stringWithLineBreaks = insertLineBreaks(line, blockIndex);

			stringWithLineBreaks.append("<$1F>\n", 10);
			insertString(outputFile, 2 * lineIndex, stringWithLineBreaks);

			lineIndexPrev = lineIndex;
		}

	}
	std::cout << "Done! \n";
	return 0;
}

int CREATE_INDIVIDUAL_BIN_WITH_ATLAS()
{
	for (int i = 0; i < 11; i++)
	{
		int blockIndex = i+1;
		String binFilename = ("output\\");
		binFilename.append(String::formatted("UU_Block_%i", blockIndex), 20);
		binFilename.append(".bin", 4);
		File binOutFile = File::getCurrentWorkingDirectory().getChildFile(binFilename.getCharPointer());
		if (binOutFile.existsAsFile())
			binOutFile.deleteFile();
		binOutFile.create();

		String txtFilename = ("output\\");
		txtFilename.append(String::formatted("UU_Block_%i", blockIndex), 20);
		txtFilename.append(".txt", 4);

		//invoke atlas
		char command[50];
		sprintf(command, "atlas %s %s", binFilename.toRawUTF8(), txtFilename.toRawUTF8());

		std::cout << "\n\n" << command << "\n\n";

		system(command);

		std::cout << "Done! \n";

	}
	return 0;
};




int PATCH_AVATAR_FROM_BIN_MANUALLY()
{
	std::cout << "PATCH_AVATAR_FROM_BIN_MANUALLY\n";

	std::cout << "OutFile: " << "AVATAR\n";
	std::cout << "Available space 0xB0000-0xB9A28: " << 0x9A28 << "\n";

	int writeAddress = 0x5C;
	int topPointerWriteAddress = 0x8;
	int totalFileSize = 0;
	const int topPointerTableStartAddr = 0xB0000;

	for (int i = 0; i < 11; i++)
	{
		int blockIndex = i+1;

		// CREATE INFILE
		String binFilename = ("output\\");
		binFilename.append(String::formatted("UU_Block_%i", blockIndex), 20);
		binFilename.append(".bin", 4);
		File binInFile = File::getCurrentWorkingDirectory().getChildFile(binFilename.getCharPointer());
		FileInputStream inStreamBin(binInFile); // [2]
		if (!inStreamBin.openedOk())
			return 1;  // failed to open

		std::cout << "\n" << "InFile: " << binFilename << "\n";

		std::cout << "FileSize: " << binInFile.getSize() << "\n";

		// CREATE OUTFILE
		File AVATAR = File::getCurrentWorkingDirectory().getChildFile("AVATAR");
		if (!AVATAR.existsAsFile())
			return 1;  // file doesn't exist
		FileOutputStream outStreamAvatar(AVATAR); // [2]
		if (!outStreamAvatar.openedOk())
			return 1;  // failed to open

		//write pointerTable position to topPointerTable
		outStreamAvatar.setPosition(topPointerTableStartAddr + topPointerWriteAddress);
		outStreamAvatar.writeInt(writeAddress);

		if (blockIndex < 3072)
			topPointerWriteAddress += 0x8;
		else
			topPointerWriteAddress += 0x4;

		// block 10 not used, save memory
		if (blockIndex == 10)
			continue;

		//manually write 0x200 (number of pointers) at start of pointer table
		outStreamAvatar.setPosition(topPointerTableStartAddr + writeAddress);
		outStreamAvatar.writeShort(512);
		writeAddress += 2;

		std::cout << "Writing bin at address: " <<  topPointerTableStartAddr + writeAddress << "\n";

		//write contents of bin file
		outStreamAvatar.setPosition(topPointerTableStartAddr + writeAddress);
		outStreamAvatar.writeFromInputStream(inStreamBin, binInFile.getSize());
		writeAddress += (int)binInFile.getSize();
		totalFileSize += (int)binInFile.getSize() + 2; //don´t count block 2

		//if not ending on an even byte, add a padding 00
		if (writeAddress % 2)
		{
			outStreamAvatar.setPosition(topPointerTableStartAddr + writeAddress);
			outStreamAvatar.writeByte(0x00);

			writeAddress += 1;
			totalFileSize += 1;
		}

		std::cout << "TotalFileSize: " << totalFileSize << "\n";
		std::cout << "Available space: " << 0x9A28 - totalFileSize << "\n";
		if (totalFileSize - 0x9A28 > 0)
		{
			std::cout << "WARNING, Available space exceeded by: " << totalFileSize - 0x9A28 << "\n\n";
			std::cin.get();
		}
	}



	std::cout << "Done! \n";
	return 0;
}


