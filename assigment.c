#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

pthread_t tid[2];
int counter;
pthread_mutex_t lock;

typedef struct {
   char name[31];
   char email[47];
   float gpa;
   unsigned int toeic;
   unsigned int credit;
   unsigned int fee;
} Student;

Student student_1 = (Student){"Vuong Kieu Oanh","oanh.vk172742@sis.hust.edu.vn",2.80,400,64,0};

typedef struct {
    Student* student;
    float gpa;
    unsigned int toeic;
    unsigned int credit;
    unsigned int fee;
} Arguments;

void* job(void *arguments)
{
    pthread_mutex_lock(&lock);

    unsigned long thread_id = pthread_self();
    counter += 1;
    printf("\nJob %d started\n", counter);

    //define type of arguments poninter
    Arguments *args = (Arguments *) arguments;
    
    //update data
    args->student->gpa = args->gpa;
    args->student->toeic = args->toeic;
    args->student->credit = args->credit;
    args->student->fee = args->fee;

    //show update
    printf("   Updated information:\n");
    printf("     Name: %s\n",args->student->name);
    printf("     GPA: %.2f\n",args->student->gpa);
    printf("     TOEIC: %u\n",args->student->toeic);
    printf("     Passed credit: %u\n",args->student->credit);
    printf("     Tuition fee: %u\n",args->student->fee);
    printf("   Thread ID: %lu\n",thread_id);
    
    //edit email
    char filename[20];
    sprintf(filename,"mail_%d.txt",counter);
    FILE *fp = fopen(filename,"w+");
     // open it for writing.
    fprintf(fp,"To: %s\n",args->student->email);
    fprintf(fp, "------------------------------------\n");
    fprintf(fp,"Name: %s\n",args->student->name);        
    fprintf(fp,"GPA: %.2f\n",args->student->gpa);
    fprintf(fp,"TOEIC: %u\n",args->student->toeic);
    fprintf(fp,"Passed credit: %u\n",args->student->credit);
    fprintf(fp,"Tuition fee: %u\n",args->student->fee);
    fprintf(fp,"Thread ID: %lu\n",thread_id);
    fprintf(fp, "------------------------------------\n");
    fclose(fp);   

    printf("Job %d finished\n", counter);

    pthread_mutex_unlock(&lock);

    return NULL;
}

void send_email(char* filename){
    //open mail file
    FILE *fp = fopen(filename,"r+");
    char to[47]; // email of the recepient.
    fscanf(fp,"To: %s", to);
    fclose(fp); 
    
    char cmd[100];  // to hold the command.
    sprintf(cmd,"sendmail %s < %s",to,filename);
    system(cmd);     // execute it.
}

int main(void)
{
    int err;
    int i = 0;
    Arguments args[2];
    args[0] = (Arguments){&student_1,2.92,450,80,10000000};
    args[1] = (Arguments){&student_1,3.41,460,96,2000000};

    printf("\n   Current information:\n");
    printf("     Name: %s\n",student_1.name);
    printf("     GPA: %.2f\n",student_1.gpa);
    printf("     TOEIC: %u\n",student_1.toeic);
    printf("     Passed credit: %u\n",student_1.credit);
    printf("     Tuition fee: %u\n",student_1.fee);
    

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    while(i<2){
        err = pthread_create(&(tid[i]), NULL, &job, (void *) &args[i]) ;
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
    }

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_mutex_destroy(&lock);


    for(i=0; i<2; i++){
        char mail_file[20];
        sprintf(mail_file,"mail_%d.txt",i+1);
        send_email(mail_file);
    }
    printf("Sending mail done.\n");

    return 0;
}
