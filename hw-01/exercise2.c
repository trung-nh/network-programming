#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SUBJECT_ID_LEN 8
#define SUBJECT_NAME_LEN 30
#define SEMESTER_ID_LEN 6
#define STUDENT_ID_LEN 9
#define SURNAME_LEN 30
#define GIVEN_NAME_LEN 10
#define ROW_LEN 100

/* ------- helper functions -------*/

// function to resolve score board naming with convention
char *resolve_score_board_naming(char subject_id[], char semester_id[])
{
    // 6 --> "_", ".txt" & terminator character "\0"
    char *file_name = malloc(strlen(subject_id) + strlen(semester_id) + 6);
    strcpy(file_name, subject_id);
    strcat(file_name, "_");
    strcat(file_name, semester_id);
    strcat(file_name, ".txt");
    return file_name;
}
char *resolve_report_naming(char subject_id[], char semester_id[])
{
    // 6 --> "_", "_rp.txt" & terminator character "\0"
    char *file_name = malloc(strlen(subject_id) + strlen(semester_id) + 9);
    strcpy(file_name, subject_id);
    strcat(file_name, "_");
    strcat(file_name, semester_id);
    strcat(file_name, "_rp.txt");
    return file_name;
}

int is_file_existed(char *file_name)
{
    FILE *fptr;
    if ((fptr = fopen(file_name, "r")) != NULL)
    {
        // file existed
        fclose(fptr);
        return 1;
    }
    return 0;
}

char resolve_grading(float midterm, float final, int midterm_p, int final_p)
{
    float res = (float)(midterm * midterm_p + final * final_p) / 100;
    if (res >= 8.5)
    {
        return 'A';
    }
    else if (res >= 7)
    {
        return 'B';
    }
    else if (res >= 5.5)
    {
        return 'C';
    }
    else if (res >= 4)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

char *string_tokenizer(char *str, const char delim[], int pos)
{
    char *tmp = malloc(sizeof(char) * strlen(str));
    char *token;
    int i;
    // copy original string to another place
    strcpy(tmp, str);

    for (i = 0; i < pos; i++)
    {
        if (i == 0)
        {
            token = strtok(tmp, delim);
        }
        else
        {
            token = strtok(NULL, delim);
        }
    }
    return token;
}

/* ------- core functions -------*/
void add_score_board()
{
    char subject_id[SUBJECT_ID_LEN], subject_name[SUBJECT_NAME_LEN], semester_id[SEMESTER_ID_LEN];
    int progress_p, final_p, student_count;
    FILE *fptr;

    // fetch input
    printf("Enter subject ID (i.e. IT4062): ");
    scanf("%s", subject_id);
    // printf("sid = %s\n", subject_id);
    printf("Enter subject name (i.e. Network Programming): ");
    getchar();
    // gets(subject_name);
    scanf("%[^\n]s", subject_name);
    // printf("sj name = %s\n", subject_name);
    printf("Enter semester ID (i.e. 20212): ");
    scanf("%s", semester_id);
    // printf("sem id = %s\n", semester_id);
    printf("Enter a percentage for grading progress (i.e. 30): ");
    scanf("%d", &progress_p);
    // printf("p1 = %d\n", progress_p);
    printf("Enter a percentage for grading final (i.e. 70): ");
    scanf("%d", &final_p);
    // printf("p2 = %d\n", final_p);
    printf("Enter the total number of students: ");
    scanf("%d", &student_count);

    // resolve file name
    char *file_name = resolve_score_board_naming(subject_id, semester_id);

    // handle case : file already existed
    if (is_file_existed(file_name))
    {
        printf("<!!!> A score board with subject ID %s and semester ID %s already existed!\n", subject_id, semester_id);
        return;
    }

    // happy case
    fptr = fopen(file_name, "w");
    fprintf(fptr, "SubjectID|%s\n", subject_id);
    fprintf(fptr, "Subject|%s\n", subject_name);
    fprintf(fptr, "F|%d|%d\n", progress_p, final_p);
    fprintf(fptr, "Semester|%s\n", semester_id);
    fprintf(fptr, "StudentCount|%d\n", student_count);
    fclose(fptr);
    printf("New score board added successfully!\n");
}

void append_score()
{
    char subject_id[SUBJECT_ID_LEN], semester_id[SEMESTER_ID_LEN], student_id[STUDENT_ID_LEN], surname[SURNAME_LEN], given_name[GIVEN_NAME_LEN];
    char *token, *tmp = malloc(30);
    const char delim[2] = "|";
    int student_count, i, midterm_p, final_p;
    float midterm, final;
    FILE *fptr;

    // fetch input
    printf("Enter subject ID (i.e. IT4062): ");
    scanf("%s", subject_id);
    printf("Enter semester ID (i.e. 20212): ");
    scanf("%s", semester_id);

    // resolve file name
    char *file_name = resolve_score_board_naming(subject_id, semester_id);

    // handle case : file not initialized
    if (!is_file_existed(file_name))
    {
        printf("<!!!> Score board with subject ID %s and semester ID %s have yet to be initialized!\n", subject_id, semester_id);
        return;
    }

    // file existed
    fptr = fopen(file_name, "r");
    if (fptr == NULL)
    {
        perror("Error opeing file for reading! Please retry!\n");
        return;
    }

    for (i = 0; i < 5; i++)
    {
        fscanf(fptr, "%[^\n]s", tmp);
        fgetc(fptr);
        // scan for progress and final percentages
        if (i == 2)
        {
            token = string_tokenizer(tmp, delim, 2);
            midterm_p = atoi(token);
            token = string_tokenizer(tmp, delim, 3);
            final_p = atoi(token);
        }
    }
    // scan for student count
    token = string_tokenizer(tmp, delim, 2);
    student_count = atoi(token);

    fclose(fptr);
    fptr = fopen(file_name, "a");
    // fetch student info and marks
    for (i = 0; i < student_count; i++)
    {
        printf("Enter student ID (i.e. 20194868): ");
        scanf("%s", student_id);
        printf("Enter student's surname (i.e. Nguyen Hoan): ");
        getchar();
        scanf("%[^\n]s", surname);
        printf("Enter student's given name (i.e. Trung): ");
        getchar();
        scanf("%[^\n]s", given_name);
        printf("Enter student's midterm score (i.e. 9.5): ");
        scanf("%f", &midterm);
        printf("Enter student's final score (i.e. 10): ");
        scanf("%f", &final);
        fprintf(fptr, "S|%s|%s|%s|%.1f|%.1f|%c|\n", student_id, surname, given_name, midterm, final, resolve_grading(midterm, final, midterm_p, final_p));
    }
    fclose(fptr);
    free(tmp);
    printf("Students' marks appended successfully!\n");
}

void remove_score()
{
    char subject_id[SUBJECT_ID_LEN], semester_id[SEMESTER_ID_LEN], student_id[STUDENT_ID_LEN];
    char *tmp = malloc(sizeof(char) * ROW_LEN);
    const char delim[2] = "|";
    int i;
    FILE *fptr, *fptr_cpy;

    // fetch input
    printf("Enter subject ID (i.e. IT4062): ");
    scanf("%s", subject_id);
    printf("Enter semester ID (i.e. 20212): ");
    scanf("%s", semester_id);
    printf("Enter student ID (i.e. 20194868): ");
    scanf("%s", student_id);

    // resolve file name
    char *file_name = resolve_score_board_naming(subject_id, semester_id);
    char *file_name_cpy = malloc(sizeof(char) * (strlen(file_name) + 5));
    // copy file name without extension ('.txt')
    strncpy(file_name_cpy, file_name, strlen(file_name) - 4);
    strcat(file_name_cpy, "_copy.txt");
    // handle case : file not initialized
    if (!is_file_existed(file_name))
    {
        printf("<!!!> Score board with subject ID %s and semester ID %s have yet to be initialized!\n", subject_id, semester_id);
        return;
    }
    fptr = fopen(file_name, "r");
    if (fptr == NULL)
    {
        perror("Error opeing file for reading! Please retry!\n");
        return;
    }

    fptr_cpy = fopen(file_name_cpy, "w");
    while (fscanf(fptr, "%[^\n]s", tmp) != EOF)
    {
        fgetc(fptr);
        char *first_token = string_tokenizer(tmp, delim, 1);
        // check if token points to student info row
        if (strcmp(first_token, "S") == 0)
        {
            // get the 2nd token in that line
            char *second_token = string_tokenizer(tmp, delim, 2);
            // and compare to student ID
            if (strcmp(second_token, student_id) == 0)
            {
                // delete student's marks
                char *third_token = string_tokenizer(tmp, delim, 3);
                char *fourth_token = string_tokenizer(tmp, delim, 4);
                strcpy(tmp, "S|");
                strcat(tmp, second_token);
                strcat(tmp, "|");
                strcat(tmp, third_token);
                strcat(tmp, "|");
                strcat(tmp, fourth_token);
                for (i = 0; i < 4; i++)
                {
                    strcat(tmp, "|");
                }
            }
        }
        fprintf(fptr_cpy, "%s\n", tmp);
    }
    fclose(fptr);
    fclose(fptr_cpy);
    free(tmp);
    free(file_name_cpy);

    // delete old file and rename new file to old name --> better than copying whole file
    remove(file_name);
    if (rename(file_name_cpy, file_name) != 0)
    {
        printf("Cannot modify files as expected!\n");
        return;
    }
    printf("The marks of student with ID %s have been cleared successfully!\n", student_id);
}

void search_score()
{
    char subject_id[SUBJECT_ID_LEN], semester_id[SEMESTER_ID_LEN], student_id[STUDENT_ID_LEN];
    char *tmp = malloc(sizeof(char) * ROW_LEN);
    char *grade = malloc(sizeof(char));
    const char delim[2] = "|";
    float midterm = -1, final = -1;
    FILE *fptr;

    // fetch input
    printf("Enter student ID (i.e. 20194868): ");
    scanf("%s", student_id);
    printf("Enter subject ID (i.e. IT4062): ");
    scanf("%s", subject_id);
    printf("Enter semester ID (i.e. 20212): ");
    scanf("%s", semester_id);

    // resolve file name
    char *file_name = resolve_score_board_naming(subject_id, semester_id);

    // handle case : file not initialized
    if (!is_file_existed(file_name))
    {
        printf("<!!!> Score board with subject ID %s and semester ID %s have yet to be initialized!\n", subject_id, semester_id);
        return;
    }

    fptr = fopen(file_name, "r");
    if (fptr == NULL)
    {
        perror("Error opeing file for reading! Please retry!\n");
        return;
    }
    while (fscanf(fptr, "%[^\n]s", tmp) != EOF)
    {
        fgetc(fptr);
        char *first_token = string_tokenizer(tmp, delim, 1);
        // check if token points to student info row
        if (strcmp(first_token, "S") == 0)
        {
            // get the 2nd token in that line
            char *second_token = string_tokenizer(tmp, delim, 2);
            // and compare to student ID
            if (strcmp(second_token, student_id) == 0)
            {
                // get student's marks
                char *midterm_str = string_tokenizer(tmp, delim, 5);
                char *final_str = string_tokenizer(tmp, delim, 6);
                grade = string_tokenizer(tmp, delim, 7);
                if (midterm_str == NULL || final_str == NULL || grade == NULL)
                {
                    printf("Marks either have been cleared or are incomplete! Cannot fetch them all!\n");
                    return;
                }
                midterm = atof(midterm_str);
                final = atof(final_str);
                break;
            }
        }
    }
    fclose(fptr);
    free(tmp);
    printf("Student with ID %s scored: %.1f (midterm) & %.1f (final) --> graded %s.\n", student_id, midterm, final, grade);
}

void display_score_board_and_report()
{
    char subject_id[SUBJECT_ID_LEN], semester_id[SEMESTER_ID_LEN];
    char *tmp = malloc(sizeof(char) * ROW_LEN);
    char *grade = malloc(1);
    char *highest_surname = malloc(SURNAME_LEN);
    char *highest_given_name = malloc(GIVEN_NAME_LEN);
    char *lowest_surname = malloc(SURNAME_LEN);
    char *lowest_given_name = malloc(GIVEN_NAME_LEN);
    float max_score = -1, min_score = 10, sum_score = 0;
    int i, A_count = 0, B_count = 0, C_count = 0, D_count = 0, F_count = 0, midterm_p, final_p, student_count;
    const char delim[2] = "|";
    FILE *fptr;

    // fetch input
    printf("Enter subject ID (i.e. IT4062): ");
    scanf("%s", subject_id);
    printf("Enter semester ID (i.e. 20212): ");
    scanf("%s", semester_id);

    // resolve file name
    char *file_name = resolve_score_board_naming(subject_id, semester_id);

    // handle case : file not initialized
    if (!is_file_existed(file_name))
    {
        printf("<!!!> Score board with subject ID %s and semester ID %s have yet to be initialized!\n", subject_id, semester_id);
        return;
    }

    fptr = fopen(file_name, "r");
    if (fptr == NULL)
    {
        perror("Error opeing file for reading! Please retry!\n");
        return;
    }
    printf("--------- Score board for subject ID %s in semester %s ---------\n", subject_id, semester_id);
    while (fscanf(fptr, "%[^\n]s", tmp) != EOF)
    {
        fgetc(fptr);
        // print score board
        printf("%s\n", tmp);

        char *first_token = string_tokenizer(tmp, delim, 1);
        // fetch student count
        if (strcmp(first_token, "StudentCount") == 0)
        {
            char *student_count_str = string_tokenizer(tmp, delim, 2);
            if (student_count_str == NULL)
            {
                printf("Student count is missing! Cannot write report!\n");
                return;
            }
            student_count = atoi(student_count_str);
        }
        // handle coefficient
        if (strcmp(first_token, "F") == 0)
        {
            char *midterm_p_str = string_tokenizer(tmp, delim, 2);
            char *final_p_str = string_tokenizer(tmp, delim, 3);
            if (midterm_p_str == NULL || final_p_str == NULL)
            {
                printf("Progress and final coefficients are missing! Cannot write report!\n");
                return;
            }
            midterm_p = atoi(midterm_p_str);
            final_p = atoi(final_p_str);
        }
        // handle student score
        else if (strcmp(first_token, "S") == 0)
        {
            // get the 2nd token in that line
            char *second_token = string_tokenizer(tmp, delim, 2);

            // get student's component marks
            char *midterm_str = string_tokenizer(tmp, delim, 5);
            char *final_str = string_tokenizer(tmp, delim, 6);
            grade = string_tokenizer(tmp, delim, 7);
            if (midterm_str == NULL || final_str == NULL || grade == NULL)
            {
                printf("Marks either have been cleared or are incomplete! Cannot fetch them all!\n");
                return;
            }
            float midterm = atof(midterm_str);
            float final = atof(final_str);
            float score = (float)(midterm * midterm_p + final * final_p) / 100;
            sum_score += score;
            if (score > max_score)
            {
                highest_surname = string_tokenizer(tmp, delim, 3);
                highest_given_name = string_tokenizer(tmp, delim, 4);
                max_score = score;
            }
            if (score < min_score)
            {
                lowest_surname = string_tokenizer(tmp, delim, 3);
                lowest_given_name = string_tokenizer(tmp, delim, 4);
                min_score = score;
            }
            grade = string_tokenizer(tmp, delim, 7);
            if (strcmp(grade, "A") == 0)
            {
                A_count++;
            }
            else if (strcmp(grade, "B") == 0)
            {
                B_count++;
            }
            else if (strcmp(grade, "C") == 0)
            {
                C_count++;
            }
            else if (strcmp(grade, "D") == 0)
            {
                D_count++;
            }
            else
            {
                F_count++;
            }
        }
    }
    fclose(fptr);
    file_name = resolve_report_naming(subject_id, semester_id);
    fptr = fopen(file_name, "w");
    fprintf(fptr, "The student with the highest mark is: %s %s\n", highest_surname, highest_given_name);
    fprintf(fptr, "The student with the lowest mark is: %s %s\n", lowest_surname, lowest_given_name);
    fprintf(fptr, "The average mark is: %.2f\n", (float)(sum_score / student_count));
    fprintf(fptr, "\nA histogram of the subject %s is:\n", subject_id);
    // Histogram A
    fprintf(fptr, "A:");
    for (i = 0; i < A_count; i++)
    {
        fprintf(fptr, "*");
    }
    // Histogram B
    fprintf(fptr, "\nB:");
    for (i = 0; i < B_count; i++)
    {
        fprintf(fptr, "*");
    }
    // Histogram C
    fprintf(fptr, "\nC:");
    for (i = 0; i < C_count; i++)
    {
        fprintf(fptr, "*");
    }
    // Histogram D
    fprintf(fptr, "\nD:");
    for (i = 0; i < D_count; i++)
    {
        fprintf(fptr, "*");
    }
    // Histogram F
    fprintf(fptr, "\nF:");
    for (i = 0; i < F_count; i++)
    {
        fprintf(fptr, "*");
    }
    fclose(fptr);
    printf("Report is successfully generated!\nTake a look...\n");
    fptr = fopen(file_name, "r");
    while (fscanf(fptr, "%[^\n]s", tmp) != EOF)
    {
        fgetc(fptr);
        // print report
        printf("%s\n", tmp);
    }
    free(tmp);
}

void print_menu()
{
    int choice;
    char again;
    do
    {
        printf("\n\n----------- Network Programming HW-01 Exercise 02 -----------\n");
        printf("------- Implemented by : Nguyen Hoan Trung - 20194868 -------\n");
        printf("------------------- Learning Management System -------------------\n\n");
        printf("******************************************************************\n");
        printf("<1> Add a new score board.\n");
        printf("<2> Add score.\n");
        printf("<3> Remove score.\n");
        printf("<4> Search score.\n");
        printf("<5> Display score board and score report.\n");
        printf("Please enter corresponding number 1-5 to your choice (Terminating program otherwise): ");
        scanf("%d", &choice);
        printf("Your choice is %d\n", choice);
        switch (choice)
        {
        case 1:
            printf("\n--------- Adding a new score board ---------\n");
            do
            {
                add_score_board();
                printf("Do you wish to continue adding a new score board? (y/n): ");
                getchar();
                scanf("%c", &again);
            } while (again == 'y' || again == 'Y');
            break;
        case 2:
            printf("\n-------------- Adding score --------------\n");
            do
            {
                append_score();
                printf("Do you wish to continue adding score ? (y/n): ");
                getchar();
                scanf("%c", &again);
            } while (again == 'y' || again == 'Y');
            break;
        case 3:
            printf("\n------------- Removing score -------------\n");
            do
            {
                remove_score();
                printf("Do you wish to continue removing score ? (y/n): ");
                getchar();
                scanf("%c", &again);
            } while (again == 'y' || again == 'Y');
            break;
        case 4:
            printf("\n-------------- Searching score --------------\n");
            do
            {
                search_score();
                printf("Do you wish to continue searching score ? (y/n): ");
                getchar();
                scanf("%c", &again);
            } while (again == 'y' || again == 'Y');
            break;
        case 5:
            printf("\n------ Displaying score board & report ------\n");
            do
            {
                display_score_board_and_report();
                printf("Do you wish to continue displaying score board & report ? (y/n): ");
                getchar();
                scanf("%c", &again);
            } while (again == 'y' || again == 'Y');
            break;
        default:
            printf("Thank you! à bientôt! Program terminated!\n");
            return;
        }
    } while (1);
}

int main()
{
    print_menu();
    return 0;
}