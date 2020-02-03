/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  run make
 *
 * Example Run:      ./climate data_tn.tdv data_wa.tdv
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: TN WA
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F on Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F on Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F on Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F on Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 *
 * TDV format:
 *
 * CA» 1428300000000»  9prcjqk3yc80»   93.0»   0.0»100.0»  0.0»95644.0»277.58716
 * CA» 1430308800000»  9prc9sgwvw80»   4.0»0.0»100.0»  0.0»99226.0»282.63037
 * CA» 1428559200000»  9prrremmdqxb»   61.0»   0.0»0.0»0.0»102112.0»   285.07513
 * CA» 1428192000000»  9prkzkcdypgz»   57.0»   0.0»100.0»  0.0»101765.0» 285.21332
 * CA» 1428170400000»  9prdd41tbzeb»   73.0»   0.0»22.0»   0.0»102074.0» 285.10425
 * CA» 1429768800000»  9pr60tz83r2p»   38.0»   0.0»0.0»0.0»101679.0»   283.9342
 * CA» 1428127200000»  9prj93myxe80»   98.0»   0.0»100.0»  0.0»102343.0» 285.75
 * CA» 1428408000000»  9pr49b49zs7z»   93.0»   0.0»100.0»  0.0»100645.0» 285.82413
 *
 * Each field is separated by a tab character \t\n and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* TODO: Add elements to the climate_info struct as necessary. */
struct climate_info {
    char code[3];
    unsigned int num_records;
    double avgHumidity;
    double temperature;
    double maxTemp;
    unsigned long long maxTempTimestamp;
    double minTemp;
    unsigned long long minTempTimestamp;
    int lightning;
    int snow;
    double cloud;
    char* geoLocation;
    double pressure;
};

void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int main(int argc, char *argv[]) {
    
    /* TODO: fix this conditional. You should be able to read multiple files. */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }
    
    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = { NULL };
    FILE *file;
    int i;
    for (i = 1; i < argc; ++i) {
        /* TODO: Open the file for reading */
        file = fopen(argv[i], "r");

        /* TODO: If the file doesn't exist, print an error message and move on to the next file. */
        if(file == NULL) {
            fprintf(stderr, "Could not open file #%d for reading.", i);
            continue;
        }

        /* TODO: Analyze the file */
        analyze_file(file, states, NUM_STATES);
        fclose(file);
    }

    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    return 0;
}

/**************************************************
*Function that counts the number of states found
**************************************************/
static int countStates(struct climate_info **states, int num_states)
{
    int num = 0;
    while(num < num_states && states[num] != NULL)
    {
        num++;
    }
    return num;
}

/**************************************************
*Function that checks for the order of the states
**************************************************/
static int compareOrder(struct climate_info **states, char codex[3], int currentStates)//returns the order of each state in the array
{
    int order = 0;
    while (order < currentStates)   //while order is less than number of states analyzed
    {
        if (states[order] == NULL)
        {
            break;
        }
        if(strcmp((states[order])->code, codex) == 0)       //if the states is present, returns its order
        {
            return order;
        }
        order++;    
    }
    return -1;        //returns -1 if checked all states and new state isn't present
}

/***********************
*Analyze file function
************************/
void analyze_file(FILE *file, struct climate_info **states, int num_states)
{
    const int line_sz = 100;
    char line[line_sz];
    int currentStates = countStates(states, num_states);
    

    
    while (fgets(line, line_sz, file) != NULL)
    {
        /* TODO: We need to do a few things here:
         *
         *       * Tokenize the line.
         *       * Determine what state the line is for. This will be the state
         *         code, stored as our first token.
         *       * If our states array doesn't have a climate_info entry for
         *         this state, then we need to allocate memory for it and put it
         *         in the next open place in the array. Otherwise, we reuse the
         *         existing entry.
         *       * Update the climate_info structure as necessary.
         */
        
        char *foundCode = strtok(line, "\t\n");// tokenizes the first token to state code
        int stateNumOrder = compareOrder(states, foundCode, currentStates);
        if (stateNumOrder == -1)// if the state is the first of its kind in the struct
        {
            if (currentStates >= num_states)
            {
                fprintf(stderr, "Error: too many states (There are already %d states, should not add more)\n",currentStates);
                exit(EXIT_FAILURE);
            }
            states[currentStates] = (struct climate_info *)calloc(sizeof(struct climate_info), 1);
            strcpy((states[currentStates])->code, foundCode);
            states[currentStates]->minTemp = -9999;
            states[currentStates]->maxTemp = 9999;
            stateNumOrder = currentStates++;
        }
        
       
        states[stateNumOrder]->num_records += 1;
        //Time Stamp
        char* currentTimeStamp = strtok(NULL, "\t\n");
        
        //skip GeoLocation
        char* currentGeol = strtok(NULL, "\t\n");
        states[stateNumOrder]->geoLocation += *currentGeol;
        //Average Humidity
        char* currentHumidity = strtok(NULL, "\t\n");
        states[stateNumOrder]->avgHumidity += atof(currentHumidity);
        //Snow Cover
        char* currentSnow = strtok(NULL, "\t\n");
        states[stateNumOrder]->snow += atoi(currentSnow);
        //Cloud Cover
        char* currentCloud = strtok(NULL, "\t\n");
        states[stateNumOrder]->cloud += atof(currentCloud);
        //Lightning Strikes
        char* currentLightning = strtok(NULL, "\t\n");
        states[stateNumOrder]->lightning += atoi(currentLightning);
        //skip Pressure
        char* currentPressure = strtok(NULL, "\t\n");
        states[stateNumOrder]->pressure += atof(currentPressure);
        //Temperature
        double temp = atof(strtok(NULL, "\t\n"));
        states[stateNumOrder]->temperature += temp;
        
        //Checking if the temperature is the minimum or maximum temp for that State
        if(temp < states[stateNumOrder]->minTemp)
        {
            states[stateNumOrder]->minTemp = temp;
            states[stateNumOrder]->minTempTimestamp = atoll(currentTimeStamp);
        }
        else if(temp > states[stateNumOrder]->maxTemp)
        {
            states[stateNumOrder]->maxTemp = temp;
            states[stateNumOrder]->maxTempTimestamp = atoll(currentTimeStamp);
        }
    }
}

void print_report(struct climate_info *states[], int num_states)
{
    printf("States found: ");
    int i;
    for (i = 0; i < num_states; ++i)
    {
        if (states[i] != NULL)
        {
            struct climate_info *info = states[i];
            printf("%s ", info->code);
        }
    }
    printf("\n");
    
    /* TODO: Print out the summary for each state. See format above. */
    
    for (i = 0; i < num_states && states[i] != NULL; i++)
    {
        const long MAX = (((*(states + i))->maxTempTimestamp) / 1000);//time conversion
        const long MIN = (((*(states + i))->minTempTimestamp) / 1000);
        double aTemp = ((states[i]->temperature) / (states[i]->num_records)) * 1.8 - 459.67;//temperature conversions
        double lTemp =  (states[i]->minTemp) * 1.8 - 459.67;
        double hTemp =  (states[i]->maxTemp) * 1.8 - 459.67;
        
        printf("-- State: %s --\n", states[i]->code);
        printf("Number of Records: %d\n", states[i]->num_records);
        printf("Average Humidity: %.1f%%\n", (states[i]->avgHumidity) / ((states[i]->num_records)));
        printf("Average Temperature: %.1fF\n", aTemp);
        printf("Max Temperature: %.1fF\n", hTemp);
        printf("Max Temperature on: %s", ctime(&MAX));
        printf("Min Temperature: %.1fF\n", lTemp);
        printf("Min Temperature on: %s", ctime(&MIN));
        printf("Lightning Strikes: %d\n", states[i]->lightning);
        printf("Records with Snow Cover: %d\n", states[i]->snow);
        printf("Average Cloud Cover: %.1lf%% \n", (states[i]->cloud / (states[i]->num_records)));
    }
}
