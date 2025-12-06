/*
 * Author: Gustavo Torres
 * Course: CS-210
 * Project: Project Three - Corner Grocer
 * Prof. Bary Pollack
 */

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <limits>

using namespace std;

class GroceryTracker {
private:
    map<string, int> itemFrequencies;

public:
    GroceryTracker() = default;

    void loadData(const string& inputFileName) {
        ifstream inFile(inputFileName);
        if (!inFile.is_open()) {
            cerr << "Error: Could not open input file: " << inputFileName << endl;
            return;
        }

        string item;
        while (inFile >> item) {
            ++itemFrequencies[item];
        }

        inFile.close();
    }

    void writeBackupFile(const string& backupFileName = "frequency.dat") const {
        ofstream outFile(backupFileName);
        if (!outFile.is_open()) {
            cerr << "Error: Could not create backup file: " << backupFileName << endl;
            return;
        }

        for (const auto& pair : itemFrequencies) {
            outFile << pair.first << " " << pair.second << endl;
        }

        outFile.close();
    }

    int getFrequencyOfItem(const string& itemName) const {
        auto it = itemFrequencies.find(itemName);
        if (it != itemFrequencies.end()) {
            return it->second;
        }
        return 0;
    }

    void printAllFrequencies() const {
        cout << "\n=== Item Frequencies ===" << endl;
        for (const auto& pair : itemFrequencies) {
            cout << pair.first << " " << pair.second << endl;
        }
        cout << endl;
    }

    void printHistogram() const {
        cout << "\n=== Item Frequency Histogram ===" << endl;
        for (const auto& pair : itemFrequencies) {
            cout << pair.first << " ";
            for (int i = 0; i < pair.second; ++i) {
                cout << "*";
            }
            cout << endl;
        }
        cout << endl;
    }
};

void displayMenu() {
    cout << "==============================" << endl;
    cout << "   Corner Grocer - Main Menu  " << endl;
    cout << "==============================" << endl;
    cout << "1. Look up frequency of an item" << endl;
    cout << "2. Print frequency of all items" << endl;
    cout << "3. Print histogram of all items" << endl;
    cout << "4. Exit program" << endl;
    cout << "------------------------------" << endl;
    cout << "Enter your choice (1-4): ";
}

int getValidatedMenuChoice() {
    int choice;
    while (true) {
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 4) {
            cout << "Invalid choice. Please enter a number between 1 and 4: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }
    }
}

int main() {
    GroceryTracker tracker;
    const string INPUT_FILE_NAME = "CS210_Project_Three_Input_File.txt";

    tracker.loadData(INPUT_FILE_NAME);
    tracker.writeBackupFile("frequency.dat");

    cout << "Data loaded from: " << INPUT_FILE_NAME << endl;
    cout << "Backup file 'frequency.dat' has been created.\n" << endl;

    bool running = true;

    while (running) {
        displayMenu();
        int choice = getValidatedMenuChoice();

        switch (choice) {
        case 1: {
            cout << "\nEnter item name: ";
            string item;
            cin >> item;
            cout << "\n'" << item << "' was purchased "
                << tracker.getFrequencyOfItem(item)
                << " time(s).\n" << endl;
            break;
        }
        case 2:
            tracker.printAllFrequencies();
            break;

        case 3:
            tracker.printHistogram();
            break;

        case 4:
            cout << "\nExiting program." << endl;
            running = false;
            break;
        }
    }

    return 0;
}
