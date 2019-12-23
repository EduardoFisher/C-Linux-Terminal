#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80 //The maximum length command
#define MAX_LENGTH 41 //The maximum length of the line

using namespace std;

void Parent_Child();

void read_input(char* statement, char** args, char** args2);
void read_input(char* statement, char** args, int& placement);
bool isPipes(char* statement);
void pipe_test(char** parsed, char** parsedpipe);
void RunArg(char** args);
void find_and_remove(char** args);
//THIS FUNCTION IS USED ONLY FOR DEBUGGING!
void print_args(char** args);

void createArray(char** args);
void deleteArray(char** args);

int main()
{
    /*
     * int** a = new int*[rowcount];
     * for(int i = 0; i < rowcount; i++)
     *  a[i] = new int[colcount];
     *
     */

    bool first_run = true;
    bool good_statment = true;
    bool doPipe = false;
    char PrevStatement[MAX_LENGTH];
    char* args[MAX_LINE/2 + 1]; //command line argument *char[41];
    char* args2[MAX_LINE/2 + 1]; //second command line argument for pipes

    cout << sizeof(args) << endl;
    char statement[MAX_LENGTH];
    int should_run = 1; //flag to determine when to exit program.
    while(should_run)
    {
        //always make a copy of the previous statement for the history feature
        strcpy(PrevStatement, statement);
        cout << "osh>";
        fflush(stdout);
        cin.get(statement, MAX_LENGTH);
        doPipe = isPipes(statement);
        if(doPipe)
            createArray(args2);
        cout << "\n";
        //Create the array so we can use it.
        createArray(args);
        //Read user input.
        read_input(statement, args, args2);
        find_and_remove(args);
        good_statment = true;
        print_args(args);
        print_args(args2);
        cin.ignore();
        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) if command included &, parent will invoke wait()
        */
        if(strcmp(args[0], "exit") == 0)
        {
            exit(0);
        }
        //else if(strcmp(statement, "!!") == 0)
        //RunArg(args);
        //Put the command in history is not the !! feature.
        if(strcmp(statement, "!!") == 0) {
            //delete anything the args array and put the prev statement in there.
            deleteArray(args);
            if(doPipe) {
                deleteArray(args2);
                createArray(args2);
            }
            createArray(args);

            read_input(PrevStatement, args, args2);
            if(strcmp(PrevStatement, "!!") == 0 || first_run == true)
            {
                cout << "No commands in history.\n";
                good_statment = false;
            }
        }
        if(good_statment)  {
            //run the args array
            if(doPipe)
            {
                pipe_test(args, args2);
            }
            else {

                RunArg(args);
            }
        }
        //delete the array after we are done using it.
        deleteArray(args);
        if(doPipe)
            deleteArray(args2);
        first_run = false;
    }

    return 0;
}

//Not Really Needed anymore.
void createArray(char** args)
{
    for(int i = 0; i < MAX_LENGTH; i++)
        args[i] = new char[MAX_LENGTH];
}

void deleteArray(char** args)
{
    for(int i = 0; args[i] != NULL; i++)
        delete args[i];
}

void print_args(char** args)
{
    int i = 0;
    while(args[i] != NULL)
    {
        cout << "args[" << i << "] = \"";
        int j = 0;
        while(args[i][j] != '\0')
        {
            cout << args[i][j];
            j++;
        }
        cout << "\"\n";
        i++;
    }
    cout << "args[" << i << "] = " << "NULL" << "\n";
}

void read_input(char* statement, char** args, char** args2)
{
    bool was_piped = false;
    int i = 0;
    int j = 0;
    int k = 0;
    args[j] = new char[MAX_LENGTH];
    for(;statement[i] != '\0';i++)
    {
        while(statement[i] == ' ')
        {
            if(k != 0)
            {
                args[j][k] = '\0';
                j++;
                //Every time we add a new line add an new char[].
                args[j] = new char[MAX_LENGTH];
                k=0;
            }
            i++;
        }
        if(statement[i] == '|')
        {
            i++;
            read_input(statement, args2, i);
            was_piped = true;
            break;
        }
        args[j][k] = statement[i];
        k++;

    }
    if (was_piped == false) {
        args[j][k] = '\0';//Don't forget to add that null terminating char.
        j++;
    }
    args[j] = new char[MAX_LENGTH];
    args[j] = NULL;
}

void read_input(char* statement, char** args, int& placement)
{
    int j = 0;
    int k = 0;
    args[j] = new char[MAX_LENGTH];
    for(;statement[placement] != '\0';placement++)
    {
        while(statement[placement] == ' ')
        {
            if(k != 0)
            {
                args[j][k] = '\0';
                j++;
                //Every time we add a new line add an new char[].
                args[j] = new char[MAX_LENGTH];
                k=0;
            }
            placement++;
        }
        args[j][k] = statement[placement];
        k++;
    }
    args[j][k] = '\0';//Don't forget to add that null terminating char.
    j++;
    args[j] = new char[MAX_LENGTH];
    args[j] = NULL;
}

void RunArg(char** args) {
    pid_t pid;
    int status;

    //ForK() a child.
    if ((pid = fork()) < 0) {
        cout << "ERROR: FAILED TO FORK A CHILD!\n";
    }
        //Child Process.
    else if (pid == 0) {
        //Run the commmand.
        if (execvp(args[0], args) < 0) {
            cout << "ERROR: FAIL TO EXECUTE COMMAND!\n";
            exit(1);
        }
    }
    else{
        //Wait for completion.
        while(wait(&status) != pid);
    }

}

//Parent/Child Process example
void Parent_Child()
{
    pid_t pid;
    // fork a child process.
    pid = fork();

    if (pid < 0)//error occurred.
    {
        fprintf(stderr, "Fork Failed");
    }
    else if (pid == 0)//child process
    {
        execlp("/bin/ls","ls",NULL);
    }
    else //parent process.
    {
        //parent will wait for the child to complete.
        wait(NULL);
        cout << "Child Complete" << endl;
    }
}

void find_and_remove(char** args)
{
    for(int i =0; args[i] != NULL; i++)
    {
        if(strcmp(args[i], "&") == 0)
        {
            args[i] = NULL;
        }
    }
}

bool isPipes(char* statement)
{
    for(int i = 0; statement[i] != '\0'; i++)
    {
        if(statement[i] == '|')
            return true;
    }
    return false;
}

void pipe_test(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2];

    pid_t p1, p2;

    int status1, status2;

    if(pipe(pipefd) < 0)
    {
        cout << "\nPipe could not be initialized";
        return;
    }
    p1 = fork();
    if(p1 < 0)
    {
        cout << "\nCould not fork";
        return;
    }

    if(p1 == 0)
    {
        //Child 1 executing...
        //It only needs to write at the write end...
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if(execvp(parsed[0], parsed) < 0)
        {
            cout << "\nCould not execute command 1...";
            exit(0);
        }
    } else{
        //Parent executing
        p2 = fork();
        if(p2 < 0)
        {
            cout << "\nCould not fork";
            return;
        }
        //Child 2 executing...
        //It only needs to read at the read end
        if(p2 == 0)
        {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            if(execvp(parsedpipe[0], parsedpipe) < 0)
            {
                cout << "\nCould not execute command 1...";
                exit(0);
            }
        }
        else{
            // parent executing, waiting for two children.
            close(pipefd[0]);
            close(pipefd[1]);
            wait(&status1);
            wait(&status2);
        }
    }

}