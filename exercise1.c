#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX_ATTEMPTS 3

void cipher_code(char *code)
{
    // init string constants
    char const *const MATERIAL_1 = "Jets";
    char const *const MATERIAL_2 = "Fog";
    char const *const MATERIAL_3 = "Foam";
    char const *const MATERIAL_4 = "Dry agent";
    char const *const REACTIVITY_V = "Can be violently reactive";
    char const *const NO_INFORMATION = "N/A";
    char const *const PROTECTION_FULL = "Full protective clothing must be worn";
    char const *const PROTECTION_BA = "Breathing apparatus";
    char const *const PROTECTION_BA_FIRE_ONLY = "Breathing apparatus, protective gloves for fire only";
    char const *const CONTAINMENT_DILUTE = "May be diluted and washed down the drain";
    char const *const CONTAINMENT_CONTAIN = "Avoid spillages from entering drains or water courses";
    char const *const EVACUATION = "Consider evacuation";

    // validation
    int code_len = strlen(code);
    if (code_len < 2 || code_len > 3)
    {
        printf("<!!!> Invalid HAZCHEM code! Please retry!\n");
        return;
    }
    // assign characters
    const int CHAR_1 = code[0] - '0';
    const char CHAR_2 = toupper(code[1]);
    const char CHAR_3 = toupper(code[2]);
    int is_black = 1;

    if (CHAR_1 < 1 || CHAR_1 > 4)
    {
        printf("<!!!> Invalid HAZCHEM code! Please retry!\n");
        return;
    }
    if (CHAR_2 != 'P' && CHAR_2 != 'R' && CHAR_2 != 'S' && CHAR_2 != 'T' && CHAR_2 != 'W' && CHAR_2 != 'X' && CHAR_2 != 'Y' && CHAR_2 != 'Z')
    {
        printf("<!!!> Invalid HAZCHEM code! Please retry!\n");
        return;
    }
    if (code_len == 3 && CHAR_3 != 'E')
    {
        printf("<!!!> Invalid HAZCHEM code! Please retry!\n");
        return;
    }

    // handle cases of S,T,Y,Z
    if (CHAR_2 == 'S' || CHAR_2 == 'T' || CHAR_2 == 'Y' || CHAR_2 == 'Z')
    {
        char color;
        int is_invalid_input = 1;

        do
        {
            printf("Color confirmation...Enter 'B' for black, 'W' for white (B/W): ");
            scanf("%c", &color);
            switch (toupper(color))
            {
            case 'W':
                is_black = 0;
                is_invalid_input = 0;
                break;
            case 'B':
                is_invalid_input = 0;
                break;

            default:
                printf("Invalid color input!\n");
                break;
            }
        } while (is_invalid_input);
    }

    // display actions
    printf("\n\n***** Emergency action advice *****\n");

    // code
    printf("Code:\t%s\n", code);

    // material
    printf("Material:\t");
    switch (CHAR_1)
    {
    case 1:
        printf("%s\n", MATERIAL_1);
        break;
    case 2:
        printf("%s\n", MATERIAL_2);
        break;
    case 3:
        printf("%s\n", MATERIAL_3);
        break;
    case 4:
        printf("%s\n", MATERIAL_4);
        break;

    default:
        printf("Something went wrong please relaunch the program!\n");
        break;
    }

    // reactivity
    printf("Reactivity:\t");
    switch (CHAR_2)
    {
    case 'P':
    case 'S':
    case 'W':
    case 'Y':
    case 'Z':
        printf("%s\n", REACTIVITY_V);
        break;
    case 'R':
    case 'T':
    case 'X':
        printf("%s\n", NO_INFORMATION);
        break;
    default:
        printf("Something went wrong please relaunch the program!\n");
        break;
    }

    // protection
    printf("Protection:\t");
    switch (CHAR_2)
    {
    case 'P':
    case 'R':
    case 'W':
    case 'X':
        printf("%s\n", PROTECTION_FULL);
        break;
    case 'S':
    case 'T':
    case 'Y':
    case 'Z':
        if (is_black)
        {
            printf("%s\n", PROTECTION_BA);
        }
        else
        {
            printf("%s\n", PROTECTION_BA_FIRE_ONLY);
        }
        break;
    default:
        printf("Something went wrong please relaunch the program!\n");
        break;
    }

    // containment
    printf("Containment:\t");
    switch (CHAR_2)
    {
    case 'P':
    case 'R':
    case 'S':
    case 'T':
        printf("%s\n", CONTAINMENT_DILUTE);
        break;
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
        printf("%s\n", CONTAINMENT_CONTAIN);
        break;
    default:
        printf("Something went wrong please relaunch the program!\n");
        break;
    }

    // evacuation
    printf("Evacuation:\t");
    if (code_len == 3)
    {
        printf("%s\n", EVACUATION);
    }
    else
    {
        printf("%s\n", NO_INFORMATION);
    }

    printf("*********************************\n\n");
}

void print_menu()
{
    int choice;
    char code[4];
    int failed_attempts = 0;

    do
    {
        printf("\n\n----------- Network Programming HW-01 Exercise 01 -----------\n");
        printf("------- Implemented by : Nguyen Hoan Trung - 20194868 -------\n");
        printf("----------------------- HAZCHEM code -----------------------\n\n");
        printf("<0> Exit program.\n");
        printf("<1> Enter a HAZCHEM code.\n");
        printf("Please enter corresponding number 0-1 to your option: ");
        scanf("%d", &choice);
        printf("Your choice is %d\n", choice);
        switch (choice)
        {
        case 0:
            printf("Thank you! à bientôt!\n");
            return;
        case 1:
            printf("Please enter a HAZCHEM code : ");
            scanf("%s", code);
            cipher_code(code);
            break;
        default:
            if (failed_attempts >= MAX_ATTEMPTS)
            {
                printf("Too many failed attempts! Program terminated! \n");
                return;
            }

            printf("Invalid choice! Please enter a number 0-1!\n");
            failed_attempts++;
            break;
        }
    } while (choice != 0);
}

int main()
{
    print_menu();
    return 0;
}