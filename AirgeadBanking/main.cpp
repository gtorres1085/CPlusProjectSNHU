/*
 * Airgead Banking Investment Calculator
 * -------------------------------------
 * Author: Gustavo Torres
 * Course: CS-210
 * Professor: Barry Pollack
 *
 * Description:
 * This program lets the user explore how an investment grows over time,
 * both with and without additional monthly deposits. The user enters
 * an initial investment amount, a monthly deposit, an annual interest
 * rate, and the number of years. The program then displays two reports
 * showing year-end balances and interest earned.
 */

#include <iostream>
#include <limits>
#include "InvestmentAccount.h"

using namespace std;

// ---------------------------------------------------------
// Helper function: Get a non-negative double from the user
// Keeps prompting until the user enters a valid value.
// ---------------------------------------------------------
double GetNonNegativeDouble(const string& prompt) {
    double value;

    while (true) {
        cout << prompt;

        // cin >> value fails if the user types text instead of a number
        if (cin >> value && value >= 0.0) {
            return value;
        }

        cout << "  Invalid input. Please enter a non-negative number.\n";

        // Clear the error state and discard the rest of the line
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ---------------------------------------------------------
// Helper function: Get a positive integer from the user
// Keeps prompting until the user enters a valid value.
// ---------------------------------------------------------
int GetPositiveInt(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value && value > 0) {
            return value;
        }

        cout << "  Invalid input. Please enter a whole number greater than 0.\n";

        // Clear the error state and discard the rest of the line
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ---------------------------------------------------------
// main
// Repeatedly collects user input, runs the investment
// calculation for that scenario, and prints the reports.
// ---------------------------------------------------------
int main() {
    char runAgain = 'y';

    while (runAgain == 'y' || runAgain == 'Y') {

        // Make sure there is no leftover newline in the input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "**********************************\n";
        cout << "********** Data Input ************\n";
        cout << "**********************************\n\n";

        // 1. Get user inputs with validation
        double initialInvestment = GetNonNegativeDouble("Initial Investment Amount: $");
        double monthlyDeposit = GetNonNegativeDouble("Monthly Deposit: $");
        double annualInterest = GetNonNegativeDouble("Annual Interest (in %): ");
        int    numYears = GetPositiveInt("Number of years: ");

        // 2. Pause before showing the reports so the user can review input
        cout << "\nPress ENTER to continue to the reports...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // clear leftover newline
        cin.get();                                            // wait for actual Enter

        // 3. Create the InvestmentAccount object that will perform calculations
        InvestmentAccount account(initialInvestment,
            monthlyDeposit,
            annualInterest,
            numYears);

        // 4. Display the two required reports
        cout << "\n\n";
        account.DisplayReportWithoutMonthlyDeposits();

        cout << "\n\n";
        account.DisplayReportWithMonthlyDeposits();

        // 5. Ask the user whether to run another scenario
        cout << "\n\nWould you like to calculate another investment? (y/n): ";
        cin >> runAgain;
    }

    cout << "\nThank you for using the Airgead Banking Investment Calculator!\n";
    return 0;
}
