#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <sys/stat.h>

#define N 20
#define MAX_ACCOMMODATION 100

typedef struct Room
{
    char room_type[N];
    char bed[N];
    float area;
    char feature[N][N];
    float price;
    unsigned int num;
}Room;

typedef struct Accommodation
{
    char room_ID[N];
    bool availability;
    char room_type[N];
} Accommodation;

Room room[MAX_ACCOMMODATION];
Accommodation accommodation[MAX_ACCOMMODATION];

int loadRoomType()
{
    FILE *file = fopen("./data/room.txt", "rb");
    if (file == NULL)
    {
        perror("Error opening file for reading");
        memset(room, '\0', sizeof(room));
    }
    int max_type = 0;
    Room buffer;
    while (fread(&buffer, sizeof(Room), 1, file))
    {
        room[max_type] = buffer; max_type++;
    }
    return max_type;
}

void saveRoomType(int size)
{
    FILE *outfile = fopen("./data/room.txt", "wb");
    if (outfile == NULL)
    {
        mkdir("data");
        outfile = fopen("./data/room.txt", "wb");
        // perror("Error opening file for writing");
    }
    fwrite(&room, sizeof(Room), size, outfile);
    fclose(outfile);
}

int loadAccommodationData() 
{
    FILE *file = fopen("./data/accommodation.txt", "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return 0;
    }
    int i = 0, size = 0;
    if (fscanf(file, "%d\n", &size) != 1) {
        perror("Error reading record count");
        fclose(file);
        return 0;
    }
    while (i < size && fscanf(file, "%s %d %[^\n]", accommodation[i].room_ID, (int*)&accommodation[i].availability, accommodation[i].room_type) == 3) {
        i++;
    }
    fclose(file);
    return size;
}

void saveAccommodationData(int size) 
{
    FILE *file = fopen("./data/accommodation.txt", "w");
    if (file == NULL) {
        mkdir("data");
        file = fopen("./data/room.txt", "wb");
    }
    fprintf(file, "%d\n", size);
    for (int i = 0; i < size; i++) {
        fprintf(file, "%s %d %s\n", accommodation[i].room_ID, accommodation[i].availability, accommodation[i].room_type);
    }
    fclose(file);
    printf("Data saved successfully.\n");
}


// For Customer and Emplyee to view
void viewAccommodation() 
{
    system("cls");
    int roomCount = loadRoomType();
    printf("====================================================================================================================================================\n");
    printf("| %-20s | %-13s | %-20s | %-8s | %-60s | %-8s |\n", 
           "Room Type", "Num Available", "Bed Info", "Area", "Feature", "Price");
    printf("====================================================================================================================================================\n");

    for (int i = 0; i < roomCount; i++) {
        printf("| %-20s | %-13u | %-20s | %-8.2f |", 
               room[i].room_type, (room[i].num-1), room[i].bed, room[i].area);
        for (int j = 0; j < N ; j++) 
        {
            if (room[i].feature[j][0] == '\0')
            {
                int temp = 20*(3 - (j % 3));
                switch (temp)
                {
                    case 0: printf("%-60s |", " "); break;
                    case 20: printf("%-20s |", " "); break;
                    case 40: printf("%-40s |", " "); break;
                    case 60: printf("%-60s |", " "); break;
                }
                if (j <= 2) {
                    printf(" %-8.2f |", room[i].price);
                }
                break;
            }
            else
            {
                printf("%-20s", room[i].feature[j] ? room[i].feature[j] : ""); 
                if (((j+1) % 3 == 0 && j !=0))
                {
                    printf(" |");
                    if ((j+1) == 3) {
                        printf(" %-8.2f |\n", room[i].price);
                    }
                    else
                    {
                        printf(" %-8s |\n", "");
                    }
                    printf("| %-20s | %-13s | %-20s | %-8s |", "", "", "", "");
                }
            }
        }
        printf("\n----------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
}

// Only For employee to see which specific room is available
void displayIDAvailable(char *type) 
{
    int roomCount = loadAccommodationData();
    printf("====================================================================================================================================================\n");
    printf("| %-20s | %-61s |\n", "Room Type", "Room ID Available");
    printf("====================================================================================================================================================\n");
    printf("| %-20s |", type);
    const int columndata = 3;
    int j = 0;
    for (int i = 0; i < roomCount; i++) 
    {
        if (!accommodation[i].availability && strcasecmp(accommodation[i].room_type, type) == 0)
        {
            printf(" %-20s", accommodation[i].room_ID);
            if ((j+1) % columndata == 0)
            {
                printf("|\n| %-20s |", "");
            }
            j++;
        }
    }
    printf("\n----------------------------------------------------------------------------------------------------------------------------------------------------\n");
    char key;
    printf("\n Enter to continue.");
    scanf("%c", &key);
}


// For Admin perspective / Viewing
void displayAccommodationData(int num)
{
    printf("\nAccommodation Data:\n");
    printf("====================================================            ===========================================\n");
    printf("| %-10s | %-12s | %-20s |\t\t| %-20s | %-5s | %-5s |\n", "Room ID", "Availability", "Room Type", "Bed Info", "Area", "Price");
    printf("====================================================            ===========================================\n");

    int idx = 0;
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < N; j++)
        {
            if (strcasecmp(accommodation[i].room_type, room[j].room_type) == 0)
            {
                idx = j;
            }
        } 
        printf("| %-10s | %-12s | %-20s |\t\t| %-20s | %3.2f | %3.2f |\n", 
               accommodation[i].room_ID, 
               accommodation[i].availability ? "Unavailable" : "Available", 
               accommodation[i].room_type,
               room[idx].bed, 
               room[idx].area, 
               room[idx].price);
    }

    printf("====================================================            ===========================================\n");

    printf("\nConclusions:\n");
    printf("==========================================\n");
    printf("| %-20s | %-15s |\n", "Room Type", "Number of Room");
    for (int i = 0; i < N; i++)
    {
        if ((int)room[i].room_type[0] == 0) {break;}
        printf("| %-20s | %-15d |\n", room[i].room_type, (room[i].num-1));
    }
    printf("==========================================\n");
    printf("Total Number of Room: %d\n", num);
    printf("==========================================\n\n");
}

void displayRoomType(int num, int specific)
{
    printf("\nROOM TYPE:\n");
    printf("===================================================================================================================================================================================\n");
    printf("| %-15s | %-15s | %-5s | %-120s | %-8s |\n", "Room Type", "Bed Info", "Area", "Feature", "Price");
    printf("===================================================================================================================================================================================\n");
    if (specific == -1){
        for (int i = 0; i < num; i++){
            int j = 0;
            char temp[N*(N+1)];
            memset(temp, '\0', sizeof(temp));
            printf("| %-15s | %-15s | %3.2f |", room[i].room_type, room[i].bed, room[i].area);
            while (true)
            {   
                if ((int)room[i].feature[j][0]!= 0) 
                {
                    strcat(temp, room[i].feature[j]);
                    strcat(temp, ",");
                }
                else {break;}
                j++;
            }
            printf(" %-120s |", temp);
            printf("RM %3.2f |\n", room[i].price);
        }
        printf("===================================================================================================================================================================================\n");
        return;
    }
    else {
        if (specific >= 0 && specific < num) {
            int j = 0;
            char temp[N*(N+1)];
            printf("| %-15s | %-15s | %3.2f |", room[specific].room_type, room[specific].bed, room[specific].area);
            while (true)
            {   
                if ((int)room[specific].feature[j][0]!= 0) 
                {
                    strcat(temp, room[specific].feature[j]);
                    strcat(temp, ",");
                }
                else {break;}
                j++;
            }
            printf(" %-120s |", temp);
            printf("RM %3.2f |\n", room[specific].price);
        }
        else 
        {
            printf("Invalid room index: %d\n", specific);
        }
    }
}

// For Admin Operations
int RoomRecord()
{   
    int max_type = loadRoomType();
    while (true) {
        system("cls");
        displayRoomType(max_type, -1);
        char type[N];
        do{
            printf("\n\nUpdate Room Type (input 'end' to exit)\n");
            printf("Enter a Room Type: ");
            scanf("%[^\n]", &type);
            getchar();
            printf("%s", type);
        }while (strlen(type) == 0);
        if (strcasecmp(type, "end")==0) {break;}
        bool other = true;
        for (int i = 0; i < max_type; i++)
        {
            if (strcasecmp(room[i].room_type, type)==0)
            {   
                other = false;
                int choice;
                while (true)
                {
                    system("cls");
                    displayRoomType(max_type, i);
                    printf("\n\n\tEditing Type\t");
                    printf("\n1. Edit Room Type\n2. Edit Bed Info\n3. Edit Room Area\n4. Edit Room Feature\n5. Edit Room Price\n6. Delete Room Type\n7. Exit\n");
                    scanf("%d", &choice);
                    getchar();
                    system("cls");
                    displayRoomType(max_type, i);
                    if (choice == 1)
                    {
                        printf("Change Room Type: ");
                        scanf("%[^\n]", &room[i].room_type);
                        getchar();
                    }
                    else if (choice == 2)
                    {
                        printf("Change Bed info: ");
                        scanf("%[^\n]", &room[i].bed);
                        getchar();
                    }
                    else if (choice == 3)
                    {
                        printf("Change Room Area: ");
                        scanf("%f", &room[i].area);
                        getchar();
                    }
                    else if (choice == 4)
                    {
                        int feature_idx = -1;
                        while ((int)room[i].feature[feature_idx+1][0] != 0){feature_idx++;}
                        while (true)
                        {
                            system("cls");
                            displayRoomType(max_type, i);
                            printf("\n\tEditing Feature for Room Type: %s\t", room[i].room_type);
                            printf("\n1. Add New Feature\n2. Remove Feature\n3. Clear All Feature\n4. Exit\n");
                            scanf("%d", &choice);
                            getchar();
                            if (choice == 1)
                            {
                                while (true)
                                {
                                    char temp[N];
                                    printf("Add new Room Feature: ");
                                    scanf("%[^\n]", &temp);
                                    getchar();
                                    if (strcasecmp(temp, "end")==0){break;}
                                    feature_idx++;
                                    strcpy(room[i].feature[feature_idx], temp); 
                                    strcpy(room[i].feature[feature_idx+1], "\0");
                                }
                            }
                            else if (choice == 2)
                            {
                                int idx = -1;
                                char feature[N];
                                printf("%d", feature_idx);
                                printf("Enter feature to remove: ");
                                scanf("%[^\n]", &feature);
                                getchar();
                                for (int j = 0; j < feature_idx+1; j++)
                                {
                                    if (strcasecmp(room[i].feature[j], feature)==0)
                                    {
                                        idx=j;
                                    }
                                }
                                if (idx != -1)
                                {
                                    room[i].feature[idx][0] = '\n';
                                    for (int j = idx; j <= feature_idx; j++)
                                    {
                                        if (j == (feature_idx))
                                        {
                                             room[i].feature[j][0] = '\0';
                                             break;
                                        }
                                        strcpy(room[i].feature[j], room[i].feature[j+1]);
                                    }
                                    feature_idx--;
                                }
                                else {printf("Feature Not Found\n");}
                            }
                            else if (choice == 3)
                            {
                                for (int j = 0; j <= feature_idx; j++)
                                {
                                    strcpy(room[i].feature[j], "\0");
                                }
                                printf("All features have been removed.\n");
                            }
                            else if (choice == 4){break;}
                        }
                        
                    }
                    else if (choice == 5)
                    {
                        printf("Change Room Price: ");
                        scanf("%f", &room[i].price);
                        getchar();
                    }
                    else if (choice == 6)
                    {
                        for (int j = i; j < max_type; j++)
                        {
                            room[j] = room[j+1];
                        }
                        max_type--;
                        break;
                    }
                    else if (choice == 7){break;}
                }
            }
        }
        if (other)
        {
            system("cls");
            printf("\nRoom Type %s not found. So a new type will be added\n", type);
            strcpy(room[max_type].room_type, type);

            printf("Enter new Room Bed Info: ");
            scanf("%[^\n]", &room[max_type].bed);
            getchar();

            printf("Enter new Room Area: ");
            scanf("%f", &room[max_type].area);
            getchar();

            room[max_type].feature[0][0] = '\0';

            printf("Enter new Room Price: ");
            scanf("%f", &room[max_type].price);
            getchar();

            room[max_type].num++;
            max_type++;
        }
    }
    return max_type;
}

void AccommodationRecord()
{   
    int max_type = RoomRecord();
    int room_num = loadAccommodationData();
    
    while (true)
    {
        system("cls");
        displayAccommodationData(room_num);
        bool other = true;
        char temp[N];
        int choice;
        do {
            printf("Record a room ID (input 'end' to exit): ");
            scanf("%s", &temp);
            getchar();
        }while (strlen(temp) == 0);
        if (strcasecmp(temp, "end")==0){break;}
        for (int i = 0; i < room_num; i++)
        {
            if (strcasecmp(accommodation[i].room_ID, temp)==0)
            {
                other = false;
                printf("\nRoom ID already exists\n");
                printf("Select operation:\n1. Update\n2. Delete\n");
                scanf("%d", &choice);
                getchar();
                for (int j = 0; j < max_type; j++)
                {
                    if (strcasecmp(room[j].room_type, accommodation[i].room_type)==0)
                    {
                        room[j].num--;
                        break;
                    }
                }
                if (choice == 1)
                {
                    printf("Update Room ID: ");
                    scanf("%s", &accommodation[i].room_ID);
                    getchar();
                    while (true)
                    {
                        printf("Update Room Type: ");
                        scanf("%[^\n]", &temp);
                        getchar();
                        bool other1 = true;
                        for (int j = 0; j < max_type; j++)
                        {
                            if (strcasecmp(room[j].room_type, temp)==0)
                            {
                                other1 = false;
                                strcpy(accommodation[i].room_type, temp);
                                room[j].num++;
                                break;
                            }
                        }
                        if (!other1){break;}
                        printf("Room Type not found\n");
                    }
                }
                else if (choice == 2)
                {
                    printf("Room ID %s has been deleted.\n", temp);
                    for (int j = i; j < room_num; j++)
                    {
                        accommodation[j] = accommodation[j+1];
                    }
                    room_num--;
                    break;
                }
                else
                {
                    printf("Invalid choice\n");
                }
            }
        }
        if (other)
        {
            strcpy(accommodation[room_num].room_ID, temp);
            accommodation[room_num].availability = false;

            while (true){
                printf("Record room type: ");
                scanf("%[^\n]", &temp);
                getchar();
                other = true;
                for (int i = 0; i < max_type; i++)
                {
                    if (strcasecmp(room[i].room_type, temp)==0)
                    {
                        other = false;
                        strcpy(accommodation[room_num].room_type, room[i].room_type);
                        room[i].num++;
                        break;
                    }
                }
                if (!other){break;}
                printf("Room Type not found\n");
            }
            room_num++;
        }
    }
    saveRoomType(max_type);
    saveAccommodationData(room_num);
}
