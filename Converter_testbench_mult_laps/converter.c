#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

void initialSpace(FILE** fp, float timestamp)
{
    for (size_t i = 0; i < 8u; i++)
    {
        if(timestamp+1 < pow(10, i))
        {
            for (size_t j = 0; j < (8u-i); i++)
            {
                fprintf(*fp, " ");        
            }
        }
    }
}

char bitsToChar(uint8_t bits)
{
    if(bits < 10u)
        return '0' + bits;
    else if(bits > 9u && bits < 16u)
        return 'A' + (bits-10u);
    else 
        return '0';
}

/* TODO
void writeNewLine(FILE** fp_out, float timestamp, uint8_t TPS_100)
{

}
*/

int main()
{
    float TPS_factor = 100.f;
    uint8_t TPS_100_u;
    size_t lap_number = 1;
    float total_time = 0.f;
    float time_input = 0.f;
    float last_time_input = 0.f;

    float    timestamp = 0.f;
    float    TPS_100 = 0.f;
    char     half_bytes[16];

    char fp_input[100] = "lap.txt";
    char fp_output[100] = "lap.asc";
    
    FILE* fp_txt = fopen(fp_input, "r");
    FILE* fp_asc = fopen(fp_output, "w");

    // check for error during file opening 
    if(fp_txt == NULL)
    {
        perror("Error opening input file");
        return 1;
    }
    if(fp_asc == NULL)
    {
        perror("Error opening output file");
        return 1;
    }
    fprintf(fp_asc, "%s", "date Tue Jul 11 03:21:50.997 pm 2023\nbase hex  timestamps absolute\ninternal events logged\n// version 16.4.0\n// Measurement UUID: e50b40cf-ee7d-4b3a-8b66-51c4a617e02d\nBegin TriggerBlock Tue Jul 11 03:21:50.997 pm 2023");

    /* --- USER INPUT ---*/

    printf("\n ~~~~~ .txt -> .asc Converter ~~~~~");

    printf("\n\n - Insert TPS limitation[0%% - 100%%]: \n > ");
    scanf("%f", &TPS_factor);

    printf("\n - Insert number of laps to be made: \n > ");
    scanf("%u", &lap_number);

    printf("\n - LOADING...");

    for (size_t lap = 0; lap < lap_number; lap++)
    {
        // reset file pointer to the beginning of the file
        rewind(fp_txt);


        while(!feof(fp_txt))
        {
            // SETUP
            for (size_t i = 0; i < 16; i++)
            {
                half_bytes[i] = '0';
            }

            /*------ READ .txt ------*/

            fscanf(fp_txt, "%f %f", &time_input, &TPS_100);
            
            // write new line only if timestamps differ
            if (time_input != last_time_input) 
            {

                timestamp = total_time + time_input;
                last_time_input = time_input;

                // TPS scaling
                TPS_100 = TPS_100 * (float)TPS_factor / 100.f;
                TPS_100_u = (uint8_t)TPS_100;

                /*------ ELABORATION ------*/

                // ROLLOVERCOUNTER BYTE x1 (7, 6)
                half_bytes[14] = bitsToChar(TPS_100_u & 0b1111);
                half_bytes[15] = bitsToChar((uint8_t)((TPS_100_u & 0b11110000) >> 4u));

                /* ------ WRITE ON .asc ----------- */

                fprintf(fp_asc, "\n");
                initialSpace(&fp_asc, timestamp);
                // TIMESTAMP
                fprintf(fp_asc, "%f", timestamp);
                // CAN BUS ID (1 or 2)
                fprintf(fp_asc, "\t2");
                // CAN ID (ex: 192873x)
                fprintf(fp_asc, "\t1300x");
                // Tx
                fprintf(fp_asc, "\tTx");
                // d
                fprintf(fp_asc, "\td");
                // DLC
                fprintf(fp_asc, " 8");
                // tps testbench
                fprintf(fp_asc, " %c%c ", half_bytes[15], half_bytes[14]);
                fprintf(fp_asc, "00 00 00 00 00 00 00");
            }
        }

        total_time += time_input;
        printf("\n %u) Lap time: \t%f", lap, time_input);
        printf("\n    Lap end:\t%f", total_time);
        
    }

    fprintf(fp_asc,"\nEnd TriggerBlock");

    fclose(fp_asc);
    fclose(fp_txt);

    printf("\n\n - DONE");
    printf("\n - Output file: '%s'", fp_output);
    printf("\n");

    return 0;
}