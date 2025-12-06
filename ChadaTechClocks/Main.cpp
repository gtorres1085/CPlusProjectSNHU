/*
 * Author: Gustavo Torres Reynoso
 * Project: CS 210 Project One - Chada Tech Clocks
 * Description:
 *   Simulates a 12-hour and 24-hour clock that update together.
 *   The user can add one hour, one minute, or one second using a menu.
 */

#include <iostream>
#include <iomanip>   // For setw and setfill
#include <sstream>   // For ostringstream (building time strings)
using namespace std;

// ------------------------
// Clock class declaration
// ------------------------
// The Clock class stores time in 24-hour format and
// provides operations to add one hour/minute/second.
// It can return the time as either a 12-hour or 24-hour string.
class Clock {
private:
    int hour;   // 0 - 23 (24-hour format internally)
    int minute; // 0 - 59
    int second; // 0 - 59

public:
    // Constructor: initialize internal time (24-hour format)
    Clock(int h = 0, int m = 0, int s = 0) {
        hour = h;
        minute = m;
        second = s;
    }

    // Add one second and roll over minute/hour if needed
    void addOneSecond() {
        second++;
        if (second >= 60) {
            second = 0;
            addOneMinute();
        }
    }

    // Add one minute and roll over hour if needed
    void addOneMinute() {
        minute++;
        if (minute >= 60) {
            minute = 0;
            addOneHour();
        }
    }

    // Add one hour and roll over to 0 after 23
    void addOneHour() {
        hour++;
        if (hour >= 24) {
            hour = 0;
        }
    }

    // Return time in 24-hour format as a string, e.g. "13:05:09"
    string getTime24Hour() const {
        ostringstream out;
        out << setw(2) << setfill('0') << hour << ":"
            << setw(2) << setfill('0') << minute << ":"
            << setw(2) << setfill('0') << second;
        return out.str();
    }

    // Return time in 12-hour format as a string, e.g. "01:05:09 PM"
    string getTime12Hour() const {
        int displayHour;
        string amPm;

        if (hour == 0) {           // midnight
            displayHour = 12;
            amPm = "AM";
        }
        else if (hour < 12) {      // 1..11 AM
            displayHour = hour;
            amPm = "AM";
        }
        else if (hour == 12) {     // noon
            displayHour = 12;
            amPm = "PM";
        }
        else {                     // 13..23 -> 1..11 PM
            displayHour = hour - 12;
            amPm = "PM";
        }

        ostringstream out;
        out << setw(2) << setfill('0') << displayHour << ":"
            << setw(2) << setfill('0') << minute << ":"
            << setw(2) << setfill('0') << second << " "
            << amPm;
        return out.str();
    }
};

// ------------------------
// Function prototypes
// ------------------------
void displayClocks(const Clock& clock);
void displayMenu();
int getUserChoice();

int main() {
    // Get initial time from the user (24-hour format)
    int startHour;
    int startMinute;
    int startSecond;

    cout << "Enter starting hour (0-23): ";
    cin >> startHour;

    cout << "Enter starting minute (0-59): ";
    cin >> startMinute;

    cout << "Enter starting second (0-59): ";
    cin >> startSecond;
    cout << endl;

    // Create clock with the initial time
    Clock clock(startHour, startMinute, startSecond);

    int choice = 0;

    // Main menu loop: keep going until user chooses Exit (4)
    while (choice != 4) {
        displayClocks(clock);   // Show both clocks
        displayMenu();          // Show options

        choice = getUserChoice();

        if (choice == 1) {
            // User chose to add one hour
            clock.addOneHour();
        }
        else if (choice == 2) {
            // User chose to add one minute
            clock.addOneMinute();
        }
        else if (choice == 3) {
            // User chose to add one second
            clock.addOneSecond();
        }
        else if (choice == 4) {
            // Exit option
            cout << "Exiting program. Goodbye!" << endl;
        }
        else {
            // Handle invalid input
            cout << "Invalid option. Please try again." << endl;
        }
    }

    return 0;
}

// ------------------------
// Function definitions
// ------------------------

// Display both 12-hour and 24-hour clocks
void displayClocks(const Clock& clock) {
    cout << "***************************"
        << "        "
        << "***************************" << endl;

    cout << "*     12-Hour Clock      *"
        << "        "
        << "*     24-Hour Clock      *" << endl;

    cout << "*      " << clock.getTime12Hour() << "      *"
        << "        "
        << "*        " << clock.getTime24Hour() << "        *" << endl;

    cout << "***************************"
        << "        "
        << "***************************" << endl;
    cout << endl;  // Blank line after clocks
}

// Show the menu options
void displayMenu() {
    cout << "****************** Menu ******************" << endl;
    cout << "* 1 - Add one hour                       *" << endl;
    cout << "* 2 - Add one minute                     *" << endl;
    cout << "* 3 - Add one second                     *" << endl;
    cout << "* 4 - Exit program                       *" << endl;
    cout << "******************************************" << endl;
    cout << "Enter your choice: ";
}

// Get the user's menu choice
int getUserChoice() {
    int choice;
    cin >> choice;
    cout << endl;  // Spacing after input
    return choice;
}
