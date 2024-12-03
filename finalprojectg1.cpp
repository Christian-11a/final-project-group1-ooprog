#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <limits>
#include <cctype>

using namespace std;

// Function to check if a file is empty
bool isFileEmpty(const string& filename) {
    ifstream file(filename, ios::ate);
    return file.tellg() == 0;
}

// Function to pause and clearing screen
void sysclear(){
	system("pause");
    system("cls");
}

// Input validation function for menu
int getValidatedChoice(int minChoice, int maxChoice) {
    string input;
    while (true) {
        cout << "Enter your choice: ";
        getline(cin, input); 

        // Check if the input is empty or contains only whitespace
        if (input.empty() || input.find_first_not_of(' ') == string::npos) {
            cout << "Invalid input. Please enter a number between " << minChoice << " and " << maxChoice << ".\n";
            sysclear();
            return -1;
        }

        // Check if the input is numeric and contains no extra characters
        bool isValid = true;
        for (char c : input) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            // Convert valid numeric string to an integer and ensure no leading zeros
            try {
                int choice = stoi(input);
                if (to_string(choice) != input) { // Check for leading zeros
                    throw invalid_argument("Leading zeros detected");
                }
                if (choice >= minChoice && choice <= maxChoice) {
                    return choice; // Valid choice
                }
            } catch (...) {
                // Catch exceptions from stoi or manual checks
            }
        }

        cout << "Invalid input. Please enter a number between " << minChoice << " and " << maxChoice << ".\n";
        sysclear();
        return -1;
    }
}

// Input validation function for double inputs
double getValidatedDouble(const string& prompt, double minValue) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail() || value < minValue) {
            cout << "Invalid input. Please enter a value greater than or equal to " << minValue << ".\n";
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the input buffer
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear trailing input
        return value;
    }
}

// For storing account information
struct Account {
    int userID;
    string username;
    string password;
};

//For storing transaction details
struct Transaction {
    int transactionID;
    string category;
    string itemname;
    string type;
    int qty;
    double price; 
    double amount;
};

// Class that use Singleton pattern for user management
class ManageAccount {
private:
    static ManageAccount* instance; //Singleton here
    int userID = 1;
    int currentUserID; 

    // Private constructor
    ManageAccount() { 
        // Load userID
        ifstream idFile("userID.txt");
        if (idFile.is_open()) {
            idFile >> userID;
            idFile.close();
        }

        // Load accounts
        loadAccounts();
    }

    friend class Admin; // For admin class to have special access
public:
    // The getter function
    static ManageAccount* getInstance() {
        if (!instance) {
            instance = new ManageAccount();
        }
        return instance;
    }

    string loggedInUser;
    vector<Account> accounts;
    string username, password;

    // For loading all accounts
    void loadAccounts() {
        accounts.clear();
        ifstream accountFile("accounts.txt");
        if (accountFile.is_open()) {
            string line;
            getline(accountFile, line); // Skip header
            getline(accountFile, line); // Skip separator
            
            while (getline(accountFile, line)) {
                Account acc;
                stringstream ss(line);
                ss >> acc.userID >> acc.username >> acc.password;
                accounts.push_back(acc);
            }
            accountFile.close();
        }
    }

    int getCurrentUserID() const {
        return currentUserID;
    }
    
    // For updating file in the current data
    void overwriteAccountsFile() {
        ofstream file("accounts.txt");
        if (file.is_open()) {
            file << setw(20) << left << "User ID"
                 << setw(20) << left << "Username"
                 << setw(15) << left << "Password" << endl;
            file << string(50, '-') << endl;

            for (const auto& account : accounts) {
                file << setw(20) << left << account.userID
                     << setw(20) << left << account.username
                     << setw(15) << left << account.password
                     << endl;
            }
            file.close();
        }
    }

    // For saving userID
    void saveUserID() {
        ofstream file("userID.txt");
        if (file.is_open()) {
            file << userID;
            file.close();
        }
    }

    // For adding new user to account file
    void saveUserToFile(int id, const string& username, const string& password) {
        string accountFile = "accounts.txt";
        ofstream file(accountFile, ios::app);
        if (file.is_open()) {
            if (isFileEmpty(accountFile)) {
                file << setw(20) << left << "User ID"
                     << setw(20) << left << "Username"
                     << setw(15) << left << "Password" << endl;
                file << string(50, '-') << endl;
            }
            file << setw(20) << left << id
                 << setw(20) << left << username
                 << setw(15) << left << password
                 << endl;
            file.close();
        }
    }

    // For creating account of regular user
    void createAccount() {
        cout << "Enter Username (minimum 4 characters): ";
        getline(cin, username);

        loadAccounts();

        if (username.length() < 4) {
            cout << "Username must be at least 4 characters long.\n";
            return;
        }

        if (username == "Admin"){
            cout << "Admin account cannot be created.\n";
            return;
        }
        
        for (const auto& account : accounts) {
            if (account.username == username) {
                cout << "Username already exists. Try again.\n";
                return;
            }
        }

        cout << "Enter Password (minimum 6 characters): ";
        getline(cin, password);

        if (password.length() < 6) {
            cout << "Password must be at least 6 characters long.\n";
            return;
        }

        // For saving the account into the file
        accounts.push_back({userID, username, password});
        saveUserToFile(userID, username, password);

        userID++;
        saveUserID();

        cout << "Account created successfully!\n";
    }
    
    // For Login
    virtual void logIn() {
        bool ok = false;
        while (!ok) {
            cout << "Enter Username: ";
            getline(cin, username);
            cout << "Enter Password: ";
            getline(cin, password);

            loadAccounts();
            
            // For admin login
            if (username == "Admin" && password == "1234") {
                cout << "Admin login successful!" << endl;
                loggedInUser = "Admin";
                currentUserID = 0; 
                ok = true;
            } else {
                for (const auto& account : accounts) {
                    if (account.username == username && account.password == password) {
                        cout << "Login successful!" << endl;
                        loggedInUser = username;
                        currentUserID = account.userID;
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    char retry;
                    cout << "Incorrect username or password! Would you like to try again? (y/n): ";
                    cin >> retry;
                    cin.ignore();
                    if (tolower(retry) == 'n') {
                        cout << "Returning to the menu..." << endl;
                        return;
                    }
                }
            }
        }
    }
};

ManageAccount* ManageAccount::instance = nullptr; // Initialize the static instance pointer

// Helper class for transactions
class TransactionHelper {
public:

// For loading the transaction from the file
 static void loadTransactionsFromFile(int userID, vector<Transaction>& transactions) {
        transactions.clear(); 
        string filename = to_string(userID) + "_transactions.txt";
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error opening file for reading. No transactions loaded.\n";
            return;
        }

        string line;
        // Skip header lines
        getline(file, line);
        getline(file, line);

        while (getline(file, line)) {
            stringstream ss(line);
            Transaction t;

            ss >> t.transactionID;
            ss >> t.category;
            ss >> t.itemname;
            ss >> t.qty;
            ss >> t.price;
            ss >> t.amount;

            transactions.push_back(t);
        }

        file.close();
    }
    static void displayTransactions(const vector<Transaction>& transactions) {
        if (transactions.empty()) {
            cout << "No transactions to display.\n";
            return;
        }

        cout << setw(10) << left << "ID"
             << setw(20) << left << "Category"
             << setw(20) << left << "Item Name"
             << setw(15) << left << "Quantity"
             << setw(15) << left << "Price"
             << setw(15) << left << "Amount" << endl;
        cout << string(90, '-') << endl;

        for (const auto& t : transactions) {
            cout << setw(10) << left << t.transactionID
                 << setw(20) << left << t.category
                 << setw(20) << left << t.itemname
                 << setw(15) << left << t.qty
                 << setw(15) << left << fixed << setprecision(2) << t.price
                 << setw(15) << left << fixed << setprecision(2) << t.amount
                 << endl;
        }
    }

    // For saving the transactions of the user
    static void saveTransactionsToFile(int userID, const vector<Transaction>& transactions) {
        string filename = to_string(userID) + "_transactions.txt";
        ofstream file(filename);

        if (!file.is_open()) {
            cerr << "Error opening file for writing.\n";
            return;
        }

        if (!transactions.empty()) {
            file << setw(10) << left << "ID"
                 << setw(20) << left << "Category"
                 << setw(20) << left << "Item Name"
                 << setw(15) << left << "Quantity"
                 << setw(15) << left << "Price"
                 << setw(15) << left << "Amount" << endl;
            file << string(90, '-') << endl;
        }

        for (const auto& t : transactions) {
            file << setw(10) << left << t.transactionID
                 << setw(20) << left << t.category
                 << setw(20) << left << t.itemname
                 << setw(15) << left << t.qty
                 << setw(15) << left << fixed << setprecision(2) << t.price
                 << setw(15) << left << fixed << setprecision(2) << t.amount
                 << endl;
        }

        file.close();
    }
};

// Abstract base class for Transaction Strategy Pattern
class TransactionStrategy {
public:
    virtual void execute(int userID, vector<Transaction>& transactions) = 0;
    // Destructor for cleaning up
    virtual ~TransactionStrategy() {}
};

// Concrete class for adding new transactions
class AddTransaction : public TransactionStrategy {
private:

    // Get the current budget from the file
    double getRemainingBudget(int userID) {
        string filename = to_string(userID) + "_budget.txt";
        ifstream file(filename);
        double remainingBudget = 0.0;

        if (file.is_open()) {
            file >> remainingBudget;
            file.close();
        }

        return remainingBudget;
    }

    // For updating the budget after transactions
    void updateBudget(int userID, double newBudget) {
        string filename = to_string(userID) + "_budget.txt";
        ofstream file(filename);
        if (file.is_open()) {
            file << fixed << setprecision(2) << newBudget;
            file.close();
        }
    }

public:
    //Implementing the transaction strategy
    void execute(int userID, vector<Transaction>& transactions) override {
        system("cls");
        double remainingBudget = getRemainingBudget(userID);

        if (remainingBudget <= 0.0) {
            cout << "Cannot add a transaction. Budget is depleted. Please update your budget first.\n";
            return;
        }

        Transaction t;
        t.transactionID = transactions.size() + 1;
		
		bool looper = true;
		while(looper){
	        cout << "Select Category:\n";
	        cout << "1. FOOD\n";
	        cout << "2. TRANSPORTATION\n";
	        cout << "3. ENTERTAINMENT\n";
	        cout << "4. BILLS/SUBSCRIPTION\n";
	        cout << "5. MISCELLANEOUS\n";
	        int categoryChoice = getValidatedChoice(1, 5);
	        if (categoryChoice == -1) {
                system("cls");
            	continue;
			}
			
	        switch (categoryChoice) {
	            case 1: t.category = "FOOD"; break;
	            case 2: t.category = "TRANSPORTATION"; break;
	            case 3: t.category = "ENTERTAINMENT"; break;
	            case 4: t.category = "BILLS/SUBSCRIPTION"; break;
	            case 5: t.category = "MISCELLANEOUS"; break;
	            default:
	                cout << "Invalid choice. Transaction cannot be added.\n";
	                continue;
	        }
	        break;
		}
		
        cout << "Enter Item Name: ";
        getline(cin, t.itemname);

        t.qty = getValidatedDouble("Enter Quantity: ", 1.0);
        t.price = getValidatedDouble("Enter Price per Unit: ", 0.01);

        double totalAmount = t.qty * t.price;

        if (totalAmount > remainingBudget) {
            cout << "Transaction total amount exceeds the remaining budget. Transaction denied.\n";
            return;
        }

        t.amount = totalAmount;
        transactions.push_back(t);

        updateBudget(userID, remainingBudget - totalAmount);
        TransactionHelper::saveTransactionsToFile(userID, transactions);

        cout << "\nTransaction added successfully!\n";
        cout << "Remaining budget: $" << remainingBudget - totalAmount << "\n\n";

        TransactionHelper::displayTransactions(transactions);

        sysclear();
    }
};

// Concrete class for updating existing transactions
class UpdateTransaction : public TransactionStrategy {
private:
    double getRemainingBudget(int userID) {
        string filename = to_string(userID) + "_budget.txt";
        ifstream file(filename);
        double remainingBudget = 0.0;

        if (file.is_open()) {
            file >> remainingBudget;
            file.close();
        }

        return remainingBudget;
    }

    void updateBudget(int userID, double newBudget) {
        string filename = to_string(userID) + "_budget.txt";
        ofstream file(filename);
        if (file.is_open()) {
            file << fixed << setprecision(2) << newBudget;
            file.close();
        }
    }

public:
    void execute(int userID, vector<Transaction>& transactions) override {
        system("cls");
        int transactionID;

        while (true) {
            cout << "Enter Transaction ID to update: ";
            cin >> transactionID;

            if (cin.fail()) {
                cout << "Invalid input. Please enter a valid transaction ID.\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                sysclear();
                continue;
            }

            cin.ignore(); // Clear trailing newline
            break;
        }

        Transaction* transactionToUpdate = nullptr;

        for (auto& t : transactions) {
            if (t.transactionID == transactionID) {
                transactionToUpdate = &t;
                break;
            }
        }

        if (!transactionToUpdate) {
            cout << "Transaction ID not found.\n";
            return;
        }

        double previousAmount = transactionToUpdate->amount;

		bool looper = true;
        while (looper) {
            cout << "Select New Category:\n";
            cout << "1. FOOD\n";
            cout << "2. TRANSPORTATION\n";
            cout << "3. ENTERTAINMENT\n";
            cout << "4. BILLS/SUBSCRIPTION\n";
            cout << "5. MISCELLANEOUS\n";

            int categoryChoice = getValidatedChoice(1, 5);
            if (categoryChoice == -1) {
                system("cls");
                continue;
            }

            switch (categoryChoice) {
                case 1: transactionToUpdate->category = "FOOD"; break;
                case 2: transactionToUpdate->category = "TRANSPORTATION"; break;
                case 3: transactionToUpdate->category = "ENTERTAINMENT"; break;
                case 4: transactionToUpdate->category = "BILLS/SUBSCRIPTION"; break;
                case 5: transactionToUpdate->category = "MISCELLANEOUS"; break;
                default:
                    cout << "Invalid choice. Please try again.\n";
                    continue;
            }
            break;
        }

        cout << "Enter New Item Name: ";
        getline(cin, transactionToUpdate->itemname);

        transactionToUpdate->qty = getValidatedDouble("Enter New Quantity: ", 1.0);
        transactionToUpdate->price = getValidatedDouble("Enter New Price per Unit: ", 0.01);

        transactionToUpdate->amount = transactionToUpdate->price * transactionToUpdate->qty;

        double remainingBudget = getRemainingBudget(userID);
        double budgetDifference = transactionToUpdate->amount - previousAmount;

        if (remainingBudget - budgetDifference < 0) {
            cout << "Transaction update exceeds available budget. Update aborted.\n";
            sysclear();
            return;
        }

        remainingBudget -= budgetDifference;

        updateBudget(userID, remainingBudget);
        TransactionHelper::saveTransactionsToFile(userID, transactions);

        cout << "\nTransaction updated successfully!\n";
        cout << "Remaining budget: $" << fixed << setprecision(2) << remainingBudget << "\n\n";

        TransactionHelper::displayTransactions(transactions);

        sysclear();
    }
};

// Concrete class for deleting existing transactions
class DeleteTransaction : public TransactionStrategy {
private:
    double getRemainingBudget(int userID) {
        string filename =  to_string(userID) + "_budget.txt";
        ifstream file(filename);
        double remainingBudget = 0.0;

        if (file.is_open()) {
            file >> remainingBudget;
            file.close();
        }
        return remainingBudget;
    }

    void updateBudget(int userID, double newBudget) {
        string filename =  to_string(userID) + "_budget.txt";
        ofstream file(filename);
        if (file.is_open()) {
            file << fixed << setprecision(2) << newBudget;
            file.close();
        }
    }

public:
    void execute(int userID, vector<Transaction>& transactions) override {
        system("cls");
        int transactionID;
        cout << "Enter Transaction ID to delete: ";
        cin >> transactionID;
        cin.ignore();

        auto it = find_if(transactions.begin(), transactions.end(), 
            [transactionID](const Transaction& t) {
                return t.transactionID == transactionID;
            });

        if (it == transactions.end()) {
            cout << "Transaction ID not found.\n";
            return;
        }

        double remainingBudget = getRemainingBudget(userID);
        remainingBudget += it->amount;

        double refundedAmount = it->amount;
        transactions.erase(it);

        for (size_t i = 0; i < transactions.size(); ++i) {
            transactions[i].transactionID = i + 1;
        }

        updateBudget(userID, remainingBudget);
        TransactionHelper::saveTransactionsToFile(userID, transactions);

        cout << "Transaction deleted successfully!\n";
        cout << "Refunded amount: $" << refundedAmount << "\n";
        cout << "Remaining budget: $" << remainingBudget << "\n\n";

        TransactionHelper::displayTransactions(transactions);

        sysclear();
    }
};

// Concrete class for viewing transaction history
class ViewTransactionHistory : public TransactionStrategy {
public:
    void execute(int userID, vector<Transaction>& transactions) override {
        system("cls");
        // Load transactions from file
        TransactionHelper::loadTransactionsFromFile(userID, transactions);

        if (transactions.empty()) {
            cout << "No transactions found.\n";
        } else {
            TransactionHelper::displayTransactions(transactions);
        }

        sysclear();
    }
};


// Admin class extends to manageaccount
class Admin : public ManageAccount {
private:
    static Admin* instance;
    Admin() {}
public:
    static Admin* getInstance() {
        if (!instance) {
            instance = new Admin();
        }
        return instance;
    }

    // Override the login for providing admin menu
    void logIn() override {
        ManageAccount::getInstance()->logIn();
        
        if (loggedInUser == "Admin") {
            manageAdminMenu();
        }
    }

    // Admin menu options
    void manageAdminMenu() {
    	bool looper = true;
        while (looper) {
            cout << "\n--- Admin Menu ---\n";
            cout << "1. Manage Users\n";
            cout << "2. Manage All Transactions\n";
            cout << "3. View All Transactions\n";
            cout << "4. Generate Financial Reports\n";
            cout << "5. Generate System Wide Reports\n";
            cout << "6. Sign Out\n";
            cout << "7. Exit Program\n";
            int choice = getValidatedChoice(1, 7);
            if (choice == -1) {
                system("cls");
                continue;
            }

            switch (choice) {
                case 1:
                    manageUser();
                    break;
                case 2:
                    manageAllTransactions();
                    break;
                case 3:
                    viewAllTransactions();
                    break;
                case 4:
                    generateFinancialReports();
                    break;
                case 5:
                    generateSystemWideReports();
                    break;
                case 6:
                    loggedInUser.clear();
                    cout << "Signed out successfully.\n";
                    sysclear();
                    return;
                case 7:
                    cout << "Exiting program...\n";
                    exit(0);
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        }
    }

    // User account management
    void manageUser() {
        system("cls");
        int inputUserId;
        cout << "Input User ID to manage: ";
        cin >> inputUserId;
        cin.ignore();

        auto it = find_if(accounts.begin(), accounts.end(), 
            [inputUserId](const Account& account) {
                return account.userID == inputUserId;
            });

        if (it != accounts.end()) {
            int choice;
            string newUsername, newPassword;
            bool looper = true;
            cout << "\nAccount Found!\n";
            while(looper){
            cout << "User ID: " << it->userID << "\n";
            cout << "Username: " << it->username << "\n\n";
            
                cout << "1. Change Username\n";
                cout << "2. Change Password\n";
                cout << "3. Back\n";
                int choice = getValidatedChoice(1, 3);
                if (choice == -1) {
                    system("cls");
                    continue;
                }

                if (choice == 1) {
                    cout << "Enter new Username: ";
                    getline(cin, newUsername);
                    if (newUsername.length() < 4) {
                      cout << "Username must be at least 4 characters long.\n";
                      sysclear();
                      continue;
                   }
                    if (find_if(accounts.begin(), accounts.end(), 
                        [&newUsername](const Account& account) {
                            return account.username == newUsername;
                        }) == accounts.end()) {
                        it->username = newUsername;
                        overwriteAccountsFile();
                        cout << "Username updated successfully!\n";
                    } else {
                        cout << "New username already exists.\n";
                    }
                } else if (choice == 2) {
                    cout << "Enter new Password: ";
                    getline(cin, newPassword);
                    if (newPassword.length() < 6) {
                    cout << "Password must be at least 6 characters long.\n";
                    sysclear();
                    continue;
                }
                    it->password = newPassword;
                    overwriteAccountsFile();
                    cout << "Password updated successfully!\n";
                } else if(choice == 3){
                    system("cls");
                    return;
                }
                else {
                    cout << "Invalid choice.\n";
                }
                break;
            }
        } else {
            cout << "Account not found!\n";
        }
        sysclear();
    }

    // Transaction management for all the users
    void manageAllTransactions() {
        system("cls");
        string input;
        cout << "Input User ID: ";
        getline(cin, input);

        auto it = accounts.end();
        if (isdigit(input[0])) {
            int inputUserId = stoi(input);
            it = find_if(accounts.begin(), accounts.end(),
                [inputUserId](const Account& account) {
                    return account.userID == inputUserId;
                });
        }

        if (it != accounts.end()) {
            vector<Transaction> transactions;
                
            cout << "\nUser found:\n";
            cout << "User ID: " << it->userID << "\n";
            cout << "Username: " << it->username << "\n\n";
                
            cout << "Current transactions:\n";
            TransactionHelper::loadTransactionsFromFile(it->userID, transactions);
            TransactionHelper::displayTransactions(transactions);

            TransactionStrategy* strategy = nullptr;
            int choice;
            bool looper = true;
            while(looper){
	            cout << "\n1. Add Transaction\n2. Update Transaction\n3. Delete Transaction\n4. Back\n";
	            int choice = getValidatedChoice(1, 4);
	            if (choice == -1) {
		            system("cls");
		            continue;
		        }
                
                switch(choice) {
                    case 1: strategy = new AddTransaction(); break;
                    case 2: strategy = new UpdateTransaction(); break;
                    case 3: strategy = new DeleteTransaction(); break;
                    case 4: return;
                    default: cout << "Invalid choice\n"; return;
                }
                
                if (strategy) {
                    strategy->execute(it->userID, transactions);
                    delete strategy;
                }
                break;
	        }
        } else {
            cout << "Account not found!\n";
            sysclear();
    }       
 }

    // Viewing all existing transaction of specific user
    void viewAllTransactions() {
        system("cls");
        for (const auto& account : accounts) {
            cout << "\nTransactions for user: " << account.username << " (ID: " << account.userID << ")\n";
            string filename =  to_string(account.userID) + "_transactions.txt";
            ifstream file(filename);
                    
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    cout << line << endl;
                }
                file.close();
            } else {
                cout << "No transactions found.\n";
            }
        }
        sysclear();
    }

    // Generate financial reports for specific users
    void generateFinancialReports() {
        system("cls");
        int userID;
        cout << "Enter User ID for financial report: ";
        cin >> userID;
        cin.ignore();

        auto it = find_if(accounts.begin(), accounts.end(),
            [userID](const Account& account) {
                return account.userID == userID;
            });

        if (it != accounts.end()) {
            vector<Transaction> transactions;
            TransactionHelper::loadTransactionsFromFile(userID, transactions);

            if (transactions.empty()) {
                cout << "No transactions found to generate a report.\n";
                return;
            }

            map<string, double> categoryTotals;
            double totalExpenses = 0.0;

            string budgetFilename = to_string(userID) + "_budget.txt";
            string initialBudgetFilename = to_string(userID) + "_initial_budget.txt";
                
            double currentBudget = 0.0;
            double initialBudget = 0.0;

            // Read initial budget
            ifstream initialBudgetFile(initialBudgetFilename);
            if (initialBudgetFile.is_open()) {
                initialBudgetFile >> initialBudget;
                initialBudgetFile.close();
            }

            // Read current budget
            ifstream budgetFile(budgetFilename);
            if (budgetFile.is_open()) {
                budgetFile >> currentBudget;
                budgetFile.close();
            }

            for (const auto& t : transactions) {
                double transactionTotal = t.qty * t.price;
                categoryTotals[t.category] += transactionTotal;
                totalExpenses += transactionTotal;
            }

            cout << "\n=== Financial Report for User ID: " << userID << " (" << it->username << ") ===\n";
            cout << "Initial Budget: $" << fixed << setprecision(2) << initialBudget << "\n";
            cout << "Total Expenses: $" << fixed << setprecision(2) << totalExpenses << "\n";
            cout << "Remaining Budget: $" << fixed << setprecision(2) << currentBudget << "\n\n";
                
            cout << "Expenses by Category:\n";
            cout << string(60, '-') << "\n";
            cout << setw(20) << left << "Category" 
                << setw(15) << right << "Amount" 
                << setw(15) << right << "Percentage" << "\n";
            cout << string(60, '-') << "\n";
            
            for (const auto& category : categoryTotals) {
                double percentage = (category.second/totalExpenses) * 100;
                cout << setw(20) << left << category.first 
                    << setw(15) << right << "$" + to_string(category.second).substr(0, to_string(category.second).find(".") + 3)
                    << setw(14) << right << fixed << setprecision(1) << percentage << "%\n";
            }
            cout << string(60, '-') << "\n";
        } else {
            cout << "Account not found.\n";
        }
        sysclear();
    }

    // System analytics
    void generateSystemWideReports() {
        system("cls");
        // System Statistics
        int totalUsers = accounts.size();
        int totalTransactions = 0;
        double totalSystemExpenses = 0.0;
        map<string, double> systemCategoryExpenses;
        map<int, double> userExpenses;
        map<string, int> categoryTransactionCount;
        double highestTransaction = 0.0;
        double lowestTransaction = numeric_limits<double>::max();
        bool hasTransactions = false;
        
        cout << "\n=== System-Wide Analytics Report ===\n";
        cout << "Total Registered Users: " << totalUsers << "\n\n";

        // Process all transactions across users
        for (const auto& account : accounts) {
            vector<Transaction> userTransactions;
            TransactionHelper::loadTransactionsFromFile(account.userID, userTransactions);
            
            double userTotalExpense = 0.0;
            
            for (const auto& trans : userTransactions) {
                if (!hasTransactions) {
                hasTransactions = true;
                lowestTransaction = trans.qty * trans.price; // Initialize with first transaction
            }
                totalTransactions++;
                double amount = trans.qty * trans.price;
                
                // Update system totals
                totalSystemExpenses += amount;
                systemCategoryExpenses[trans.category] += amount;
                userExpenses[account.userID] += amount;
                categoryTransactionCount[trans.category]++;
                
                // Track highest and lowest transactions
                highestTransaction = max(highestTransaction, amount);
                lowestTransaction = min(lowestTransaction, amount);
                
                userTotalExpense += amount;
            }
        }

        if (!hasTransactions) {
        cout << "No transactions recorded in the system.\n";
        sysclear();
        return;
    }
        // Calculate averages and percentages
        double avgTransactionAmount = totalTransactions > 0 ? totalSystemExpenses / totalTransactions : 0;
        double avgUserExpense = totalUsers > 0 ? totalSystemExpenses / totalUsers : 0;

        // Display System Overview
        cout << "=== System Overview ===\n";
        cout << fixed << setprecision(2);
        cout << "Total Transactions: " << totalTransactions << "\n";
        cout << "Total System Expenses: $" << totalSystemExpenses << "\n";
        cout << "Average Transaction Amount: $" << avgTransactionAmount << "\n";
        cout << "Average User Expense: $" << avgUserExpense << "\n";
        cout << "Highest Transaction: $" << highestTransaction << "\n";
        cout << "Lowest Transaction: $" << lowestTransaction << "\n\n";

        // Display Category Analysis
        cout << "=== Category Analysis ===\n";
        cout << setw(20) << left << "Category" 
            << setw(15) << right << "Amount"
            << setw(15) << right << "% of Total"
            << setw(20) << right << "Transaction Count" << "\n";
        cout << string(70, '-') << "\n";
        
        for (const auto& category : systemCategoryExpenses) {
            double percentage = (category.second / totalSystemExpenses) * 100;
            cout << setw(20) << left << category.first 
                << setw(10) << right << "$" << category.second
                << setw(12) << right << percentage << "%"
                << setw(14) << right << categoryTransactionCount[category.first] << "\n";
        }

        // Display Top Spenders
        cout << "\n=== Top 5 Users by Expense ===\n";
        vector<pair<int, double>> userExpenseVec(userExpenses.begin(), userExpenses.end());
        sort(userExpenseVec.begin(), userExpenseVec.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        cout << setw(10) << left << "UserID"
            << setw(20) << left << "Username"
            << setw(15) << right << "Total Spent"
            << setw(15) << right << "% of Total" << "\n";
        cout << string(60, '-') << "\n";

        int count = 0;
        for (const auto& user : userExpenseVec) {
            if (count >= 5) break;
            
            auto it = find_if(accounts.begin(), accounts.end(),
                [&user](const Account& acc) { return acc.userID == user.first; });
                
            if (it != accounts.end()) {
                double percentage = (user.second / totalSystemExpenses) * 100;
                cout << setw(10) << left << user.first
                    << setw(20) << left << it->username
                    << setw(7) << right << "$" << user.second
                    << setw(13) << right << percentage << "%\n";
            }
            count++;
        }
        sysclear();
    }
};

Admin* Admin::instance = nullptr;

// Abstract base class for account operations
class AccountStrategy {
public:
    virtual void execute(ManageAccount& manage, vector<Transaction>& transactions) = 0;
    virtual ~AccountStrategy() {}
};

// For editing account
class EditAccount : public AccountStrategy {
public:
    void execute(ManageAccount& manage, vector<Transaction>& transactions) override {
        system("cls");

        if (manage.loggedInUser == "Admin") {
            // Admin can edit any user's account
            Admin* admin = static_cast<Admin*>(ManageAccount::getInstance());
            admin->manageUser();
        } else {
            // Regular user can only edit their own account
            string newUsername, newPassword;
            bool looper = true;
            while(true){
	            cout << "1. Change Username\n";
	            cout << "2. Change Password\n";
                cout << "3. Back\n";
	            int choice = getValidatedChoice(1, 3);
	            if (choice == -1) {
		            system("cls");
		            continue;
		        }
	            auto& accounts = manage.accounts;
	            auto it = find_if(accounts.begin(), accounts.end(),
	                [&manage](const Account& account) {
	                    return account.username == manage.loggedInUser;
	                });
	
	            if (it != accounts.end()) {
	                if (choice == 1) {
	                    cout << "Enter New Username (minimum 4 characters): ";
	                    getline(cin, newUsername);
                        if (newUsername.length() < 4) {
                        cout << "Username must be at least 4 characters long.\n";
                        sysclear();
                        continue;
                    }
	                    if (find_if(accounts.begin(), accounts.end(),
	                        [&newUsername](const Account& account) {
	                            return account.username == newUsername;
	                        }) == accounts.end()) {
	                        it->username = newUsername;
	                        manage.loggedInUser = newUsername;  // Update logged-in username
	                        manage.overwriteAccountsFile();
	                        cout << "Username updated successfully!\n";
	                    } else {
	                        cout << "New username already exists.\n";
	                    }
	                } else if (choice == 2) {
	                    cout << "Enter New Password (minimum 6 characters): ";
                         getline(cin, newPassword);
                        if (newPassword.length() < 6) {
                            cout << "Password must be at least 6 characters long.\n";
                            sysclear();
                            continue;
                        }
                            it->password = newPassword;
                            manage.overwriteAccountsFile();
                            cout << "Password updated successfully!\n";
	                } else if( choice == 3 ) {
                        system("cls");
                        return;
                    }
                    else {
	                    cout << "Invalid choice.\n";
	                }
	            }
                break;
			}
        }
        sysclear();
    }
};

// For transactions management and menu
class ManageTransactions : public AccountStrategy {
public:
    void execute(ManageAccount& manage, vector<Transaction>& transactions) override {
        system("cls");
        TransactionStrategy* strategy = nullptr;

        while (true) {
            cout << "\n--- Manage Transactions ---\n";
            cout << "1. Add Transaction\n";
            cout << "2. Update Transaction\n";
            cout << "3. Delete Transaction\n";
            cout << "4. View Transaction History\n";
            cout << "5. Back to Main Menu\n";
            int choice = getValidatedChoice(1, 5);
            if (choice == -1) {
		            system("cls");
		            continue;
		        }

            switch (choice) {
                case 1:
                    strategy = new AddTransaction();
                    break;
                case 2:
                    strategy = new UpdateTransaction();
                    break;
                case 3:
                    strategy = new DeleteTransaction();
                    break;
                case 4:
                    strategy = new ViewTransactionHistory();
                    break;
                case 5:
                    cout << "Returning to Main Menu...\n";
                    sysclear();
                    return;
                default:
                    cout << "Invalid choice. Try again.\n";
                    continue;
            }

            if (strategy) {
                strategy->execute(manage.getCurrentUserID(), transactions);
                delete strategy;
                strategy = nullptr;
            }
        }
    }
};

// For budget management
class SetMonthlyBudget : public AccountStrategy {
private:
    double validateBudgetInput(double currentBudget) {
        double newBudget;
        while (true) {
            try {
                cout << "\nEnter your new monthly budget (at least $100): $";
                cin >> newBudget;
                cin.ignore();

                if (cin.fail()) {
                    throw invalid_argument("Input must be a number.");
                }
                if (newBudget < 100) {
                    throw out_of_range("Budget must be at least $100.");
                }
                if (newBudget == currentBudget){
                    throw out_of_range("New budget cannot be the same as the current budget");
                }

                return newBudget;
            } catch (const invalid_argument& e) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input: " << e.what() << " Please try again.\n";
            } catch (const out_of_range& e) {
                cout << "Error: " << e.what() << " Please try again.\n";
            }
        }
    }

public:
    void execute(ManageAccount& manage, vector<Transaction>& transactions) override {
        system("cls");
        int userID = manage.getCurrentUserID();
        string budgetFilename = to_string(userID) + "_budget.txt";
        string initialBudgetFilename = to_string(userID) + "_initial_budget.txt";
        
        double currentBudget = 0.0;
        double initialBudget = 0.0;

        // Check if initial budget exists
        ifstream initialBudgetFile(initialBudgetFilename);
        if (initialBudgetFile.is_open()) {
            initialBudgetFile >> initialBudget;
            initialBudgetFile.close();
        }

        ifstream budgetFile(budgetFilename);
        if (budgetFile.is_open()) {
            budgetFile >> currentBudget;
            budgetFile.close();
        }

        if (currentBudget > 0) {
            cout << "Current monthly budget: $" << fixed << setprecision(2) << currentBudget << "\n";
            cout << "Do you want to replace it? (Y/N): ";
            char choice;
            cin >> choice;
            cin.ignore();

            if (tolower(choice) != 'y') {
                cout << "Budget remains unchanged.\n";
                sysclear();
                return;
            }
        }

        double newBudget = validateBudgetInput(currentBudget);

        // Save both current and initial budget
        ofstream budgetOut(budgetFilename);
        ofstream initialBudgetOut(initialBudgetFilename);

        if (budgetOut.is_open() && initialBudgetOut.is_open()) {
            budgetOut << fixed << setprecision(2) << newBudget;
            initialBudgetOut << fixed << setprecision(2) << newBudget;

            cout << "\nMonthly budget updated to: $" << newBudget << endl;
        } else {
            cout << "Error: Unable to save the budget.\n";
        }
        sysclear();
    }
};

// For checking current budget
class CheckCurrentBudget : public AccountStrategy {
public:
    void execute(ManageAccount& manage, vector<Transaction>& transactions) override {
        system("cls");
        int userID = manage.getCurrentUserID();
        string filename =  to_string(userID) + "_budget.txt";
        ifstream budgetFile(filename);
        double currentBudget = 0.0;

        if (budgetFile.is_open()) {
            budgetFile >> currentBudget;
            budgetFile.close();

            if (currentBudget > 0) {
                cout << "Your current budget is: $" << fixed << setprecision(2) << currentBudget << endl;
            } else {
                cout << "You have not set a budget or your budget is zero.\n";
            }
        } else {
            cout << "No budget found. Please set your budget first.\n";
        }
        sysclear();
    }
};

// For generating financial reports
class GenerateFinancialReports : public AccountStrategy {
private:
    double getRemainingBudget(int userID) {
        string filename = to_string(userID) + "_budget.txt";
        ifstream file(filename);
        double budget = 0.0;
        
        if (file.is_open()) {
            file >> budget;
            file.close();
        }
        return budget;
    }

public:
    void execute(ManageAccount& manage, vector<Transaction>& transactions) override {
        TransactionHelper::loadTransactionsFromFile(manage.getCurrentUserID(), transactions);
        system("cls");

        if (transactions.empty()) {
            cout << "No transactions found to generate a report.\n";
            sysclear();
            return;
        }

        map<string, double> categoryTotals;
        double totalExpenses = 0.0;
        
        int userID = manage.getCurrentUserID();
        string budgetFilename = to_string(userID) + "_budget.txt";
        string initialBudgetFilename = to_string(userID) + "_initial_budget.txt";
        
        double currentBudget = 0.0;
        double initialBudget = 0.0;

        // Read initial budget
        ifstream initialBudgetFile(initialBudgetFilename);
        if (initialBudgetFile.is_open()) {
            initialBudgetFile >> initialBudget;
            initialBudgetFile.close();
        }

        // Read current budget
        ifstream budgetFile(budgetFilename);
        if (budgetFile.is_open()) {
            budgetFile >> currentBudget;
            budgetFile.close();
        }

        for (const auto& t : transactions) {
            double transactionTotal = t.qty * t.price;
            categoryTotals[t.category] += transactionTotal;
            totalExpenses += transactionTotal;
        }

        cout << "\n=== Financial Report ===\n";
        cout << "Initial Budget: $" << fixed << setprecision(2) << initialBudget << "\n";
        cout << "Total Expenses: $" << fixed << setprecision(2) << totalExpenses << "\n";
        cout << "Remaining Budget: $" << fixed << setprecision(2) << currentBudget << "\n\n";
        
        cout << "Expenses by Category:\n";
        cout << string(60, '-') << "\n";
        cout << setw(20) << left << "Category" 
             << setw(15) << right << "Amount" 
             << setw(15) << right << "Percentage" << "\n";
        cout << string(60, '-') << "\n";
        
        for (const auto& category : categoryTotals) {
            double percentage = (category.second/totalExpenses) * 100;
            cout << setw(20) << left << category.first 
                 << setw(15) << right << "$" + to_string(category.second).substr(0, to_string(category.second).find(".") + 3)
                 << setw(14) << right << fixed << setprecision(1) << percentage << "%\n";
        }
        cout << string(60, '-') << "\n";

        sysclear();
    }
};


int main() {
    ManageAccount* manage = ManageAccount::getInstance();
    vector<Transaction> transactions;
    AccountStrategy* strategy = nullptr;

    start:
    bool looper = true;
    while (looper) {
        cout << "\n--- Expense Tracker Main Menu ---\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        int choice = getValidatedChoice(1, 3);
        if (choice == -1) {
            continue;
        }
        

        switch (choice) {
            case 1:
                manage->createAccount();
                sysclear();
                break;

            case 2:
                manage->logIn();
				sysclear();
				
                if (manage->loggedInUser == "Admin") {
                    Admin* admin = Admin::getInstance();
                    admin->manageAdminMenu(); 
                } 
               else if (!manage->loggedInUser.empty()) {
                    while (true) {
                        // Load transactions for current user
                        transactions.clear();
                        string filename =  to_string(manage->getCurrentUserID()) + "_transactions.txt";
                        ifstream file(filename);
                        
                        // Get current budget - Move this inside the loop to refresh after updates
                        double currentBudget = 0.0;
                        string budgetFile =  to_string(manage->getCurrentUserID()) + "_budget.txt";
                        ifstream bFile(budgetFile);
                        if (bFile.is_open()) {
                            bFile >> currentBudget;
                            bFile.close();
                        }

                        if (file.is_open()) {
                            string line;
                            getline(file, line); // Skip header
                            getline(file, line); // Skip separator
                            while (getline(file, line)) {
                                Transaction t;
                                stringstream ss(line);
                                ss >> t.transactionID >> t.category >> t.itemname >> t.qty >> t.amount;
                                transactions.push_back(t);
                            }
                            file.close();
                        }
                        
                        bool looper2 = true;
                        while(looper2){
	                        cout << "\n--- User Menu ---\n";
	                        cout << "1. Manage Transactions\n";
	                        cout << "2. Set Monthly Budget\n";
	                        cout << "3. Check Current Budget\n";
	                        cout << "4. Generate Financial Reports\n";
	                        cout << "5. Account Management\n";
	                        cout << "6. Log Out\n";
	                        cout << "7. Exit\n";
	                        int choice = getValidatedChoice(1, 7);
							if (choice == -1) {
					            continue;
					        }
					        
	                        switch (choice) {
	                            case 1: {
	                                if (currentBudget <= 0) {
	                                    cout << "You must set a budget before adding transactions, or your budget is depleted.\n";
	                                    sysclear();
	                                    break;
	                                }
	                                strategy = new ManageTransactions();
	                                break;
	                            }
	                            case 2: strategy = new SetMonthlyBudget(); break;
	                            case 3: strategy = new CheckCurrentBudget(); break;
	                            case 4: strategy = new GenerateFinancialReports(); break;
	                            case 5: strategy = new EditAccount(); break;
	                            case 6: {
	                                cout << "Logging out...\n";
	                                manage->loggedInUser.clear();
	                                sysclear();
	                                goto start;
	                            }
	                            case 7: {
	                                cout << "Exiting... Goodbye!\n";
	                                return 0;
	                            }
	                            default: {
	                                cout << "Invalid choice. Please try again.\n";
	                                continue;
	                            }
	                        }
	                        break;
						}

                        if (strategy) {
                            strategy->execute(*manage, transactions);
                            delete strategy;
                            strategy = nullptr;
                        }
                    }
                }
                break;

            case 3:
                cout << "\nExiting... Goodbye!\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}


