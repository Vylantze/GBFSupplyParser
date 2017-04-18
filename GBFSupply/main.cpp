#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const string inputFilename = "Granblue Fantasy.html";
const string outputFilename = "GBF_Supply.txt";

const enum Type { 
	R, // Recovery
	E, // Enhancement/Evolution
	T, // Ticket
	N // Normal
};

string getEnumType(Type type) {
	switch (type)	{
		case R: return "R";
		case E: return "E";
		case T: return "T";
		default: return "N";
	}
}

struct Item {
	string imageName;
	int index;
	int quantity;
	Type type = N;
};

void printItem(Item item) {
	cout << "['" << getEnumType(item.type)
		<< item.index << "': "  <<
		"[quantity:'" << item.quantity << "'], " <<
		"[imageName:'" << item.imageName << "'] " <<
		"]" << endl;

	//string test;
	//cin >> test;
}

void main() {
	const string searchString = "<div class=\"lis-item se\" data-index=\"";
	const string evolSearchString = "<div class=\"lis-item btn-evolution";
	const string otherSearchString = "<div class=\"lis-item btn-enhancement-npc";
	const string ticketSearchString = "<div class=\"prt-tickets-num\">";
	const string imgString = "<img src=";
	const string endString = "</div>";
	const string terminateString = "<%= index %>";
	ifstream html;
	ofstream output;
	string line = "";
	size_t found = 0;
	string temp = "";
	bool recoveryItem = true; 
	int recoveryIndex = 0;
	int evolutionIndex = 0;
	int ticketIndex = 0;

	html.open(inputFilename, ios::in);

	if (html.is_open()) {
		output.open(outputFilename, ios::out);
		output << "Index	Quantity	ImageName" << endl;
		while (!html.eof()) {
			// Create a new object
			Item newItem;
			getline(html, line);
			found = line.find(searchString);
			if (found == string::npos) {
				found = line.find(evolSearchString);
				if (found == string::npos) {
					found = line.find(otherSearchString);
					if (found == string::npos) {
						continue;
					} 
					else {// pass Other check
						newItem.type = E;
					}
				} else {// pass evol search string
					recoveryItem = false;
					newItem.type = E;
				}
			} // else an object has been found

			// Get index
			temp = line.substr(
				found + searchString.length(), 
				line.find_last_of('"') - found - searchString.length()
			);
			if (temp == terminateString) { // if it is no longer a valid number
				break; 
			} 
			else if (recoveryItem) {
				newItem.type = R;
				newItem.index = recoveryIndex; // add index
				recoveryIndex++;
			}
			else if (newItem.type == E) {
			  newItem.index = evolutionIndex;
				evolutionIndex++;
			}
			else {
				newItem.index = stoi(temp); // add index
			}

			while (getline(html, line)) {
				// A pre-check for tickets
				found = line.find(ticketSearchString);
				if (found != string::npos) {
					newItem.type = T;
					newItem.index = ticketIndex;
					ticketIndex++;
				}

				// If end of the div is hit, end it.
				found = line.find(endString);
				if (found != string::npos) {
					// Get quantity (most important part)
					temp = line.substr(0, found);
					temp = temp.substr(temp.find(">")+1);
					newItem.quantity = stoi(temp);
					break; // break the inner loop
				}

				// Else, keep searching for the img
				found = line.find(imgString);
				if (found != string::npos) {
					temp = line.substr(0, line.find_last_of('"'));
					newItem.imageName = temp.substr(temp.find_last_of("/") + 1);
					// continue;
				}
			} // end-while for individual item
			printItem(newItem);
			if (newItem.type != N) {
				output << getEnumType(newItem.type);
			}
			output 
				<< newItem.index << "	"
				<< newItem.quantity << "	"
				<< newItem.imageName
			<< endl;
		} // end-while loop for file
	} // end-if file open
	html.close();
	output.close();
	//cout << "Done" << endl;
	//system("pause");
}
