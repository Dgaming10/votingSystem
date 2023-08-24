//Created by Dgaming10
//Works both on windows and unix
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define RESET "\x1B[0m"
#define BOLD "\x1B[1m"

struct citizen {
    const char id[10];
    uint8_t age;
    char name[20];
    bool isAdmin;
    bool voted;
};

struct party {
    char name[15];
    char leader[30];
    uint16_t peopleNum;
    char manifesto[256];
    uint32_t numVotes;
};

struct server {
    struct citizen* allCitizens;
    struct citizen* adminsOnly;
    struct party allParties[10];
    size_t numOfCitizens;
    size_t numOfAdmins;
    size_t numOfParties;
};

#define findElem( type, arr, length,  key, comparator,flag )\
do{\
    *flag = -1;\
    for (size_t i = 0; i < length; i++)\
    {\
        type* current = &((type*)arr)[i];\
        if (comparator(current, key) == 0)\
        {\
            *flag = 1;\
            break;\
        }\
    }\
    if(*flag == -1){*flag = 0;}\
}while(0)\
//generic search algorithm : O(n) time complexity, writing the result to a given flag

signed char charComparator(const void* a, const void* b)
{
    char charA = *(const char*)a;
    char charB = *(const char*)b;
    return charA - charB;
}

bool validID(char* id)
{
    if (strlen(id) != 9) { return false; }
    uint8_t sum = 0;//max sum of such id is 80, therefore the sum value can also fit into one byte memory space

    for (uint8_t i = 0; i < 9; i++)
    {
        if (!isdigit(id[i])) { return false; }
        uint8_t currentNum = id[i] - '0';
        if (i % 2 == 0) { sum += currentNum; }
        else
        {
            currentNum *= 2;
            if (currentNum > 10)
            {
                short tmp = 0;
                for (short i = 0; i < 2; i++)
                {
                    tmp += currentNum % 10;
                    currentNum /= 10;
                }
                currentNum = tmp;
            }
            sum += currentNum;
        }
    }
    return (sum % 10 == 0) ? true : false;
}


struct citizen* findCitizenById(char id[10], struct citizen* group, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (strcmp(id, group[i].id) == 0)
        {
            return &group[i];
        }
    }
    return NULL;
}
char getOption(size_t num, char* options)
{
    char* arr = (char*)malloc(sizeof(char) * (num + 1));
    for (int i = 0; i < num; i++) { arr[i] = options[i]; }
    arr[num] = '\0';
    char tmp;
    while (1)
    {
        scanf(" %c", &tmp);
        char tmp1 = getchar();
        int flag = -1;
        findElem(char, arr, num, &tmp, charComparator, &flag);
        if ((tmp1 == '\n' || tmp1 == EOF) && flag == 1) {
            break;
        }
        printf("Wrong input, please enter a valid number\n");
        if (tmp1 == '\n' || tmp == EOF)
        {
            continue;
        }
        while ((tmp1 = getchar()) != '\n' && tmp1 != EOF);


    }
    free(arr);
    return tmp;
}

char* waitUntillIDValidated()
{
    char* id = (char*)malloc(sizeof(char) * 10);
    while (1)
    {
        scanf(" %9s", id);
        char s = getchar();
        if (validID(id) == true && (s == '\n' || s == EOF)) { break; }
        printf("Your id is not valid, please try again\n");
        if (s == '\n' || s == EOF)
        {
            continue;
        }
        while ((s = getchar()) != '\n' && s != EOF);
    }
    return id;
}

struct citizen* userLogin(struct citizen* allCitizens, size_t numOfCitizens)
{
    printf("Welcome to the user login! please enter your id in order to log in: ");

    char id[10];
    char* ansID = waitUntillIDValidated();
    strcpy(id, ansID);
    free(ansID);
    struct citizen* cit = findCitizenById(id, allCitizens, numOfCitizens);
    if (cit == NULL)
    {
        printf("Sorry, this id is not in our database, you can register if you'd like\n");
        return NULL;
    }
    else if (cit->age < 18)
    {
        printf("Sorry, seems that you are too young for voting, come back later!\n");
        return NULL;
    }
    else if (cit->voted == true)
    {
        printf("Sorry, seems that you have already voted, come back next time!\n");
        return NULL;
    }
    return cit;
}

void adminChoice(struct citizen* adminsOnly, size_t numOfAdmins, struct party* allParties, size_t numOfParties, struct citizen** allCitizens, size_t* numOfCitizens)
{
    printf("------------------------------------------\n  Welcome to the admin login! please enter your id:\t\n------------------------------------------\n");
    char id[10];
    char* ansID = waitUntillIDValidated();
    strcpy(id, ansID);
    free(ansID);
    struct citizen* cit = findCitizenById(id, adminsOnly, numOfAdmins);
    if (cit == NULL)
    {
        printf("Sorry, this id is not in our database\n");
        return;
    }
    else if (cit->isAdmin == false)
    {
        printf("Sorry, you are not an admin\n");
        return;
    }

    while (1)
    {
        printf("----------------------------\nWelcome %s%s%s to the admin panel! please choose between the given options!\n", BOLD, cit->name, RESET);
        printf("Edit party: 0\t|\tRegister new citizen:1\t|\tEdit citizen:2\t|\treturn to menu:3\n");
        char adminOptions[] = { '0','1','2','3' };
        char opt = getOption(4, adminOptions);
        char s;
        if (opt == '0')
        {
            printf("Choose the party you would like to edit:\n");
            char* partyIDS = (char*)malloc(sizeof(char) * (numOfParties + 1));
            partyIDS[numOfParties] = '\0';
            for (size_t i = 0; i < numOfParties; i++)
            {
                printf("%s%d%s: %s - leader: %s \n", BOLD, i, RESET, allParties[i].name, allParties[i].leader);
                partyIDS[i] = i + '0';
            }
            char partyNum = getOption(numOfParties, partyIDS);
            free(partyIDS);
            struct party* chosenParty = &allParties[partyNum - '0'];
            printf("You chose %s%s%s! what information would you like to edit?\n", BOLD, chosenParty->name, RESET);
            printf("name:0\t|\tleader:1\t|\tmanifesto:2\n");
            char chosen = getOption(3, adminOptions);
            if (chosen == '0')
            {
                printf("Enter the new name of the party, the limit is 14 letters:");
                char name[15];
                scanf(" %14s", name);
                strcpy(chosenParty->name, name);
                printf("Name changed to %s!\n", name);
            }
            else if (chosen == '1')
            {
                printf("Enter the new party leader, the limit is 29 letters:");
                char leader[30];
                scanf(" %29s", leader);
                strcpy(chosenParty->leader, leader);
                printf("leader changed to %s!\n", leader);
            }
            else if (chosen == '2')
            {
                printf("Enter the new manifesto, the limit is 255 letters:");
                char manifesto[256];
                scanf(" %255s", manifesto);
                strcpy(chosenParty->manifesto, manifesto);
                printf("manifesto changed to %s\n", manifesto);
            }
            while ((s = getchar()) != '\n' && s != EOF);
        }
        else if (opt == '1')
        {
            printf("Please enter the new citizen's id:");
            char id[10];
            char* ansID = waitUntillIDValidated();
            strcpy(id, ansID);
            free(ansID);
            struct citizen* potentialCitizen = findCitizenById(id, *allCitizens, *numOfCitizens);
            if (potentialCitizen != NULL)
            {
                printf("Sorry, the citizen is already registered!");
            }
            else
            {
                printf("Please enter the new details in the given format:age,name. name limit 19 letters\n");
                char name[20];
                uint8_t age;
                scanf("%hhu,%19s", &age, name);
                *allCitizens = realloc(*allCitizens, sizeof(struct citizen) * (*numOfCitizens + 1));
                (*allCitizens)[*numOfCitizens].age = age;
                (*allCitizens)[*numOfCitizens].isAdmin = false;
                (*allCitizens)[*numOfCitizens].voted = false;
                strcpy((*allCitizens)[*numOfCitizens].name, name);
                strcpy((*allCitizens)[*numOfCitizens].id, id);
                (*numOfCitizens)++;

            }
        }
        else if (opt == '2')
        {
            printf("Choose the id of the citizen you would like to edit:\n");
            while (1)
            {
                scanf(" %9s", id);
                s = getchar();
                if (validID(id) == true && (s == '\n' || s == EOF)) { break; }
                printf("Your id is not valid, please try again\n");
                if (s == '\n' || s == EOF)
                {
                    continue;
                }
                while ((s = getchar()) != '\n' && s != EOF);

            }
            struct citizen* citizenToEdit = findCitizenById(id, *allCitizens, *numOfCitizens);
            if (citizenToEdit == NULL)
            {
                printf("Sorry, the citizen is not in our database!\n");
            }
            else if (citizenToEdit->isAdmin == true)
            {
                printf("Sorry, you can't edit admins!\n");
            }
            else {
                printf("%sid:%s,age:%hd,name:%s,isAdmin:%c,voted:%c%s\n", BOLD, citizenToEdit->id, citizenToEdit->age, citizenToEdit->name, citizenToEdit->isAdmin, citizenToEdit->voted, RESET);
                printf("You chose %s%s%s! enter the new name (limit of 19 letters):\n", BOLD, citizenToEdit->name, RESET);
                char name[20];
                scanf(" %19[^\n]", name);
                printf("The name has changed to %s!\n", name);
                strcpy(citizenToEdit->name, name);
                while ((s = getchar()) != '\n' && s != EOF);
            }
        }
        if (opt == '3')
        {
            break;
        }
    }
}

void userChoice(struct citizen* allCitizens, size_t numOfCitizens, struct party* allParties, size_t numOfParties)
{
    while (1)
    {
        printf("------------------------------------------\n  Welcome to the user panel! please choose between:\t\n------------------------------------------\n");
        printf("login:0\t|\tback to main menu:1\n");
        char userOptions[] = { '0','1' };
        char userChoice = getOption(2, userOptions);
        if (userChoice == '0')
        {
            struct citizen* user = userLogin(allCitizens, numOfCitizens);
            if (user == NULL)
            {
                return;
            }
            printf("Hello %s%s%s, please choose the party for whom you'd like to vote!\n", BOLD, user->name, RESET);
            char* partyIDS = (char*)malloc(sizeof(char) * (numOfParties + 1));
            partyIDS[numOfParties] = '\0';
            for (size_t i = 0; i < numOfParties; i++)
            {
                printf("%s%d%s: %s - leader: %s \n", BOLD, i, RESET, allParties[i].name, allParties[i].leader);
                partyIDS[i] = i + '0';
            }
            char vote = getOption(numOfParties, partyIDS);
            allParties[vote - '0'].numVotes++;
            user->voted = true;
            for (size_t i = 0; i < numOfParties; i++)
            {
                printf("%s - %u\n", allParties[i].name, allParties[i].numVotes);
            }
            free(partyIDS);
        }
        else if (userChoice == '1')
        {
            break;
        }
    }
}

int main(void) {

    struct server mainServer;
    char n[] = "ab";
    for (uint8_t i = 0; i < 10; i++)//limited the number of parties to 10, keeping it simple (:
    {
        mainServer.allParties[i].peopleNum = i + 1;
        strcpy(mainServer.allParties[i].name, n);
        strcpy(mainServer.allParties[i].leader, n);
        strcpy(mainServer.allParties[i].manifesto, "MANIFESTO");
        mainServer.allParties[i].numVotes = 0;
        n[0]++;
        n[1]++;
    }
    mainServer.numOfParties = 10;
    mainServer.numOfAdmins = 1;
    mainServer.numOfCitizens = 1;
    mainServer.allCitizens = (struct citizen*)malloc(sizeof(struct citizen));
    mainServer.adminsOnly = (struct citizen*)malloc(sizeof(struct citizen));
    mainServer.allCitizens[0].age = 18;
    mainServer.allCitizens[0].isAdmin = true;
    mainServer.allCitizens[0].voted = false;
    strcpy(mainServer.allCitizens[0].id, "123456782");
    strcpy(mainServer.allCitizens[0].name, "John");
    mainServer.adminsOnly[0].age = 18;
    mainServer.adminsOnly[0].isAdmin = true;
    mainServer.adminsOnly[0].voted = false;
    strcpy(mainServer.adminsOnly[0].id, "123456782");
    strcpy(mainServer.adminsOnly[0].name, "John");
    //entering initial admin in order to operate with him
    while (1)
    {
        printf("------------------------------------------\n  Welcome to the official voting system!\t\n------------------------------------------\n");
        printf("Login as:  User:0   \t|\tAdmin:1\t|\texit:2\n");
        char initialOptions[] = { '0','1','2' };
        char loginOpt = getOption(3, initialOptions);
        if (loginOpt == '0')
        {
            printf("You are now being forwarded to the user page!\n");
            printf("\033[0;32m");//green color text formatting
            for (int i = 0; i < 35; i++)
            {
                #ifdef _WIN32
                Sleep(50);
                #else
                usleep(50000);
                #endif
                printf("-");
                fflush(stdout);//in case stdout is not automatically flush
            }
            printf("\033[0m\n");
            userChoice(mainServer.allCitizens, mainServer.numOfCitizens, mainServer.allParties, mainServer.numOfParties);
        }
        else if (loginOpt == '1')
        {
            adminChoice(mainServer.adminsOnly, mainServer.numOfAdmins, mainServer.allParties,
                mainServer.numOfParties, &mainServer.allCitizens, &mainServer.numOfCitizens);
            //realloc might change the pointed address without changing the passed one (mainServer.allCitizens),
            // therefore we need to pass the address of the pointer.
        }
        else if (loginOpt == '2')
        {
            break;
        }
    }
    return 0;
}
