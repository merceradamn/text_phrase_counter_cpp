/*
Name:		Adam Mercer
Date:		
Project:	Common Phrase Text Comparison
Descript:	Reads in two texts and calculates 10 most common phrases between
			the two texts.
Resources:	
Notes:		Steps taken to produce the solution:
			1) textFormatter(...)
				Format the text, removing anything but alpha-numeric and apostrophes
				Produces a formatted text file for each source text file
			2) buildWordVector(...)
				Parses the formatted text files for strings and stores them in a vector
			3) calculateFrequencies(...)
				Creates up to N word phrases using the string vector produced in step 2
			4) Non-function Step
				Removes any created text files (used for debugging or steps) and frees up
				any memory locked up in both string vectors previously needed in earlier
				steps
			5) cleanPhraseMap(...)
				Cleans any phrases that don't have at least 1 occurence in both texts
			6) printNCommonPhrases(...)
				Splits the built map into smaller maps that hold equal word length phrases
				Prints them to the console as well as writing them to a file for further reading

			There are files created along the way that can be used to diagnose any problems
			as well as being able to check how accurately each step works so any tweaks can
			be made to ensure a proper solution.
*/

#include <iostream>				// For using cout
#include <string>				// String-handling header
#include <iomanip>				// Formatting header
#include <fstream>				// File-handling header
#include <map>
#include <vector>
using namespace std;

// Struct definitions for holding data from phrase construction
struct phrasePair {
	string phrase;
	int numWords;
	int freq1, freq2;
};

struct phrObj {
	string phrase;
	int frq1, frq2;
	
};

void textFormatter(const string &textInput, const string &form);
void buildWordVector(const string &formTxt, vector<string> &wordVec);
// const string &phrTxt is not necessary, just for sake of debugging/error correction
void calculateFrequencies(map<long long, phrasePair> &phrFrq, vector<string> &wordVec,
	const string &debugPhr, const int &t);
void cleanPhraseMap(map<long long, phrasePair> &phrFrq);
void printNCommonPhrases(map<long long, phrasePair> &phrFrq);

int main() {

/*************************************************************************************************/
/*									VARIABLE DECLARATIONS										 */
/*************************************************************************************************/
	string text1 = "Adventures of Huckleberry Finn.txt";
	string text2 = "The Adventures of Tom Sawyer.txt";
	string text1formatted = "text1_formatted.txt";
	string text2formatted = "text2_formatted.txt";
	string phrTxt1 = "phraseText1.txt";
	string phrTxt2 = "phraseText2.txt";

	vector<string> wordVec1, wordVec2;					// Word vectors holding texts
	map<long long, phrasePair> phrFrq;					// Map 1 of the key-phrasePair pair

	int text;	// Keeps track of which text we're working on

/*************************************************************************************************/
/*										TEXT PROCESSING											 */
/*************************************************************************************************/
	cout << "================================================\n";
	cout << "             PROCESSING TEXT ONE...             \n";
	cout << "================================================\n";
	text = 1;
	textFormatter(text1, text1formatted);
	buildWordVector(text1formatted, wordVec1);
	calculateFrequencies(phrFrq, wordVec1, phrTxt1, text);

	// Remove the created file and clear the string vector
	const char *T1F = text1formatted.c_str();
	if (remove(T1F) != 0)
		cout << "text1_formatted not removed.\n";

	wordVec1.clear();

	cout << "================================================\n";
	cout << "             PROCESSING TEXT TWO...             \n";
	cout << "================================================\n";
	text = 2;
	textFormatter(text2, text2formatted);
	buildWordVector(text2formatted, wordVec2);
	calculateFrequencies(phrFrq, wordVec2, phrTxt2, text);

	// Remove the created files and clear the string vector
	const char *T2F = text2formatted.c_str();
	if (remove(T2F) != 0)
		cout << "text2_formatted not removed.\n";

	wordVec2.clear();

	cout << "================================================\n";
	cout << "       CALCULATING MOST COMMON PHRASES...       \n";
	cout << "================================================\n";

	cleanPhraseMap(phrFrq);
	printNCommonPhrases(phrFrq);

/*************************************************************************************************/
	return 0;
}

// Function that gets passed a source file name and a destination filename
// Formats the source file text to contain only alpha-numeric characters and apostrophes
// Converts any uppercase letters to lowercase and removes any other characters
void textFormatter(const string &textInput, const string &form) {

	fstream text;				// Reads the file in
	fstream output;				// Writes formatted text to a file
	char ch;

	// Open the first text and check if it properly opened
	text.open(textInput, ios::in);
	if (!text.is_open()) {
		cout << "Text Open Fail.\n";
	}

	cout << "Formatting the text now...\t";
	// Open the output file and start formatting the first text for processing
	output.open(form, ios::out | ios::trunc);
	while (text.get(ch)) {
		if (ch >= 65 && ch <= 90) {			// Handle uppercase letters
			ch = tolower(ch);
			output << ch;
		}
		// Handles lowercase letters( a-z ) and numbers ( 0-9 )
		else if ((ch >= 97 && ch <= 122) || (ch >= 48 && ch <= 57)) {
			output << ch;
		}
		// Handles CR LF, space, and commas
		else if (ch == 10 || ch == 13 || ch == 32 || ch == 44) {
			ch = 32;
			output << ch;
		}
		// Handles dashes and periods
		else if (ch == '-' || ch == '.') {
			ch = 32;
			output << ch;
		}
		// Handles apostrophes
		else if (ch == -30) {
			ch = '\'';
			text.ignore(1, 1);
			output << ch;
		}
	}
	output.close();
	text.close();
	cout << " DONE!\n";

}

// Function that gets passed a formatted text file name and a string vector
// Parses a text file and stores every "word" in the passed vector
void buildWordVector(const string &formTxt, vector<string> &wordVec) {

	fstream output;
	string input;
	// Open the formatted text in read mode and load it into the vector
	cout << "Vectorizing the formatted text...\t";
	output.open(formTxt, ios::in);
	while (output >> input) {							// Process the text
		wordVec.push_back(input);						// Add a word to the vector
	}

	output.close();										// Close the text
	cout << " DONE!\n";

}

// Function that calculates the frequency of unique phrases in a text
// Passed the string vector created in buildWordVector
void calculateFrequencies(map<long long, phrasePair> &phrFrq, vector<string> &wordVec,
	const string &debugPhr, const int &t) {

	hash<string> strHash;								// Hashing strings
	long long key;										// Stores the hashed value of a string
	string phrase;										// Holds current phrase being manipulated

	map<long long, phrasePair>::iterator phrFrqIter;	// Iterator for phrFrq
	phrasePair PP;										// phrasePair var boilerplate

	vector<string>::iterator current, reader;			// Word vector iterators, current + reader
														// Current iterates over the vector list; Reader iterates over a 10 word segment
	vector<string> phr;									// Holds 10 word string vector for phrases
	vector<string>::iterator phrIT;						// Iterator for phr

	// Start assembling phrases
	cout << "Building phrase frequency map for text...\t";
	current = wordVec.begin();
	// Get first 10 words
	reader = current;
	for (int i = 0; i < 10; i++) {
		phr.push_back(*reader);
		reader++;
	}
	phrIT = phr.begin();

	while (current != wordVec.end()) {
		for (int j = 1; j < 11; j++) {
			for (int i = 0; i < j; i++) {
				phrase += phr[i];
				phrase += " ";
			}

			// Hash the phrase and check the map to see if it's present
			key = strHash(phrase);

			// Uses map of int64, phrasePair struct
			phrFrqIter = phrFrq.find(key);
			if (phrFrqIter != phrFrq.end()) {
				// Found the phrase so just increment the frequency
				// Check if we're working with text 1 or 2
				if(t == 1)
					phrFrqIter->second.freq1++;
				else if(t == 2)
					phrFrqIter->second.freq2++;
			}
			else if (phrFrqIter == phrFrq.end()) {
				// Not in map yet so add it to the map
				// Check if we're working with text 1 or 2
				if (t == 1) {
					PP.freq1 = 1;
					PP.freq2 = 0;
					PP.numWords = j;
					PP.phrase = phrase;
					phrFrq.insert(pair<long long, phrasePair>(key, PP));
				}
				else if (t == 2) {
					PP.freq1 = 0;
					PP.freq2 = 1;
					PP.numWords = j;
					PP.phrase = phrase;
					phrFrq.insert(pair<long long, phrasePair>(key, PP));
				}
			}

			phrase.clear();
		}

		// Dump the first word and add the next
		phr.erase(phr.begin());				// Erase the first element
		current++;							// Increment iterator to the next word
		phr.push_back(*reader);
		reader = current;					// Assign reader to current iterator
											// Get the next word and put it on the end of phr
		phrase.clear();
	}

	cout << " DONE!\n";
	phr.clear();
}

// Function that cleans trash entries in phrase frequency map and preps it for printing
// Trash entries are any elements that don't have at least one frequency in each file
void cleanPhraseMap(map<long long, phrasePair> &phrFrq) {
	map<long long, phrasePair>::iterator iter;
	map<long long, phrasePair>::iterator iterPrev;

	cout << "Cleaning uncommon phrases from map...\t";
	iter = phrFrq.begin();
	while (iter != phrFrq.end()) {
		if (iter->second.freq1 == 0 || iter->second.freq2 == 0) {
			// Delete the element
			iterPrev = iter;
			iterPrev--;
			phrFrq.erase(iter);
			if (iterPrev == phrFrq.end()) {
				iter = phrFrq.begin();
			}
			else {
				iter = iterPrev;
			}
		}
		else {
			iter++;
		}
	}
	cout << " DONE!\n";
}

// Function that takes the phrase frequency map and prints the N most common phrases
// Breaks the passed phrFrq variable and splits it into smaller maps that hold the maps with elements
// that have the same number of words, then prints out the top 10 elements of each map & writes to file
void printNCommonPhrases(map<long long, phrasePair> &phrFrq) {

	multimap<int, phrObj> phrases[10];					// Array of maps, corresponds to word counts
	map<long long, phrasePair>::iterator read;			// Iterator for phrFrq map
	int words;											// Temp variable for tracking number of words
	phrObj PO;											// Object used to copy data from map to map	

	int temp;

 	cout << "Building phrase maps by word counts...\t";
	read = phrFrq.begin();								// Initialize iterator to beginning of map
	while (read != phrFrq.end()) {
		// Check how many words the current element has
		// Sort into appropriate map
		words = read->second.numWords;					// Get the number of words current element has
		words--;										// Handle the offset for array indexing

		// Get the data from the phrasePair inside the map and then insert it into its respective map
		PO.frq1 = read->second.freq1;					// Get the first frequency
		PO.frq2 = read->second.freq2;					// Get the second frequency
		PO.phrase = read->second.phrase;				// Get the phrase
		temp = PO.frq1 + PO.frq2;						// Total frequency between files
		phrases[words].insert(pair<int, phrObj>(temp, PO));	// Insert into the new map

		phrFrq.erase(read);								// Erase element after moving into new map
		read = phrFrq.begin();							// Set the iterator to the beginning of phrFrq
	}
	cout << " DONE!\n";
	
	cout << "Printing N most common phrases...\n";

	// Print each map's top 10 most frequent phrases
	// phrase, freq in text1, freq in text2
	multimap<int, phrObj>::reverse_iterator phrRead;
	fstream phraseList;
	phraseList.open("commonPhraseList.txt", ios::out | ios::trunc);

	for (int i = 0; i < 10; i++) {
		phrRead = phrases[i].rbegin();	// Set iterator to the end of the specific map

		// Formatting, might break if command window is sized differently
		cout << "Top 10 most common " << i + 1 << " word phrases:\n";
		cout << setw(3) << "#:" << setw(65) << "Phrase:\t";
		cout << setw(5) << "T1Frq\t" << setw(5) << "T2Frq\t" << setw(5) << "TolFq\n";

		// Write to file
		phraseList << "Top 10 most common " << i + 1 << " word phrases:\n";
		phraseList << setw(3) << "#:" << setw(65) << "Phrase:\t\t";
		phraseList << setw(5) << "T1Frq\t" << setw(5) << "T2Frq\t" << setw(5) << "TolFq\n";

		for (int j = 1; j < 11; j++) {
			// Print the last 10 elements of the map
			temp = phrRead->second.frq1 + phrRead->second.frq2;

			// Print to console
			cout << setw(2) << j << ": ";
			cout << setw(64) << phrRead->second.phrase << "\t";
			cout << setw(5) << phrRead->second.frq1 << "\t";
			cout << setw(5) << phrRead->second.frq2 << "\t";
			cout << setw(5) << temp << "\n";

			// Write to file
			phraseList << setw(2) << j << ": ";
			phraseList << setw(64) << phrRead->second.phrase << "\t";
			phraseList << setw(5) << phrRead->second.frq1 << "\t";
			phraseList << setw(5) << phrRead->second.frq2 << "\t";
			phraseList << setw(5) << temp << "\n";

			phrRead++;
		}

		cout << "\n";
		phraseList << "\n";

	}
	phraseList.close();

}