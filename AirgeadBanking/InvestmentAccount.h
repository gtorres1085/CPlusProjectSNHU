/*
 * Airgead Banking Investment Calculator
 * -------------------------------------
 * Author: Gustavo Torres
 * Course: CS-210
 * Professor: Barry Pollack
 *
 * Header file for the InvestmentAccount class.
 * This class stores the investment parameters and provides
 * functions to print reports with and without monthly deposits.
 */

#ifndef INVESTMENTACCOUNT_H
#define INVESTMENTACCOUNT_H

class InvestmentAccount {
public:
    // Constructor: initialize all data members
    InvestmentAccount(double initialInvestment,
        double monthlyDeposit,
        double annualInterestRate,
        int numYears);

    // Display the yearly balances and interest
    // when NO additional monthly deposits are made
    void DisplayReportWithoutMonthlyDeposits() const;

    // Display the yearly balances and interest
    // when monthly deposits ARE made
    void DisplayReportWithMonthlyDeposits() const;

private:
    // Initial lump-sum investment amount
    double m_initialInvestment;

    // Fixed amount deposited at the beginning of every month
    double m_monthlyDeposit;

    // Annual interest rate as a percentage (for example, 4.5 for 4.5%)
    double m_annualInterestRate;

    // Number of years the investment will grow
    int m_numYears;
};

#endif // INVESTMENTACCOUNT_H

