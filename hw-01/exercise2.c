#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SUBJECT_ID_LEN 8
#define SUBJECT_NAME_LEN 30
#define SEMESTER_ID_LEN 6
#define STUDENT_ID_LEN 9
#define SURNAME_LEN 30
#define GIVEN_NAME_LEN 10

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
        printf("<!!!> Score board with subject ID %s and semester ID %s have yet to be initialized!", subject_id, semester_id);
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
            char *percent = malloc(sizeof(char) * strlen(tmp));
            strcpy(percent, tmp);
            token = strtok(percent, delim);
            token = strtok(NULL, delim);
            midterm_p = atoi(token);
            token = strtok(NULL, delim);
            final_p = atoi(token);
        }
    }
    // scan for student count
    token = strtok(tmp, delim);
    token = strtok(NULL, delim);
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
        fprintf(fptr, "S|%s|%s|%s|%.1f|%.1f|%c\n", student_id, surname, given_name, midterm, final, resolve_grading(midterm, final, midterm_p, final_p));
    }
    fclose(fptr);
    printf("Students' marks appended successfully!\n");
}

void remove_score()
{
    char subject_id[SUBJECT_ID_LEN], semester_id[SEMESTER_ID_LEN], student_id[STUDENT_ID_LEN];
    char *token, *tmp = malloc(30);
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

    // handle case : file not initialized
    if (!is_file_existed(file_name))
    {
        printf("<!!!> Score board with subject ID %s and semester ID %s have yet to be initialized!", subject_id, semester_id);
        return;
    }
    fptr = fopen(file_name, "r");
    if (fptr == NULL)
    {
        perror("Error opeing file for reading! Please retry!\n");
        return;
    }
    //  TODO
    // fptr_cpy = fopen('')
}

void print_menu()
{
    int choice;

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
            add_score_board();
            break;
        case 2:
            printf("\n-------------- Adding score --------------\n");
            append_score();
            break;
        case 3:
            printf("\n------------- Removing score -------------\n");
            remove_score();
            break;
        case 4:
            printf("\n-------------- Search score --------------\n");
            break;
        case 5:
            printf("\n------ Display score board & report ------\n");

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