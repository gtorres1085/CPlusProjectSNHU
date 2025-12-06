/*
 * Airgead Banking Investment Calculator
 * -------------------------------------
 * Author: Gustavo Torres
 * Course: CS-210
 * Professor: Barry Pollack
 *
 * Implementation file for the InvestmentAccount class.
 * Contains the logic to calculate yearly balances and interest
 * for an investment with and without monthly deposits.
 */

#include "InvestmentAccount.h"
#include <iostream>
#include <iomanip>   // setw, setprecision, fixed

using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;
using std::setw;

// ---------------------------------------------------------
// Constructor: store all input parameters
// ---------------------------------------------------------
InvestmentAccount::InvestmentAccount(double initialInvestment,
    double monthlyDeposit,
    double annualInterestRate,
    int numYears)
{
    m_initialInvestment = initialInvestment;
    m_monthlyDeposit = monthlyDeposit;
    m_annualInterestRate = annualInterestRate;
    m_numYears = numYears;
}

// ---------------------------------------------------------
// DisplayReportWithoutMonthlyDeposits
// Calculates and displays year-end balances and interest
// assuming no additional monthly deposits are made.
// Interest is applied once per year.
// ---------------------------------------------------------
void InvestmentAccount::DisplayReportWithoutMonthlyDeposits() const {
    double currentBalance = m_initialInvestment;
    double yearlyRate = m_annualInterestRate / 100.0;   // convert percent to decimal

    cout << "====================================================\n";
    cout << "  Balance and Interest Without Monthly Deposits\n";
    cout << "====================================================\n";
    cout << "Year        Year End Balance      Year End Earned Interest\n";
    cout << "----------------------------------------------------------\n";

    cout << fixed << setprecision(2);

    // Loop once per year
    for (int year = 1; year <= m_numYears; ++year) {
        double yearInterest = currentBalance * yearlyRate;  // simple annual interest
        currentBalance += yearInterest;                     // add interest to balance

        cout << setw(4) << year
            << setw(18) << currentBalance
            << setw(24) << yearInterest << endl;
    }
}

// ---------------------------------------------------------
// DisplayReportWithMonthlyDeposits
// Calculates and displays year-end balances and interest
// assuming a fixed deposit is made at the beginning of each
// month, and then interest is applied monthly.
// ---------------------------------------------------------
void InvestmentAccount::DisplayReportWithMonthlyDeposits() const {
    double currentBalance = m_initialInvestment;
    double monthlyRate = (m_annualInterestRate / 100.0) / 12.0; // monthly rate

    cout << "====================================================\n";
    cout << "   Balance and Interest With Monthly Deposits\n";
    cout << "====================================================\n";
    cout << "Year        Year End Balance      Year End Earned Interest\n";
    cout << "----------------------------------------------------------\n";

    cout << fixed << setprecision(2);

    // Outer loop: one pass per year
    for (int year = 1; year <= m_numYears; ++year) {
        double yearInterest = 0.0;

        // Inner loop: compound monthly
        for (int month = 1; month <= 12; ++month) {
            // Deposit occurs at the beginning of the month
            currentBalance += m_monthlyDeposit;

            // Interest is calculated on the updated balance
            double monthInterest = currentBalance * monthlyRate;
            currentBalance += monthInterest;       // add monthly interest
            yearInterest += monthInterest;       // track interest for this year
        }

        cout << setw(4) << year
            << setw(18) << currentBalance
            << setw(24) << yearInterest << endl;
    }
}
