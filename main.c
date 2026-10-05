#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <io.h>
#include <ctype.h>
#include "accommodation.h"


#define MAX_USERS 100
const char *jobDepartments[] = {"Cleaner", "Receptionist", "Security", "Manager"};
#define NUM_DEPARTMENTS (sizeof(jobDepartments) / sizeof(jobDepartments[0]))
#define MAX_CUSTOMERS 100
#define MAX_BOOKINGS 100
#define PASSWORD_MAX_LENGTH 50
#define ROOM_TYPE_MAX_LENGTH 20
#define ALL_CUSTOMERS_FILE "all_customers.txt"
#define FEEDBACK_FILE "feedback.txt"
#define RECORD_FILE "record.txt"
#define MAX_ROOMS 50
#define MAX_NAME_LEN 50
#define MAX_TYPE_LEN 20 
#define DEBUG_MODE 1 

// Structures
typedef struct {
    char id[20];         
    char name[100];       
    char contact[15];     
    char department[50];  
} Employee;

typedef struct {
    char username[50];
    char password[50];
    char role[20]; 
} User;

typedef struct {
    char name[100];
    char email[100];
    char phone[15];
    char id[50];
    char checkInDate[11];
    char checkOutDate[11];
    int roomNumber;
    float discount;
    char feedback[500];
} Customer;

typedef struct {
    char name[100];
    char date[11];
    char state[50];
} Holiday;

typedef struct {
    char name[100];
    char startDate[11];
    char endDate[11];
    char group[20];
} SchoolHoliday;


typedef struct {
    char customerName[100];
    char roomID[10];   
    char roomType[ROOM_TYPE_MAX_LENGTH]; 
    int days;
    float totalCost;
    char checkInDate[11];
    char checkOutDate[11];
} Booking;

int employeeCount = 0; 
Employee employees[MAX_USERS];

User users[MAX_USERS];
Customer customers[MAX_CUSTOMERS];
Room rooms[MAX_ROOMS];
Booking bookings[MAX_BOOKINGS];

int bookingCount = 0;
int userCount = 0;
int roomCount = 0;
int numRooms = 0;
int max_type = 0;

void clearBuffer();
int validateSecurityPassword(const char *inputPassword, const char *correctPassword);
int isUsernameUnique(const char *username);
void saveUserRecord();
void loadUserRecord();
void registerUser();
int loginUser(User *loggedInUser);
void mainMenu(User loggedInUser);
void adminMenu();
void employeeMenu();
void customerMenu(User loggedInUser);
void ViewCustomersInformation();
void registerJob();
void collectFeedback();
void ViewlistofAccommodation();
void completeCustomerInfo();
void viewCurrentCustomers();
void customerCheckOut();
void addHeaderIfNewFile(const char *filename);
int isDuplicateCustomer(const char *customerID);
int isCustomerRegisteredByEmployee(const char *username);
int isHoliday(const char *date);
float calculateDiscount(const char *checkInDate, const char *checkOutDate);
void ViewlistofCustomer();
void collectFeedback();
void viewFeedback();
void bookingMenu();
void saveBookings();
void loadBookings();
void startBooking();
void viewBookings();
void bill_info();


Holiday publicHolidays[] = {
    {"New Year's Day", "2025-01-01", "Nationwide"},
    {"Chinese New Year", "2025-01-29", "Nationwide"},
    {"Chinese New Year (Second Day)", "2025-01-30", "Nationwide"},
    {"Hari Raya Aidilfitri", "2025-03-31", "Nationwide"},
    {"Hari Raya Aidilfitri (Second Day)", "2025-04-01", "Nationwide"},
    {"Labour Day", "2025-05-01", "Nationwide"},
    {"Wesak Day", "2025-05-12", "Nationwide"},
    {"Agong's Birthday", "2025-06-03", "Nationwide"},
    {"Hari Raya Haji", "2025-06-07", "Nationwide"},
    {"Merdeka Day", "2025-08-31", "Nationwide"},
    {"Malaysia Day", "2025-09-16", "Nationwide"},
    {"Deepavali", "2025-10-20", "All except Sarawak"},
    {"Prophet Muhammad's Birthday", "2025-09-05", "Nationwide"},
    {"Christmas Day", "2025-12-25", "Nationwide"}
};


SchoolHoliday schoolHolidays[] = {
    {"Chinese New Year Holidays", "2025-01-18", "2025-02-16", "Group A & B"},
    {"Hari Raya Aidilfitri Holidays", "2025-03-31", "2025-04-04", "Group A & B"},
    {"Term 1 Holidays", "2025-03-13", "2025-03-21", "Group A"},
    {"Term 1 Holidays", "2025-03-14", "2025-03-22", "Group B"},
    {"Mid-Year Holidays", "2025-05-22", "2025-06-06", "Group A"},
    {"Mid-Year Holidays", "2025-05-23", "2025-06-07", "Group B"},
    {"Term 2 Holidays", "2025-07-24", "2025-08-01", "Group A"},
    {"Term 2 Holidays", "2025-07-25", "2025-08-02", "Group B"},
    {"Deepavali Holidays", "2025-10-19", "2025-10-21", "Group A"},
    {"Deepavali Holidays", "2025-10-20", "2025-10-22", "Group B"},
    {"End of Year Holidays", "2025-12-19", "2026-01-10", "Group A"},
    {"End of Year Holidays", "2025-12-20", "2026-01-11", "Group B"}
};

void clearBuffer() {
    while (getchar() != '\n');
}

int validateSecurityPassword(const char *inputPassword, const char *correctPassword) {
    int i = 0;
    while (inputPassword[i] != '\0' && correctPassword[i] != '\0') {
        if (inputPassword[i] != correctPassword[i]) {
            return 0; // Mismatch found
        }
        i++;
    }
    return inputPassword[i] == '\0' && correctPassword[i] == '\0'; // Both should end at the same time
}

int isUsernameUnique(const char *username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return 0; 
        }
    }
    return 1; 
}

void saveUserRecord() {
    FILE *file = fopen(RECORD_FILE, "w");
    if (file == NULL) {
        printf("Error: Unable to save user records.\n");
        return;
    }

    fprintf(file, "%d\n", userCount); 
    for (int i = 0; i < userCount; i++) {
        fprintf(file, "%s %s %s\n", users[i].username, users[i].password, users[i].role);
    }

    fclose(file);
}

void loadUserRecord() {
    FILE *file = fopen(RECORD_FILE, "r");
    if (file == NULL) {
        return; 
    }

    fscanf(file, "%d\n", &userCount); 
    for (int i = 0; i < userCount; i++) {
        fscanf(file, "%s %s %s\n", users[i].username, users[i].password, users[i].role);
    }

    fclose(file);
}

// Register a new user
void registerUser() {
    int roleChoice;
    system("cls");
    printf("\n=== Register Menu ===\n");
    printf("1. Customer\n");
    printf("2. Administrator\n");
    printf("3. Employee\n");
    printf("Enter your choice: ");
    scanf("%d", &roleChoice);
    clearBuffer();

    char securityPassword[50];
    switch (roleChoice) {
        case 1:
            strcpy(users[userCount].role, "Customer");
            break;
        case 2:
            printf("Enter Administrator Security Password: ");
            scanf("%s", securityPassword);
            clearBuffer();
            if (!validateSecurityPassword(securityPassword, "AIT101")) {
                printf("Invalid Administrator Security Password. Registration failed.\n");
                return;
            }
            strcpy(users[userCount].role, "Administrator");
            break;
        case 3:
            printf("Enter Employee Security Password: ");
            scanf("%s", securityPassword);
            clearBuffer();
            if (!validateSecurityPassword(securityPassword, "CST101")) {
                printf("Invalid Employee Security Password. Registration failed.\n");
                return;
            }
            strcpy(users[userCount].role, "Employee");
            break;
        default:
            printf("Invalid choice. Registration cancelled.\n");
            return;
    }

    char username[50];
    printf("Enter username: ");
    scanf("%s", username);
    clearBuffer();

    if (!isUsernameUnique(username)) {
        printf("Username already exists. Please try a different username.\n");
        return;
    }

    strcpy(users[userCount].username, username);

    printf("Enter password: ");
    scanf("%s", users[userCount].password);
    clearBuffer();

    userCount++;
    saveUserRecord();

    printf("User registered successfully as %s!\n", users[userCount - 1].role);
}

// Login a user
int loginUser(User *loggedInUser) {
    char username[50], password[PASSWORD_MAX_LENGTH];
    system("cls");
    printf("Enter username: ");
    scanf("%s", username);
    clearBuffer();

    printf("Enter password: ");
    scanf("%s", password);
    clearBuffer();

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            *loggedInUser = users[i];
            system("cls");
            printf("Login successful! Welcome, %s.\n", loggedInUser->username);
            return 1;
        }
    }
    system("cls");
    printf("Invalid username or password.\n");
    return 0;
}

int isCustomerRegisteredByEmployee(const char *username) {
    FILE *file = fopen(ALL_CUSTOMERS_FILE, "r");
    if (!file) {
        return 0;
    }

    char line[500];
    while (fgets(line, sizeof(line), file)) {
        Customer customer;
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d", 
               customer.name, customer.email, customer.phone, customer.id, 
               customer.checkInDate, customer.checkOutDate, &customer.roomNumber);
        
        if (strcmp(customer.name, username) == 0) {
            fclose(file);
            return 1; 
        }
    }

    fclose(file);
    return 0; 
}

// Main Menu
void mainMenu(User loggedInUser) {
    if (strcmp(loggedInUser.role, "Administrator") == 0) {
        adminMenu();
    } else if (strcmp(loggedInUser.role, "Employee") == 0) {
        employeeMenu();
    } else if (strcmp(loggedInUser.role, "Customer") == 0) {
        customerMenu(loggedInUser);
    } else {
        printf("Unknown role: %s\n", loggedInUser.role);
    }
}

// Admin Menu
void adminMenu() {
    int choice;
    do {
        printf("\n=== Admin Menu ===\n");
        printf("1. Record Accomodation Information\n");
        printf("2. View Customers Information\n");
        printf("3. View List of Customers\n");
        printf("4. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1:
                AccommodationRecord();
                break;
            case 2:
                ViewCustomersInformation();
                break;
            case 3:
                ViewlistofCustomer();
                break;
            case 4:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
        char key;
        printf("\n Enter to continue.");
        scanf("%c", &key);
        system("cls");
    } while (1);
}

// Employee Menu
void employeeMenu() {
    int choice;
    do {
        printf("\n=== Employee Menu ===\n");
        printf("1. Registered based on Job department\n");
        printf("2. View List of Customer\n");
        printf("3. View List of Accommodation\n");
        printf("4. Customer Checkout\n");
        printf("5. View feedback\n");
        printf("6. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1:
                registerJob();
                break;
            case 2:
                ViewlistofCustomer();
                break;
            case 3:
                ViewlistofAccommodation();
                break;
            case 4:
                customerCheckOut();
                break;
            case 5:
                viewFeedback();
                break;
            case 6:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
        char key;
        printf("\n Enter to continue.");
        scanf("%c", &key);
        system("cls");
    } while (1);
}

// Customer Menu
void customerMenu(User loggedInUser) {
    int choice;
    do {
        printf("\n=== Customer Menu ===\n");
        printf("1. Complete Customer Information\n");
        printf("2. View Accommodation\n");
        printf("3. Make Booking\n");
        printf("4. Give Feedback\n");
        printf("5. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1:
                completeCustomerInfo();
                break;
            case 2:
                viewAccommodation();
                break;
            case 3:
                bookingMenu();
                break;
            case 4:
                collectFeedback();
                break;
            case 5:
                printf("Logging out...\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
        char key;
        printf("\n Enter to continue.");
        scanf("%c", &key);
        system("cls");
    } while (1);
}

// Admin part
void ViewCustomersInformation() {
    FILE *file = fopen(ALL_CUSTOMERS_FILE, "r");
    if (!file) {
        printf("No customer data available to view.\n");
        return;
    }

    char searchKey[100];
    int choice;

    printf("\n=== Search Customer Information ===\n");
    printf("1. Search by Name\n");
    printf("2. Search by ID\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    if (choice == 1) {
        printf("Enter Customer Name: ");
        fgets(searchKey, sizeof(searchKey), stdin);
        searchKey[strcspn(searchKey, "\n")] = '\0';
    } else if (choice == 2) {
        printf("Enter Customer ID: ");
        fgets(searchKey, sizeof(searchKey), stdin);
        searchKey[strcspn(searchKey, "\n")] = '\0';
    } else {
        printf("Invalid choice. Returning to menu.\n");
        fclose(file);
        return;
    }

    char line[500];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        Customer customer;
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d",
               customer.name, customer.email, customer.phone, customer.id,
               customer.checkInDate, customer.checkOutDate, &customer.roomNumber);

        if ((choice == 1 && strcasecmp(customer.name, searchKey) == 0) ||
            (choice == 2 && strcasecmp(customer.id, searchKey) == 0)) {

            // Fetch the correct room number from bookings
            char correctRoomID[10] = "Not Found";
            for (int i = 0; i < bookingCount; i++) {
                if (strcasecmp(bookings[i].customerName, customer.name) == 0) {
                    strcpy(correctRoomID, bookings[i].roomID);
                    break;
                }
            }

            printf("\nCustomer Information:\n");
            printf("+----------------------+----------------------------------+\n");
            printf("| %-20s | %-32s |\n", "Field", "Details");
            printf("+----------------------+----------------------------------+\n");
            printf("| Name                 | %-32s |\n", customer.name);
            printf("| Email                | %-32s |\n", customer.email);
            printf("| Phone                | %-32s |\n", customer.phone);
            printf("| ID                   | %-32s |\n", customer.id);
            printf("| Check-In Date        | %-32s |\n", customer.checkInDate);
            printf("| Check-Out Date       | %-32s |\n", customer.checkOutDate);
            printf("| Room Number          | %-32s |\n", correctRoomID);
            printf("+----------------------+----------------------------------+\n");

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("\nNo customer found with the provided information.\n");
    }

    fclose(file);
}

// Employee part
// Updated registerJob function to include file saving
void registerJob() {
    FILE *file = fopen("employee_records.txt", "a"); 
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    if (employeeCount >= MAX_USERS) {
        printf("Employee limit reached. Cannot register more employees.\n");
        return;
    }

    printf("\nAvailable Job Departments:\n");
    for (int i = 0; i < NUM_DEPARTMENTS; i++) {
        printf("%d. %s\n", i + 1, jobDepartments[i]);
    }

    int departmentChoice;
    printf("Select Department (1-%d): ", NUM_DEPARTMENTS);
    scanf("%d", &departmentChoice);
    clearBuffer();

    if (departmentChoice < 1 || departmentChoice > NUM_DEPARTMENTS) {
        printf("Invalid department selection.\n");
        return;
    }

    Employee newEmployee;
    printf("Enter Employee Name: ");
    scanf("%[^\n]", newEmployee.name);
    clearBuffer();

    printf("Enter Employee ID: ");
    scanf("%s", newEmployee.id);
    clearBuffer();

    printf("Enter Contact Number: ");
    scanf("%s", newEmployee.contact);
    clearBuffer();

    strcpy(newEmployee.department, jobDepartments[departmentChoice - 1]);
    employees[employeeCount++] = newEmployee;

    fprintf(file, "%s,%s,%s,%s\n", newEmployee.name, newEmployee.id, newEmployee.contact, newEmployee.department);
    fclose(file);
    printf("Employee registered successfully as %s!\n", newEmployee.department);
}

void ViewlistofCustomer() {
    system("cls");
    loadBookings();

    FILE *file = fopen(ALL_CUSTOMERS_FILE, "r");
    if (!file) {
        printf("No customer data available to view.\n");
        return;
    }

    char line[500];
    printf("\nCustomer List\n");
    printf("+----------------------+---------------------------+-----------------+------------+------------+----------+------------------+-------------+\n");
    printf("| %-20s | %-25s | %-15s | %-10s | %-10s | %-8s | %-16s | %-11s |\n", 
           "Name", "Email", "Phone", "Check-In", "Check-Out", "Room No", "Customer ID", "Price (RM)");
    printf("+----------------------+---------------------------+-----------------+------------+------------+----------+------------------+-------------+\n");

    while (fgets(line, sizeof(line), file)) {
        char name[100] = {0};
        char email[100] = {0};
        char phone[15] = {0};
        char id[50] = {0};
        char checkIn[11] = {0};
        char checkOut[11] = {0};
        
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]",name, email, phone, id, checkIn, checkOut);

        char *outs = strchr(checkOut, '|');
        if (outs) *outs= '\0';

        for (int i = strlen(name) - 1; i >= 0 && isspace(name[i]); i--) name[i] = '\0';
        for (int i = strlen(email) - 1; i >= 0 && isspace(email[i]); i--) email[i] = '\0';
        for (int i = strlen(phone) - 1; i >= 0 && isspace(phone[i]); i--) phone[i] = '\0';
        for (int i = strlen(id) - 1; i >= 0 && isspace(id[i]); i--) id[i] = '\0';
        for (int i = strlen(checkIn) - 1; i >= 0 && isspace(checkIn[i]); i--) checkIn[i] = '\0';
        for (int i = strlen(checkOut) - 1; i >= 0 && isspace(checkOut[i]); i--) checkOut[i] = '\0';

        char roomNumber[10] = "N/A";
        float totalCost = 0.0;
        float discountedPrice = 0.0;
        int bookingFound = 0;

        for (int i = 0; i < bookingCount; i++) {
            if (strcasecmp(bookings[i].customerName, name) == 0) {
                strcpy(roomNumber, bookings[i].roomID); 
                totalCost = bookings[i].totalCost;
                float discountRate = calculateDiscount(checkIn, checkOut);
                discountedPrice = totalCost * (1 - discountRate);
                bookingFound = 1;
                break;
            }
        }

        if (strcmp(roomNumber, "N/A") != 0) {
            char truncName[21] = {0};
            char truncEmail[26] = {0};
            char truncPhone[16] = {0};
            char truncID[17] = {0};

            strncpy(truncName, name, 20);
            strncpy(truncEmail, email, 25);
            strncpy(truncPhone, phone, 15);
            strncpy(truncID, id, 16);

            printf("| %-20s | %-25s | %-15s | %-10s | %-10s | %-8s | %-16s | %11.2f |\n",
                   truncName, truncEmail, truncPhone, checkIn, checkOut, roomNumber, truncID, discountedPrice);
        }
    }
    printf("+----------------------+---------------------------+-----------------+------------+------------+----------+------------------+-------------+\n");
    fclose(file);
}


void ViewlistofAccommodation() 
{
    char type[N];
    while (true)
    {
        system("cls");
        viewAccommodation();
        printf("Viewing Room ID Available\n");
        printf("Select Room Type (input 'end' to exit): ");
        scanf("%[^\n]", &type);
        getchar();
        if (strcasecmp(type, "end")==0){break;}
        displayIDAvailable(type);
    }
}

void viewFeedback() {
    FILE *file = fopen(FEEDBACK_FILE, "r");
    char line[512];
    if (file == NULL) {
        printf("No feedback available.\n");
        return;
    }

    int a = 1;
    printf("\n+-----------------------------------------------------------+\n");
    printf("|                         Feedback                          |\n");
    printf("+-----------------------------------------------------------+\n");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        printf("| %d. %-54s |\n", a++, line);
    }
    printf("+-----------------------------------------------------------+\n");
    fclose(file);
}

void collectFeedback() {
    FILE *file = fopen(FEEDBACK_FILE, "a");
    char feedback[500];
    printf("Enter Feedback: ");
    fgets(feedback, sizeof(feedback), stdin);
    feedback[strcspn(feedback, "\n")] = '\0';
    fprintf(file, "%s\n", feedback);
    fclose(file);
    printf("Thank you for your feedback!\n");
}

// Customer part
void completeCustomerInfo() {
    Customer customer;
    struct tm checkIn = {0}, checkOut = {0};
    int validDate;
    char employeeID[50];

    do {
        printf("\nEnter Customer Name: ");
        fgets(customer.name, 100, stdin);
        customer.name[strcspn(customer.name, "\n")] = 0;

        if (strlen(customer.name) == 0) {
            printf("Name cannot be empty. Please enter a valid name.\n");
        }
    } while (strlen(customer.name) == 0);

    // Input and validate email
    do {
        printf("Enter Email: ");
        fgets(customer.email, 100, stdin);
        customer.email[strcspn(customer.email, "\n")] = 0;

        const char *at = strchr(customer.email, '@');
        const char *dot = strrchr(customer.email, '.');
        if (!(at && dot && at < dot)) {
            printf("Invalid email format. Please enter a valid email address.\n");
        }
    } while (!(strchr(customer.email, '@') && strrchr(customer.email, '.') &&
               strchr(customer.email, '@') < strrchr(customer.email, '.')));

    // Input and validate phone number
    do {
        printf("Enter Phone Number: ");
        fgets(customer.phone, 15, stdin);
        customer.phone[strcspn(customer.phone, "\n")] = 0;

        int validPhone = 1;
        for (int i = 0; customer.phone[i]; ++i) {
            if (!isdigit(customer.phone[i]) && customer.phone[i] != '-') {
                validPhone = 0;
                break;
            }
        }

        if (!(validPhone && strlen(customer.phone) >= 7 && strlen(customer.phone) <= 15)) {
            printf("Invalid phone number. Please enter a valid phone number.\n");
        }
    } while (!(strlen(customer.phone) >= 7 && strlen(customer.phone) <= 15 &&
               ({
                   int validPhone = 1;
                   for (int i = 0; customer.phone[i]; ++i) {
                       if (!isdigit(customer.phone[i]) && customer.phone[i] != '-') {
                           validPhone = 0;
                           break;
                       }
                   }
                   validPhone;
               })));

    // Input and validate ID/Passport number
    do {
        printf("Enter ID/Passport Number: ");
        fgets(customer.id, 50, stdin);
        customer.id[strcspn(customer.id, "\n")] = 0;

        if (strlen(customer.id) == 0) {
            printf("ID/Passport number cannot be empty. Please enter a valid ID.\n");
        } else if (isDuplicateCustomer(customer.id)) {
            printf("This customer has already been registered.\n");
            return;
        }
    } while (strlen(customer.id) == 0);

    // Input and validate check-in date
    do {
        printf("Enter Check-In Date (YYYY-MM-DD): ");
        fgets(customer.checkInDate, 11, stdin);
        customer.checkInDate[strcspn(customer.checkInDate, "\n")] = 0;
        getchar();

        validDate = sscanf(customer.checkInDate, "%d-%d-%d", &checkIn.tm_year, &checkIn.tm_mon, &checkIn.tm_mday);
        if (validDate != 3 || checkIn.tm_mon < 1 || checkIn.tm_mon > 12 || checkIn.tm_mday < 1 || checkIn.tm_mday > 31) {
            printf("Invalid Check-In Date. Please ensure the date is in YYYY-MM-DD format with valid values.\n");
            validDate = 0;
        }
    } while (!validDate);

    checkIn.tm_year -= 1900;
    checkIn.tm_mon -= 1;

    // Input and validate check-out date
    do {
        printf("Enter Check-Out Date (YYYY-MM-DD): ");
        fgets(customer.checkOutDate, 11, stdin);
        customer.checkOutDate[strcspn(customer.checkOutDate, "\n")] = 0;
        getchar();

        validDate = sscanf(customer.checkOutDate, "%d-%d-%d", &checkOut.tm_year, &checkOut.tm_mon, &checkOut.tm_mday);
        if (validDate != 3 || checkOut.tm_mon < 1 || checkOut.tm_mon > 12 || checkOut.tm_mday < 1 || checkOut.tm_mday > 31) {
            printf("Invalid Check-Out Date. Please ensure the date is in YYYY-MM-DD format with valid values.\n");
            validDate = 0;
        } else {
            checkOut.tm_year -= 1900;
            checkOut.tm_mon -= 1;

            time_t checkInTime = mktime(&checkIn);
            time_t checkOutTime = mktime(&checkOut);

            if (difftime(checkOutTime, checkInTime) < 0) {
                printf("Invalid Check-Out Date. It cannot be earlier than the Check-In Date.\n");
                validDate = 0;
            }
        }
    } while (!validDate);

    FILE *allFile = fopen(ALL_CUSTOMERS_FILE, "a");
    if (allFile == NULL) {
        printf("Error opening file for all customers.\n");
        return;
    }
    fprintf(allFile, "%s|%s|%s|%s|%s|%s\n",
            customer.name, customer.email, customer.phone, customer.id,
            customer.checkInDate, customer.checkOutDate);
    fclose(allFile);

    printf("Customer registration successful!\n");
}

int isDuplicateCustomer(const char *customerID) {
    FILE *file = fopen(ALL_CUSTOMERS_FILE, "r");
    if (!file) {
        return 0; 
    }

    char line[500];
    while (fgets(line, sizeof(line), file)) {
        Customer customer;
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%d", 
               customer.name, customer.email, customer.phone, customer.id, 
               customer.checkInDate, customer.checkOutDate, &customer.roomNumber);
        
        if (strcmp(customer.id, customerID) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0; 
}

int isHoliday(const char *date) {
    struct tm inputDate = {0};
    if (sscanf(date, "%4d-%2d-%2d", &inputDate.tm_year, &inputDate.tm_mon, &inputDate.tm_mday) != 3) {
        printf("\nInvalid date format: %s. Expected format is YYYY-MM-DD.\n", date);
        return 0; // Not a holiday
    }

    inputDate.tm_year -= 1900;  
    inputDate.tm_mon -= 1;

    time_t inputTime = mktime(&inputDate);
    if (inputTime == -1) {
        printf("\nInvalid date provided: %s. Unable to parse.\n", date);
        return 0; // Not a holiday
    }

    for (int i = 0; i < sizeof(publicHolidays) / sizeof(publicHolidays[0]); i++) {
        struct tm holidayDate = {0};
        sscanf(publicHolidays[i].date, "%4d-%2d-%2d", &holidayDate.tm_year, &holidayDate.tm_mon, &holidayDate.tm_mday);
        holidayDate.tm_year -= 1900;  
        holidayDate.tm_mon -= 1;

        time_t holidayTime = mktime(&holidayDate);
        if (holidayTime == inputTime) {
            return 1; 
        }
    }

    for (int i = 0; i < sizeof(schoolHolidays) / sizeof(schoolHolidays[0]); i++) {
        struct tm startDate = {0}, endDate = {0};
        sscanf(schoolHolidays[i].startDate, "%4d-%2d-%2d", &startDate.tm_year, &startDate.tm_mon, &startDate.tm_mday);
        sscanf(schoolHolidays[i].endDate, "%4d-%2d-%2d", &endDate.tm_year, &endDate.tm_mon, &endDate.tm_mday);
        startDate.tm_year -= 1900;  
        startDate.tm_mon -= 1;
        endDate.tm_year -= 1900;  
        endDate.tm_mon -= 1;

        time_t startTime = mktime(&startDate);
        time_t endTime = mktime(&endDate);

        if (inputTime >= startTime && inputTime <= endTime) {
            return 1;
        }
    }

    return 0; // Not a holiday
}



float calculateDiscount(const char *checkInDate, const char *checkOutDate) {
    return isHoliday(checkInDate) || isHoliday(checkOutDate) ? 0.15f : 0.0f;
}


void bookingMenu() {
    int amount_people;
    int a;

    do {
        printf("\n===================================================\n");
        printf("\tWelcome to the Hotel Booking System\n");
        printf("===================================================\n");

        printf("\nStart booking, view bookings, or back to main menu?\n");
        printf("1. Start a booking\n");
        printf("2. View booking details\n");
        printf("3. Back to main menu\n");
        printf("Pick your choice (1 - 3): ");

        if (scanf("%d", &a) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); 
            continue;
        }

        switch (a) {
            case 1:
                startBooking();
                break;

            case 2:
                bill_info();
                break;

            case 3:
                printf("Returning back to main menu...\n");
                return;

            default:
                printf("***********************************************************\n");
                printf("\tPlease select your choice only (1, 2, or 3) !!!\n");
                printf("***********************************************************\n");
                break;
        }
    } while (1);
}

void saveBookings() {
    FILE *file = fopen("bookings.txt", "w"); 
    if (!file) {
        printf("Error: Unable to open bookings file for saving.\n");
        return;
    }

    for (int i = 0; i < bookingCount; i++) {
        fprintf(file, "%s|%s|%s|%.2f|%s|%s\n",
                bookings[i].customerName,
                bookings[i].roomType,
                bookings[i].roomID,
                bookings[i].totalCost,
                bookings[i].checkInDate,
                bookings[i].checkOutDate);
    }

    fclose(file);
}

void loadBookings() {
    FILE *file = fopen("bookings.txt", "r");
    if (!file) {
        printf("Unable to open bookings.txt file.\n");
        bookingCount = 0;
        return;
    }

    char line[256];
    bookingCount = 0;
    
    while (fgets(line, sizeof(line), file) && bookingCount < MAX_BOOKINGS) {
        char *data;
        
        data = strtok(line, "|");
        if (data) strcpy(bookings[bookingCount].customerName, data);
        
        data = strtok(NULL, "|");
        if (data) strcpy(bookings[bookingCount].roomType, data);
        
        data = strtok(NULL, "|");
        if (data) strcpy(bookings[bookingCount].roomID, data);
        
        data = strtok(NULL, "|");
        if (data) bookings[bookingCount].totalCost = atof(data);
        
        data = strtok(NULL, "|");
        if (data) strcpy(bookings[bookingCount].checkInDate, data);
        
        data = strtok(NULL, "\n");
        if (data) strcpy(bookings[bookingCount].checkOutDate, data);
        bookingCount++;
    }
    fclose(file);
}


void startBooking() {
    system("cls");

    int accommodationCount = loadAccommodationData();
    int max_type = loadRoomType();

    if (accommodationCount == 0) {
        printf("No rooms available in this session\n");
        return;
    }

    printf("=================================================================\n");
    printf("| Room ID    | Room Type  | Availability |\n");
    printf("=================================================================\n");
    for (int i = 0; i < accommodationCount; i++) {
        printf("| %-10s | %-10s | %-12s |\n",
               accommodation[i].room_ID,
               accommodation[i].room_type,
               accommodation[i].availability == 0 ? "Available" : "Unavailable");
    }
    printf("=================================================================\n");

    Booking newBooking;
    printf("\nPlease enter your name: ");
    scanf(" %[^\n]", newBooking.customerName);

    printf("Enter the name of the room type : ");
    scanf(" %[^\n]", newBooking.roomType);

    printf("Enter the name of the room ID : ");
    scanf(" %[^\n]", newBooking.roomID);

    bool roomFound = false;
    int roomIndex = -1;
    float roomPrice = 0;

    for (int i = 0; i < accommodationCount; i++) {
        if (strcasecmp(accommodation[i].room_ID, newBooking.roomID) == 0 &&
            strcasecmp(accommodation[i].room_type, newBooking.roomType) == 0) {

            if (accommodation[i].availability == 0) {
                roomFound = true;
                roomIndex = i;

                for (int j = 0; j < max_type; j++) {
                    if (strcasecmp(room[j].room_type, newBooking.roomType) == 0) {
                        roomPrice = room[j].price;
                        room[j].num--;
                        saveRoomType(max_type);
                        break;
                    }
                }
                break;
            } else {
                printf("Room %s is already booked.\n", accommodation[i].room_ID);
                return;
            }
        }
    }

    if (!roomFound) {
        printf("Room ID or Room Type not found or unavailable. Please try again.\n");
        return;
    }

    struct tm checkIn = {0}, checkOut = {0};
    char dateInput[11];


    while (1) {
        printf("Enter check-in date (YYYY-MM-DD): ");
        scanf(" %[^\n]", dateInput);

        if (sscanf(dateInput, "%4d-%2d-%2d", 
                   &checkIn.tm_year, &checkIn.tm_mon, &checkIn.tm_mday) == 3) {
            checkIn.tm_year -= 1900;  
            checkIn.tm_mon -= 1;     

            if (checkIn.tm_year >= 0 && checkIn.tm_mon >= 0 && checkIn.tm_mon < 12 && 
                checkIn.tm_mday > 0 && checkIn.tm_mday <= 31) {
                time_t checkInTime = mktime(&checkIn);
                if (checkInTime != -1) {
                    strcpy(newBooking.checkInDate, dateInput);
                    break;
                }
            }
        }
        printf("Invalid date. Please try again.\n");
    }

    while (1) {
        printf("Enter check-out date (YYYY-MM-DD): ");
        scanf(" %[^\n]", dateInput);

        if (sscanf(dateInput, "%4d-%2d-%2d", 
                   &checkOut.tm_year, &checkOut.tm_mon, &checkOut.tm_mday) == 3) {
            checkOut.tm_year -= 1900; 
            checkOut.tm_mon -= 1;    

            if (checkOut.tm_year >= 0 && checkOut.tm_mon >= 0 && checkOut.tm_mon < 12 && 
                checkOut.tm_mday > 0 && checkOut.tm_mday <= 31) {
                time_t checkOutTime = mktime(&checkOut);
                if (checkOutTime != -1 && difftime(checkOutTime, mktime(&checkIn)) > 0) {
                    strcpy(newBooking.checkOutDate, dateInput);
                    break;
                } else {
                    printf("Check-out date must be after check-in date. Please try again.\n");
                }
            }
        }
        printf("Invalid date. Please try again.\n");
    }

    printf("\nDates validated successfully!\n");
    printf("Check-In Date: %s\nCheck-Out Date: %s\n", newBooking.checkInDate, newBooking.checkOutDate);

    time_t checkInTime = mktime(&checkIn);
    time_t checkOutTime = mktime(&checkOut);

    if(difftime(checkOutTime, checkInTime) <= 0) {
        printf("Invalid check-in/check-out dates.\n");
        return;
    }

    newBooking.days = (int)(difftime(checkOutTime, checkInTime) / (60 * 60 * 24));
    newBooking.totalCost = roomPrice * newBooking.days;

    bookings[bookingCount++] = newBooking;
    saveBookings();

    accommodation[roomIndex].availability = 1;
    saveAccommodationData(accommodationCount);

    printf("\nBooking successful!\n");
    printf("Name: %s\nRoom Type: %s\nRoom ID: %s\nCheck-In Date: %s\nCheck-Out Date: %s\nDays: %d\nTotal Cost: RM%.2f\n",
           newBooking.customerName, newBooking.roomType, newBooking.roomID, newBooking.checkInDate, newBooking.checkOutDate, newBooking.days, newBooking.totalCost);
    printf("\nPress Enter to return to the booking menu...");
    getchar();
    getchar();
}


void bill_info() {
    system("cls");
    loadBookings();
    if (bookingCount == 0) {
        printf("No bookings available.\n");
        return;
    }

    for (int i = 0; i < bookingCount; i++) {
        float discountRate = calculateDiscount(bookings[i].checkInDate, bookings[i].checkOutDate);
        float discountedPrice = bookings[i].totalCost * (1 - discountRate);

        printf("\n==============================================================\n");
        printf("                         BILL                                \n");
        printf("==============================================================\n");
        printf("Customer Name      : %s\n", bookings[i].customerName);
        printf("Room Type          : %s\n", bookings[i].roomType);
        printf("Room ID            : %s\n", bookings[i].roomID);
        printf("Check-In Date      : %s\n", bookings[i].checkInDate);
        printf("Check-Out Date     : %s\n", bookings[i].checkOutDate);
        printf("Total Cost (RM)    : RM%.2f\n", bookings[i].totalCost);
        printf("Discount Applied   : %.0f%%\n", discountRate * 100);
        printf("Discounted Price   : RM%.2f\n", discountedPrice);
        printf("==============================================================\n");
    }

    printf("\nPress Enter to return to the booking menu...");
    getchar();
    getchar();
}

void customerCheckOut()
{
    system("cls");
    loadBookings();
    int num = loadAccommodationData();
    int max_type = loadRoomType();
    char room_ID[N];
    printf("Enter room ID that want to check out: ");
    scanf("%[^\n]", room_ID); getchar();
    for (int i = 0 ; i < num ; i++)
    {
        if (strcasecmp(accommodation[i].room_ID, room_ID) == 0)
        {
            if (!accommodation[i].availability) {printf("The Room is empty.\n");break;}
            accommodation[i].availability = false;

            for (int j = 0; j < max_type; j++)
            {
                if (strcasecmp(room[j].room_type, accommodation[i].room_type)==0)
                {
                    room[j].num++;
                    saveRoomType(max_type);
                    break;
                }
            }

            for (int j = 0; j < bookingCount; j++)
            {
                if (strcasecmp(bookings[j].roomID, room_ID) == 0)
                {
                    for (int x = j; x < userCount; x++)
                    {
                        if (x+1 == userCount){bookingCount--;}
                        bookings[x] = bookings[x+1];
                    }
                    break;
                }
            }
            saveBookings();
            saveAccommodationData(num);
            break;
        }
    }
}


// Main Function
int main() {
    User loggedInUser;
    loadUserRecord();
    while (1) {
        system("cls");
        int choice;
        printf("\n=== Hotel Management System ===\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearBuffer();

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                if (loginUser(&loggedInUser)) {
                    mainMenu(loggedInUser);
                }
                break;
            case 3:
                system("cls");
                printf("Exit the program.Thank you for using!\n");
                return 0;
            default:
                system("cls");
                printf("Invalid choice. Try again.\n");
        }
    }
}

