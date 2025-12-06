/*
 * BlackJackCore
 * Author: Gustavo Torres
 *
 * Rules:
 *  - BlackJackCore - Personal project by Gustavo Torres
 *  - Copyright (c) 2025 Gustavo Torres. All rights reserved.
 *  - This code is for educational and portfolio purposes only.
 *  - 6- or 8-deck shoe (selectable from menu)
 *  - American hole-card blackjack (dealer peeks for blackjack)
 *  - Dealer hits soft 17
 *  - Player can double down on any 2-card hard total from 2 to 11
 *  - Player can split all pairs (10-valued cards treated as same), aces only once
 *  - If dealer shows Ace, offers insurance up to half the bet, pays 2:1
 *  - Blackjack pays 3:2
 *  - Shoe automatically reshuffles when 75% of cards have been dealt
 *  - Table min bet: 5, table max bet: 3000, max buy-in: 10000
 */

#include <fstream>
#include <string>
#include <limits>
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>


using namespace std;

// ====================== CONFIG ======================

const double MIN_BET = 5.0;      // Table minimum
const double TABLE_MAX_BET = 3000.0;   // Table maximum bet per hand (initial bet)
const double MAX_BUYIN = 10000.0;  // Maximum starting bankroll

// ====================== STATS ======================

struct Stats {
    int handsPlayed = 0;
    int handsWon = 0;
    int handsLost = 0;
    int handsPushed = 0;
    int blackjacks = 0;
};

// ====================== CARD TYPES ======================

struct Card {
    int rank;  // 1 = Ace, 2–10, 11 = J, 12 = Q, 13 = K
    int suit;  // 0 = Clubs, 1 = Diamonds, 2 = Hearts, 3 = Spades
};

string CardToString(const Card& c) {
    static const string rankNames[] = {
        "A","2","3","4","5","6","7","8","9","10","J","Q","K"
    };
    static const string suitNames[] = {
        "C","D","H","S"
    };
    return rankNames[c.rank - 1] + suitNames[c.suit];
}

class Deck {
public:
    Deck(int numDecks = 6) : numDecks(numDecks) {
        BuildShoe();
    }

    void SetNumDecks(int newNumDecks) {
        numDecks = newNumDecks;
        BuildShoe();
    }

    void BuildShoe() {
        cards.clear();
        for (int d = 0; d < numDecks; ++d) {
            for (int s = 0; s < 4; ++s) {
                for (int r = 1; r <= 13; ++r) {
                    Card c;
                    c.rank = r;
                    c.suit = s;
                    cards.push_back(c);
                }
            }
        }
        totalCards = static_cast<int>(cards.size());
        Shuffle();
    }

    void Shuffle() {
        static random_device rd;
        static mt19937 gen(rd());
        shuffle(cards.begin(), cards.end(), gen);
        index = 0;
    }

    Card Deal() {
        if (index >= static_cast<int>(cards.size())) {
            BuildShoe();
        }
        return cards[index++];
    }

    int Remaining() const {
        return static_cast<int>(cards.size()) - index;
    }

    // Auto-shuffle when 75% of the shoe has been used
    bool NeedsShuffle() const {
        if (totalCards == 0) return false;
        int remaining = Remaining();
        return remaining <= static_cast<int>(totalCards * 0.25); // 25% or less remaining
    }

    int GetNumDecks() const {
        return numDecks;
    }

private:
    vector<Card> cards;
    int index = 0;
    int numDecks = 6;
    int totalCards = 0;
};

class Hand {
public:
    void AddCard(const Card& c) {
        cards.push_back(c);
    }

    void Clear() {
        cards.clear();
    }

    // Returns best total, using Aces as 1 or 11
    int GetValue(bool* isSoftOut = nullptr) const {
        int total = 0;
        int aceCount = 0;

        for (const Card& c : cards) {
            if (c.rank >= 2 && c.rank <= 10) {
                total += c.rank;
            }
            else if (c.rank >= 11 && c.rank <= 13) {
                total += 10;   // J,Q,K
            }
            else { // Ace
                total += 1;
                ++aceCount;
            }
        }

        int aceOriginal = aceCount;
        while (aceCount > 0 && total + 10 <= 21) {
            total += 10;
            --aceCount;
        }

        if (isSoftOut != nullptr) {
            *isSoftOut = (aceOriginal != aceCount); // at least one Ace counted as 11
        }

        return total;
    }

    bool IsBlackjack() const {
        return cards.size() == 2 && GetValue(nullptr) == 21;
    }

    bool IsPair() const {
        if (cards.size() != 2) return false;
        int v1 = CardValueForPair(cards[0]);
        int v2 = CardValueForPair(cards[1]);
        return v1 == v2;
    }

    const vector<Card>& GetCards() const {
        return cards;
    }

    void Print(bool hideFirst = false) const {
        for (size_t i = 0; i < cards.size(); ++i) {
            if (i == 0 && hideFirst) {
                cout << "[Hidden] ";
            }
            else {
                cout << CardToString(cards[i]) << " ";
            }
        }
        if (!hideFirst) {
            cout << "(" << GetValue(nullptr) << ")";
        }
        cout << endl;
    }

private:
    vector<Card> cards;

    // For pair logic, treat all 10-value cards as equal, Ace as 11
    static int CardValueForPair(const Card& c) {
        if (c.rank >= 11 && c.rank <= 13) return 10; // J,Q,K -> 10
        if (c.rank == 1) return 11;                  // Ace
        return c.rank;
    }
};

// ====================== INPUT HELPERS ======================

void ClearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

double GetBet(double bankroll) {
    double bet;
    while (true) {
        cout << "Bankroll: $" << bankroll << endl;
        cout << "Enter your bet (min $" << MIN_BET
            << ", max $" << TABLE_MAX_BET
            << ", 0 to go back to menu): ";
        if (!(cin >> bet)) {
            ClearInput();
            cout << "Invalid input.\n";
            continue;
        }
        if (bet == 0) {
            return 0;
        }
        if (bet < MIN_BET) {
            cout << "Bet must be at least $" << MIN_BET << ".\n";
            continue;
        }
        if (bet > TABLE_MAX_BET) {
            cout << "Bet cannot exceed table max of $" << TABLE_MAX_BET << ".\n";
            continue;
        }
        if (bet > bankroll) {
            cout << "You cannot bet more than your bankroll.\n";
            continue;
        }
        return bet;
    }
}

char GetChoice(const string& prompt, const string& valid) {
    char c;
    while (true) {
        cout << prompt;
        if (!(cin >> c)) {
            ClearInput();
            continue;
        }
        c = static_cast<char>(tolower(c));
        if (valid.find(c) != string::npos) {
            return c;
        }
        cout << "Invalid choice.\n";
    }
}

// ====================== ENUMS & HELPERS ======================

enum Outcome {
    LOSS,
    PUSH,
    WIN
};

// HARD total (all Aces as 1) – used for double-down rule 2–11
int GetHardValue(const Hand& h) {
    int total = 0;
    for (const Card& c : h.GetCards()) {
        if (c.rank >= 2 && c.rank <= 10) {
            total += c.rank;
        }
        else if (c.rank >= 11 && c.rank <= 13) {
            total += 10;  // J,Q,K
        }
        else { // Ace
            total += 1;
        }
    }
    return total;
}

Outcome ResolveHandVsDealer(
    const Hand& player,
    double& bet,
    const Hand& dealer,
    double& bankroll
) {
    int playerVal = player.GetValue(nullptr);
    int dealerVal = dealer.GetValue(nullptr);

    cout << "Dealer hand: ";
    dealer.Print(false);
    cout << "Your hand:   ";
    player.Print(false);

    Outcome result = PUSH;

    if (playerVal > 21) {
        cout << "You bust. Lose $" << bet << endl;
        result = LOSS;
    }
    else if (dealerVal > 21) {
        cout << "Dealer busts. You win $" << bet << endl;
        bankroll += bet * 2; // return bet + win
        result = WIN;
    }
    else if (playerVal > dealerVal) {
        cout << "You win $" << bet << endl;
        bankroll += bet * 2;
        result = WIN;
    }
    else if (playerVal < dealerVal) {
        cout << "Dealer wins. Lose $" << bet << endl;
        result = LOSS;
    }
    else {
        cout << "Push. Your bet is returned." << endl;
        bankroll += bet; // return bet
        result = PUSH;
    }

    return result;
}

void ShowRules(int numDecks) {
    cout << "\n===== BlackJackCore RULES =====\n";
    cout << "- Using a " << numDecks << "-deck shoe (you can switch between 6 and 8 in the menu).\n";
    cout << "- Shoe automatically reshuffles when 75% of cards have been dealt.\n";
    cout << "- American hole-card: Dealer takes a down card and peeks for blackjack.\n";
    cout << "- Blackjack pays 3:2.\n";
    cout << "- Dealer hits soft 17 (e.g., A + 6).\n";
    cout << "- You can double down on any 2-card hard total from 2 to 11.\n";
    cout << "  (10, J, Q, K all count as 10. Aces counted as 1 for this rule.)\n";
    cout << "- You can split any pair, including tens (J/Q/K/10 treated as same).\n";
    cout << "- If you split Aces, each Ace receives exactly one card and you cannot hit.\n";
    cout << "- If dealer shows an Ace, you may take insurance up to half your bet.\n";
    cout << "  Insurance pays 2:1 if dealer has blackjack.\n";
    cout << "- Table minimum bet: $" << MIN_BET << "\n";
    cout << "- Table maximum bet: $" << TABLE_MAX_BET << " (initial bet).\n";
    cout << "- Maximum buy-in:    $" << MAX_BUYIN << "\n";
    cout << "================================\n\n";
}

void ShowStats(const Stats& stats, double bankroll, const string& playerName) {
    cout << "\n===== BlackJackCore SESSION STATS =====\n";
    cout << "Player:         " << playerName << endl;
    cout << "Hands played:   " << stats.handsPlayed << endl;
    cout << "Hands won:      " << stats.handsWon << endl;
    cout << "Hands lost:     " << stats.handsLost << endl;
    cout << "Hands pushed:   " << stats.handsPushed << endl;
    cout << "Blackjacks:     " << stats.blackjacks << endl;
    cout << "Current bankroll: $" << bankroll << endl;
    cout << "Table min bet:  $" << MIN_BET << endl;
    cout << "Table max bet:  $" << TABLE_MAX_BET << endl;
    cout << "Max buy-in:     $" << MAX_BUYIN << endl;
    cout << "=======================================\n\n";
}

// ====================== SAVE / LOAD ======================

const string SAVE_FILE = "blackjackcore_save.txt";

bool LoadSession(string& playerName, double& bankroll, int& numDecks, Stats& stats) {
    ifstream in(SAVE_FILE);
    if (!in) return false;

    string savedName;
    double savedBankroll;
    int savedDecks;
    Stats s;

    getline(in, savedName);
    if (!(in >> savedBankroll)) return false;
    if (!(in >> savedDecks)) return false;
    if (!(in >> s.handsPlayed >> s.handsWon >> s.handsLost
        >> s.handsPushed >> s.blackjacks)) {
        return false;
    }

    cout << "\nFound previous BlackJackCore session:\n";
    cout << "  Player:   " << savedName << "\n";
    cout << "  Bankroll: $" << savedBankroll << "\n";
    cout << "  Decks:    " << savedDecks << "\n";
    cout << "  Hands:    " << s.handsPlayed << " ("
        << s.handsWon << "W / " << s.handsLost << "L / "
        << s.handsPushed << "P, BJ: " << s.blackjacks << ")\n";

    char c = GetChoice("Load this session? (y/n): ", "yn");
    if (c == 'y') {
        playerName = savedName;
        bankroll = savedBankroll;
        numDecks = savedDecks;
        stats = s;
        return true;
    }
    return false;
}

void SaveSession(const string& playerName, double bankroll, int numDecks, const Stats& stats) {
    ofstream out(SAVE_FILE);
    if (!out) {
        cout << "Warning: Could not save BlackJackCore session.\n";
        return;
    }
    out << playerName << "\n";
    out << bankroll << "\n";
    out << numDecks << "\n";
    out << stats.handsPlayed << " "
        << stats.handsWon << " "
        << stats.handsLost << " "
        << stats.handsPushed << " "
        << stats.blackjacks << "\n";
}

// ====================== MAIN ROUND LOGIC ======================

void PlayBlackjackRound(double& bankroll, Deck& deck, Stats& stats) {
    // Auto-shuffle when 75% of shoe has been used
    if (deck.NeedsShuffle()) {
        cout << "\nShoe penetration reached 75%. Reshuffling BlackJackCore shoe...\n";
        deck.BuildShoe();
    }

    double bet = GetBet(bankroll);
    if (bet == 0) {
        cout << "Hand cancelled. Returning to table.\n";
        return;
    }

    bankroll -= bet;

    Hand dealer;
    Hand player;

    // Initial deal (player, dealer up, player, dealer hole)
    player.AddCard(deck.Deal());
    dealer.AddCard(deck.Deal());   // up card
    player.AddCard(deck.Deal());
    dealer.AddCard(deck.Deal());   // hole card

    cout << "\n===== New BlackJackCore Hand =====\n";
    cout << "Dealer shows: ";
    dealer.Print(true); // hide first
    cout << "Your hand:    ";
    player.Print(false);

    bool dealerHasBlackjack = dealer.IsBlackjack();
    bool playerHasBlackjack = player.IsBlackjack();

    // ---------- Insurance (dealer showing Ace) ----------
    double insuranceBet = 0.0;
    const vector<Card>& dealerCards = dealer.GetCards();
    // Up-card is index 1 when using hideFirst=true
    if (dealerCards[1].rank == 1) { // Ace
        if (bankroll >= bet / 2.0) {
            char ins = GetChoice("Dealer shows Ace. Take insurance (y/n)? ", "yn");
            if (ins == 'y') {
                insuranceBet = bet / 2.0;
                bankroll -= insuranceBet;
                cout << "You placed insurance: $" << insuranceBet << endl;
            }
        }
    }

    // ---------- Dealer peek for blackjack ----------
    if (dealerHasBlackjack) {
        cout << "\nDealer peeks and has BLACKJACK!\n";
        cout << "Dealer hand: ";
        dealer.Print(false);

        // Resolve insurance
        if (insuranceBet > 0) {
            cout << "Insurance pays 2:1. You win $" << insuranceBet * 2 << endl;
            bankroll += insuranceBet * 3; // stake + 2x profit
        }

        stats.handsPlayed++;

        if (playerHasBlackjack) {
            cout << "You also have blackjack. Push on main bet.\n";
            bankroll += bet; // return main bet
            stats.handsPushed++;
            stats.blackjacks++; // you had a blackjack
        }
        else {
            cout << "You lose your main bet of $" << bet << endl;
            stats.handsLost++;
        }
        return;
    }
    else {
        if (insuranceBet > 0) {
            cout << "Dealer does NOT have blackjack. Insurance loses.\n";
        }
    }

    // ---------- Natural blackjack for player ----------
    if (playerHasBlackjack) {
        cout << "\nYou have BLACKJACK!\n";
        cout << "Your hand: ";
        player.Print(false);
        cout << "Dealer hand: ";
        dealer.Print(false);

        double payout = bet * 2.5; // 3:2 payout
        cout << "Blackjack pays 3:2. You receive $" << payout - bet << " profit.\n";
        bankroll += payout;

        stats.handsPlayed++;
        stats.handsWon++;
        stats.blackjacks++;
        return;
    }

    // ---------- Splitting logic (max one split) ----------
    vector<Hand> playerHands;
    vector<double> handBets;
    vector<bool> isSplitAces;

    playerHands.push_back(player);
    handBets.push_back(bet);
    isSplitAces.push_back(false);

    // Can we split?
    if (player.IsPair() && bankroll >= bet) {
        cout << "\nYou have a pair. ";
        char sp = GetChoice("Do you want to split (y/n)? ", "yn");
        if (sp == 'y') {
            Hand first;
            Hand second;
            const vector<Card>& cards = player.GetCards();
            first.AddCard(cards[0]);
            second.AddCard(cards[1]);

            bankroll -= bet;              // second hand bet

            playerHands.clear();
            handBets.clear();
            isSplitAces.clear();

            playerHands.push_back(first);
            handBets.push_back(bet);

            playerHands.push_back(second);
            handBets.push_back(bet);

            bool splitAce = (cards[0].rank == 1 && cards[1].rank == 1);

            isSplitAces.push_back(splitAce);
            isSplitAces[0] = splitAce;

            if (splitAce) {
                cout << "\nSplitting Aces. Each Ace gets one card only.\n";
                for (size_t i = 0; i < playerHands.size(); ++i) {
                    playerHands[i].AddCard(deck.Deal());
                }
            }
            else {
                // Non-ace pairs: deal one new card to each
                for (size_t i = 0; i < playerHands.size(); ++i) {
                    playerHands[i].AddCard(deck.Deal());
                }
            }
        }
    }

    // ---------- Play each player hand ----------
    for (size_t h = 0; h < playerHands.size(); ++h) {
        Hand& hand = playerHands[h];
        double& hBet = handBets[h];

        cout << "\n--- Playing Hand " << (h + 1) << " ---\n";

        // If this is a split aces hand: no hits, just show
        if (isSplitAces[h]) {
            cout << "Split Aces hand: ";
            hand.Print(false);
            continue;
        }

        bool done = false;
        bool firstAction = true;

        while (!done) {
            int softTotal = hand.GetValue(nullptr);   // display total (with Aces as 1 or 11)
            int hardTotal = GetHardValue(hand);       // used for double-down rule
            cout << "Your hand: ";
            hand.Print(false);

            if (softTotal > 21) {
                cout << "You bust!\n";
                break;
            }

            // Options: hit/stand, maybe double (hard total 2–11)
            string options = "hs";
            bool canDouble =
                firstAction &&
                hardTotal >= 2 &&
                hardTotal <= 11 &&
                bankroll >= hBet;

            if (canDouble) options += "d";

            cout << "Options: (h)it, (s)tand";
            if (canDouble) cout << ", (d)ouble";
            cout << "  [Hard total: " << hardTotal << "]\n";

            char choice = GetChoice("Choose: ", options);

            if (choice == 'h') {
                hand.AddCard(deck.Deal());
                firstAction = false;
            }
            else if (choice == 's') {
                done = true;
            }
            else if (choice == 'd' && canDouble) {
                bankroll -= hBet;
                hBet *= 2;
                hand.AddCard(deck.Deal());
                cout << "You doubled down. Final hand: ";
                hand.Print(false);
                done = true;
            }
        }
    }

    // ---------- Dealer's turn ----------
    cout << "\nDealer reveals hand: ";
    dealer.Print(false);

    bool dealerSoft = false;
    int dealerVal = dealer.GetValue(&dealerSoft);
    cout << "Dealer total: " << dealerVal
        << (dealerSoft ? " (soft)" : "") << endl;

    while (dealerVal < 17 || (dealerVal == 17 && dealerSoft)) {
        cout << "Dealer hits.\n";
        dealer.AddCard(deck.Deal());
        dealerVal = dealer.GetValue(&dealerSoft);
        cout << "Dealer hand: ";
        dealer.Print(false);
        cout << "Dealer total: " << dealerVal
            << (dealerSoft ? " (soft)" : "") << endl;
    }

    if (dealerVal > 21) {
        cout << "Dealer busts.\n";
    }
    else {
        cout << "Dealer stands.\n";
    }

    // ---------- Resolve each hand & update stats ----------
    for (size_t h = 0; h < playerHands.size(); ++h) {
        cout << "\n=== Result for Hand " << (h + 1) << " ===\n";
        stats.handsPlayed++;
        Outcome o = ResolveHandVsDealer(playerHands[h], handBets[h], dealer, bankroll);
        if (o == WIN) {
            stats.handsWon++;
        }
        else if (o == LOSS) {
            stats.handsLost++;
        }
        else {
            stats.handsPushed++;
        }
    }
}

// ====================== MAIN ======================

int main() {
    string playerName;
    cout << "Welcome to BlackJackCore!\n";
    cout << "Enter your name: ";
    cin >> ws;
    getline(cin, playerName);
    if (playerName.empty()) {
        playerName = "Player";
    }

    int currentNumDecks = 6;
    double bankroll = 0.0;
    Stats stats;

    // Try loading previous session (if file exists)
    bool loaded = LoadSession(playerName, bankroll, currentNumDecks, stats);

    if (!loaded) {
        cout << "\nWelcome, " << playerName << "!" << endl;
        cout << "BlackJackCore default: " << currentNumDecks
            << "-deck shoe. Dealer hits soft 17. Blackjack pays 3:2.\n\n";

        cout << "Enter starting bankroll (between $" << MIN_BET
            << " and $" << MAX_BUYIN << "): $";
        while (!(cin >> bankroll) || bankroll < MIN_BET || bankroll > MAX_BUYIN) {
            ClearInput();
            cout << "Please enter a value between $" << MIN_BET
                << " and $" << MAX_BUYIN << ": $";
        }
    }
    else {
        cout << "\nLoaded previous BlackJackCore session for " << playerName << ".\n";
    }

    Deck deck(currentNumDecks);

    bool running = true;
    while (running) {
        cout << "\n===== BLACKJACKCORE MAIN MENU =====\n";
        cout << "1) Sit at table / play hands\n";
        cout << "2) View rules\n";
        cout << "3) View stats\n";
        cout << "4) Change number of decks (current: " << deck.GetNumDecks() << ")\n";
        cout << "5) Quit\n";
        cout << "===================================\n";

        char choice = GetChoice("Choose an option (1-5): ", "12345");

        if (choice == '1') {
            if (bankroll < MIN_BET) {
                cout << "You don't have enough for the table minimum. Game over.\n";
                break;
            }

            bool keepPlaying = true;
            while (keepPlaying) {
                PlayBlackjackRound(bankroll, deck, stats);

                if (bankroll < MIN_BET) {
                    cout << "\nYou don't have enough for the table minimum anymore.\n";
                    cout << "Leaving the BlackJackCore table.\n";
                    keepPlaying = false;
                    break;
                }

                char again = GetChoice("Play another hand? (y/n): ", "yn");
                if (again == 'n') {
                    keepPlaying = false;
                }
            }
        }
        else if (choice == '2') {
            ShowRules(deck.GetNumDecks());
        }
        else if (choice == '3') {
            ShowStats(stats, bankroll, playerName);
        }
        else if (choice == '4') {
            cout << "Enter number of decks (6 or 8): ";
            int nd;
            if (!(cin >> nd)) {
                ClearInput();
                cout << "Invalid input.\n";
            }
            else if (nd != 6 && nd != 8) {
                cout << "Only 6 or 8 decks are allowed.\n";
            }
            else {
                deck.SetNumDecks(nd);
                currentNumDecks = nd;
                cout << "Deck configuration updated to " << nd << " decks.\n";
            }
        }
        else if (choice == '5') {
            running = false;
        }
    }

    cout << "\n" << playerName
        << ", you leave the BlackJackCore table with $"
        << bankroll << endl;
    ShowStats(stats, bankroll, playerName);

    // Save session on exit
    SaveSession(playerName, bankroll, currentNumDecks, stats);
    cout << "Session saved to '" << SAVE_FILE << "'.\n";
    cout << "Thanks for playing BlackJackCore!\n";
    return 0;
}

