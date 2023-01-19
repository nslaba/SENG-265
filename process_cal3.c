/*
 * process_cal3.c
 *
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2021.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"
#include "time.h"
#include "stdbool.h"

#define MAX_LINE_LEN 132
#define MAX_EVENTS 500

time_t convert_string_to_datetime(char *);
time_t add_days(time_t, int);
time_t get_datetime_from_ints(int y, int m, int d);
char* format_date(time_t);
char* format_summary(node_t* node);
char* get_dash_string(char* string);


node_line *read_file(char* file_name) {
    FILE *fp;
    char buffer[MAX_LINE_LEN];
    node_line *head = NULL;

    fp = fopen(file_name, "r");

    fgets(buffer, sizeof(buffer), fp);
    head = new_node1(buffer);
    
    while(fgets(buffer, sizeof(buffer), fp) != NULL) {
        add_end1(head, new_node1(buffer));
    }
    fclose(fp);
    return head;
    
} 

node_t* parse_events(node_line* file_content_head) 
{
    node_t *head = NULL;
    
    node_line *curr = NULL;

    char key[MAX_LINE_LEN];
    char value[MAX_LINE_LEN];
    char c;

    for(curr = file_content_head; curr != NULL; curr = curr->next){
        sscanf(curr->fline, "%[^:]%c%[^\n]", key, &c, value);
        
        struct event_t event;
        if (strcmp(key, "DTSTART")==0) {
            event.dtstart = convert_string_to_datetime(value);
        } else if (strcmp(key, "DTEND")==0) {
            event.dtend = convert_string_to_datetime(value);
        } else if (strcmp(key, "SUMMARY")==0) {
            strcpy(event.summary, value);
        } else if (strcmp(key, "LOCATION")==0) {
            char empty[1] ="";
            if (value[0] == '2') {
                strcpy(event.location, empty);
            } else {
                strcpy(event.location, value);
            }
        } else if (strcmp(key, "RRULE")==0) {
            strcpy(event.rrule, value);
        } else if (strcmp(key,"END")==0 && 0 == strcmp(value, "VEVENT")) {
            head = add_end(head, new_node(event));
            strcpy(event.rrule, "");
        } else {
            continue;
        }
    }
    
     deallocate_mem_line_list(file_content_head);
     return head;
}

node_t* explode_recurring_events(node_t *head){
    node_t *curr = NULL;
    //create a new list to append to.
    node_t *new_head=NULL;
    
    for (curr = head; curr != NULL; curr = curr-> next) {
        if (strcmp(curr->val.rrule, "") != 0) {
            
            //create copy of node and append it to new list.
            struct event_t val_copy;
            val_copy =curr->val;
            new_head =add_end(new_head, new_node(val_copy));
            //getting expiry date from rrule value
            char expiry_date[MAX_LINE_LEN];
            char c;
            char poop[MAX_LINE_LEN];
            
            sscanf(val_copy.rrule, "%[^U]%c%s", poop, &c, expiry_date);
            sscanf(expiry_date, "%[^=]%c%s", poop, &c, expiry_date);
            sscanf(expiry_date, "%[^;]%c%s", expiry_date, &c, poop);
            
            //converting expiry date to datetime obj
            time_t exp_datetime;
            exp_datetime = convert_string_to_datetime(expiry_date);            

            val_copy.dtstart = add_days(val_copy.dtstart, 7);
            val_copy.dtend = add_days(val_copy.dtend, 7);
           
            while (difftime(exp_datetime, val_copy.dtstart) >= 0) {
                //append the new node and increase dtstart &dtend
                new_head = add_end(new_head, new_node(val_copy));
                val_copy.dtstart = add_days(val_copy.dtstart, 7);
                val_copy.dtend = add_days(val_copy.dtend, 7);
                
            }

        } else {
            struct event_t val_copy;
            val_copy = curr->val;
            new_head = add_end(new_head, new_node(val_copy));
        }
    }
    
    return new_head;
}

time_t add_days(time_t time, int days_to_add) {
    struct tm* srctime;
    srctime = localtime(&time);

    srctime->tm_mday +=days_to_add;
    return mktime(srctime);
}


time_t convert_string_to_datetime(char * dt_time) {
    struct tm temp_time;

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year, 
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour, 
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -=1900;
    temp_time.tm_mon -= 1;
    return mktime(&temp_time);
}
time_t get_datetime_from_ints(int y, int m, int d){
    struct tm temp_time;

    memset(&temp_time, 0, sizeof(struct tm));
    temp_time.tm_year = y;
    temp_time.tm_mon = m;
    temp_time.tm_mday = d;
    temp_time.tm_year -=1900;
    temp_time.tm_mon -=1;
    return mktime(&temp_time);

}
void pretty_print(node_t *head, time_t from_date, time_t to_date) {
    //grabbing midnight of next day :)
    to_date = add_days(to_date, 1);
    //cropping head off
    node_t *cur, *curr, *current, *new_head;
    new_head = NULL;
    if (difftime(head->val.dtstart, from_date) >= 0) {
        new_head = head;
    } else {
        for (curr = head; curr != NULL; curr = curr->next) {
            if (curr->next !=NULL && difftime(curr->next->val.dtstart, from_date) >=0) {
                new_head = curr->next;
                curr->next = NULL;
                break;
            }
        }
    }
    
    if (new_head == NULL) return;
    // cropping tail off
    for (current = new_head; current->next != NULL; current = current->next) {
        if (difftime(current->next->val.dtstart, to_date) >0) {
           current->next = NULL;
           break;
        }
    }

    char today[MAX_LINE_LEN];
    strcpy(today, format_date(new_head->val.dtstart));
    printf("%s\n", today);
    printf("%s\n", get_dash_string(today));
    for (cur = new_head; cur != NULL; cur = cur->next) {
        if (strcmp(today, format_date(cur->val.dtstart)) !=0) {
            strcpy(today, format_date(cur->val.dtstart));
            printf("%s\n", today);
            printf("%s\n", get_dash_string(today));
        }
        if (cur->next == NULL || (strcmp(today, format_date(cur->next->val.dtstart)) ==0)) {
            printf("%s\n", format_summary(cur));
        } else {
            printf("%s\n\n", format_summary(cur));
        }
     }
}
char *format_date(time_t time) {
    char buffer[80];
    char* buffer_ptr=NULL;
    buffer_ptr = &buffer[0];
    strftime(buffer, sizeof(buffer), "%B %d, %Y (%a)", localtime(&time));
    return buffer_ptr;
}
char * format_summary(node_t* node) {
    char buffer[500];
    char* buffer_ptr = NULL;
    buffer_ptr = &buffer[0];

    char start_time[MAX_LINE_LEN];
    char end_time[MAX_LINE_LEN];    
   //have to get rid of zeroes with space_padding
 
    strftime(start_time, MAX_LINE_LEN, "%I:%M %p", localtime(&node->val.dtstart));
    strftime(end_time, MAX_LINE_LEN, "%I:%M %p", localtime(&node->val.dtend));
    
    if (start_time[0] == '0') {
        start_time[0] = ' ';
    }
    if (end_time[0] == '0') {
        end_time[0] = ' ';
    }

    sprintf(buffer, "%s to %s: %s {{%s}}", start_time, end_time, node->val.summary, node->val.location);
    return buffer_ptr;
}
char * get_dash_string(char * string) {
    char buffer[MAX_LINE_LEN]="";
    char dash[MAX_LINE_LEN]="-";
    char* buffer_ptr =NULL;
    buffer_ptr = &buffer[0];
    int i;
    i=0;
    while(i < strlen(string)) {
        strcat(buffer, dash);   
        i++;
    }
    return buffer_ptr;
}
int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *file_name = NULL;
    int i; 

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            file_name = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || file_name == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    time_t from_date, to_date;

    from_date = get_datetime_from_ints(from_y, from_m, from_d);
    to_date = get_datetime_from_ints(to_y, to_m, to_d);

    node_t *events = NULL;

    events = explode_recurring_events(parse_events(read_file(file_name)));
    merge_sort(&events);
    pretty_print(events, from_date, to_date);

    exit(0);

}
