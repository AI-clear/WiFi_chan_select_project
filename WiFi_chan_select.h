#ifndef WIFI_CHAN_SELECT_H_
#define WIFI_CHAN_SELECT_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <float.h>

typedef struct {
    double* maxima;
    int* indices;
    //int min_count;
    int max_count;
} MaximaResult;

typedef struct {
    double* minima;
    int* indices;
    int min_count;
} MinimaResult;

struct AP_info {
    unsigned char Mac_address[6];
    char SSID[32];
    int chan_number;
    int Bandwidth;
    int RSSI;
};

struct channel_info {
    int channelnumber;
    double accum_RSSI_mW;
    double instant_RSSI_mW;
    double SINR;
    int accum_RSSI_dBm;
};

struct AFC_info{
   int chan_number;
   int Max_TX_PWR;
};

void debugging(struct AP_info ap_list[], int size){
   int excluded_numbers[] = {31, 63, 95, 127, 159, 191, 223, 231};
   int is_excluded = 0;

   for (int i = 0;i < size;i++){
      if (ap_list[i].chan_number % 2 == 0){
         printf("chan number has even number.\n");
      }
      for (int j = 0; j < sizeof(excluded_numbers) / sizeof(excluded_numbers[0]); j++) {
         if (ap_list[i].chan_number == excluded_numbers[j]){
            is_excluded = 1;
            printf("Invalid chan_number value.\n");
            break;}
      }
      int max_c = 0;

      for (int j = 2; j <= 5; j++) {
            double den = pow(2, j);
            int c = ap_list[i].chan_number % (int) den;
            if (c == 1 || c == 3 || c == 7 || c == 15) {
              if (c > max_c) {
                 max_c = c;
              }
            }
        }

     switch(max_c){
         int c_err = (max_c + 1)*10;
         if (ap_list[i].Bandwidth != c_err) {
             printf("Error: Bandwidth does not match expected value.\n");
             break; // switch 문을 빠져나감
         }

     }
   }
}


void initialize_AFC_info(struct AFC_info AFC_info[], int BW) {
   int ch_num = 59/(BW/20);
    int x = 0;
    for (int i = 0; i < ch_num ; i++) {
       int max_tx_pwr_options[] = {21};//, 24, 27};
        int random_index = rand() % sizeof(max_tx_pwr_options);
        AFC_info[i].Max_TX_PWR = 21;
        /*if (i == 1 || i == 5){
            AFC_info[i].Max_TX_PWR = 21; // max_tx_pwr_options[random_index];
        }
        else{
            AFC_info[i].Max_TX_PWR = 18;
        }*/
        x= (BW/20);
        AFC_info[i].chan_number = (2*x-1+4*x*i);
    }
}

//-----------------Beginning of  Self-test part ----------------------//
struct AP_info *generateAPlist(int numAPs){
    int num_channels = 0;
    int lower = -63, upper = -20;
    int num_range = upper - lower + 1;

    struct AP_info *ap_list = (struct AP_info *)malloc(numAPs * sizeof(struct AP_info));
       if (!ap_list) {
           perror("Memory allocation failed");
           return NULL;
       }
       for(int i = 0;i < numAPs; i++){
          ap_list[i].chan_number = 0;
          ap_list[i].Bandwidth = 0;
          ap_list[i].RSSI = 0;
       }


       struct AP_info ch_list[109];  // AP_info 구조체 배열
       int excluded_numbers[] = {31, 63, 95, 127, 159, 191, 223, 231};
       int index = 0;  // ch_list 배열의 인덱스

       for (int i = 1; i <= 233; i += 2) {
       int is_excluded = 0;  // 제외할 숫자인지 확인하는 플래그

               for (int j = 0; j < sizeof(excluded_numbers) / sizeof(excluded_numbers[0]); j++) {
                   if (i == excluded_numbers[j]) {
                       is_excluded = 1;
                       break;
                   }
               }

               if (!is_excluded) {
                   ch_list[index].chan_number = i;
                   index++;

                   if (index == 109) {
                       break;  // 109개의 숫자를 다 넣었으면 루프 종료
                   }
               }
           }


    for (int i = 0; i < numAPs; i++) {
    	for (int j = 0; j < 6; j++) {
    		ap_list[i].Mac_address[j] = rand() % 0xFF; // Generate a random value between 0 and 255
    	}
        char ssid_rand[32];
        snprintf(ssid_rand, sizeof(ssid_rand), "studentroom%d", i + 1);
        strcpy(ap_list[i].SSID, ssid_rand);
        int choose_rnd = rand() % 109;
        ap_list[i].chan_number= ch_list[choose_rnd].chan_number;

        int max_c = 0;

      for (int j = 2; j <= 5; j++) {
           double den = pow(2, j);
           int c = ap_list[i].chan_number % (int) den;
           if (c == 1 || c == 3 || c == 7 || c == 15) {
              if (c > max_c) {
                  max_c = c;
              }
           }
      }

      num_channels = (max_c + 1)/2;
      ap_list[i].Bandwidth = (int) num_channels*20;

      ap_list[i].RSSI = (rand() % num_range) + lower;

  }
    return ap_list;
}

//------------------- End of Self-test part --------------------------//


double dBmTomW(double dbm) { // Change dBm to mw
    return pow(10, dbm / 10)*0.001;
}

void initialize_channel_info(struct channel_info channels[], int size) {
    for (int i = 0; i < size; i++) {
        channels[i].accum_RSSI_mW = 0;
        channels[i].instant_RSSI_mW = 0;
        channels[i].accum_RSSI_dBm = 0;
        channels[i].channelnumber = 1 + 4 * i;
    }
}


void process_rssi(struct AP_info ap_list[], struct channel_info channels[], int N) {

   //The input N is the number of beacon message of N different APs.

    for (int i = 0; i < N; i++) {
        int chan_number = ap_list[i].chan_number;
        int max_c = 0;

        for (int j = 2; j <= 5; j++) {
            double den = pow(2, j);
            int c = chan_number % (int) den;

            if (c == 1 || c == 3 || c == 7 || c == 15) {
                if (c > max_c) {
                    max_c = c;
                }
            }
        }

        int num_channels = 0;
        int start_channel = 0;

        num_channels = (max_c + 1)/2;
        start_channel = chan_number - (max_c-1);

        double rssi_mW = dBmTomW(ap_list[i].RSSI) / num_channels;

        int channel_index = (start_channel - 1) / 4;

        for (int k = 0; k < num_channels; k++) {
            channels[channel_index + k].instant_RSSI_mW = rssi_mW;
            channels[channel_index + k].accum_RSSI_mW =  channels[channel_index + k].accum_RSSI_mW  + channels[channel_index + k].instant_RSSI_mW;
        }
    }


}

void take_average(struct channel_info channels[], int num_M){
   int total_20_ch_num = 59;

   for (int i = 0; i<total_20_ch_num ;i++){
      if (channels[i].accum_RSSI_mW != 0){
    	  channels[i].accum_RSSI_mW = channels[i].accum_RSSI_mW/num_M;
      	  channels[i].accum_RSSI_dBm = (int) 10*log10(channels[i].accum_RSSI_mW/0.001);
      	  printf("%.15lf\n",channels[i].accum_RSSI_mW);
      }
      else{
    	  channels[i].accum_RSSI_mW = 0;
    	  channels[i].accum_RSSI_dBm = - 174;
      	}
   }
}


MaximaResult find_channels_MAX_SINR(int bandwidth, struct channel_info channels[], struct AFC_info AFC_info[], int G) {
    double tmp_sum = 0;
    MaximaResult result_max;

    double max_value = 0;

    double white_noise = pow(10,-12);

    result_max.max_count = 0;

    int j_end = 0;
    int p_st = 0;



    j_end = bandwidth/20;
    p_st = 2*j_end - 1;
    G = G/j_end;

    struct channel_info channels_tmp[G];

    for (int i = 0; i < G; i++) {
        tmp_sum = 0; // Reset tmp_sum for each channel

        for (int j = 0; j < j_end; j++) {
            tmp_sum += channels[i * j_end + j].accum_RSSI_mW;
        }
        channels_tmp[i].accum_RSSI_mW = tmp_sum + white_noise;
        channels_tmp[i].SINR = log2(1+dBmTomW(AFC_info[i].Max_TX_PWR)/channels_tmp[i].accum_RSSI_mW);

    }

   for (int i = 0; i < G; i++) {
        if (channels_tmp[i].SINR > max_value) {
              max_value = channels_tmp[i].SINR;
              result_max.max_count = 1;
        }
        else if (channels_tmp[i].SINR == max_value) {
               result_max.max_count++;
        }
   }
    printf("%d",result_max.max_count);

    // Allocate memory for the minima and indices
    result_max.maxima = (double*)malloc(sizeof(double) * result_max.max_count);
    result_max.indices = (int*)malloc(sizeof(int) * result_max.max_count);

    int j = 0;
    for(int i = 0; i < G; i++) {
        if(channels_tmp[i].SINR == max_value) {
            result_max.maxima[j] = channels_tmp[i].SINR;
             result_max.indices[j] = p_st + j_end*4*i;
           j++;
       }
    }
    return result_max;
}

MinimaResult find_channels_min_RSSI(int bandwidth, struct channel_info channels[], int G) {
    double tmp_sum = 0;
    MinimaResult result;
    double min_value = DBL_MAX;
    result.min_count = 0;

    int j_end = 0;
    int p_st = 0;

    struct channel_info channels_tmp[G];

    // G is the total number of 20 MHz channels: it is always 59.

    j_end = bandwidth/20;
    p_st = 2*j_end - 1;
    G = G/j_end;


    for (int i = 0; i < G; i++) {
        tmp_sum = 0; // Reset tmp_sum for each channel

        for (int j = 0; j < j_end; j++) {
                tmp_sum += channels[i * j_end + j].accum_RSSI_mW;
        }
        channels_tmp[i].accum_RSSI_mW = tmp_sum;
    }

    for (int i = 0; i < G; i++) {
        if (channels_tmp[i].accum_RSSI_mW < min_value) {
           min_value = channels_tmp[i].accum_RSSI_mW;
           result.min_count = 1;
        }
        else if (channels_tmp[i].accum_RSSI_mW == min_value) {
           result.min_count++;
        }
    }
    // Allocate memory for the minima and indices
    result.minima = (double*)malloc(sizeof(double) * result.min_count);
    result.indices = (int*)malloc(sizeof(int) * result.min_count);

    int j = 0;
    for(int i = 0; i < G; i++) {
        if(channels_tmp[i].accum_RSSI_mW == min_value) {
            result.minima[j] = channels_tmp[i].accum_RSSI_mW;
            result.indices[j] = p_st + j_end*4*i;
            j++;
        }
    }
    return result;
}


int allocateRandomToChannel(int count) {

   if (count <= 0) {
        printf("Invalid count value.\n");
        return -1;
    }

    int randomValue = rand() % count; // Generate a random value between 0 and min_count - 1
    return randomValue;
}

void plot_with_Matlab(struct channel_info channels[], struct AFC_info AFC_info[], int total_20_ch_num, FILE *file){

   for (int i=0;i<total_20_ch_num ; i++){
      if (channels[i].accum_RSSI_mW != 0){
    	  //fprintf(file,"%d %.10lf\n",channels[i].channelnumber, channels[i].accum_RSSI_mW);}
          //fprintf(file,"%d %f\n",channels[i].channelnumber, 10*log10(channels[i].accum_RSSI_mW/0.001));
    	  fprintf(file,"%d %f\n", channels[i].channelnumber,log2(1+dBmTomW(AFC_info[i].Max_TX_PWR)/channels[i].accum_RSSI_mW));
    	  //fprintf(file,"%d %d\n",channels[i].channelnumber, channels[i].accum_RSSI_dBm);
          }
      else
      {
          fprintf(file,"%d %d\n",channels[i].channelnumber,-174);
      }

   }

}

#endif /* WIFI_CHAN_SELECT_H_ */
