#include <WiFi_chan_select.h>

int main(void) {
    srand(time(NULL)); // 랜덤 시드 초기화
    int Bandwidth_to_use = 160;
    int numAPs = 25;
    int total_20_ch_num = 59;

    struct channel_info channels[total_20_ch_num];
    initialize_channel_info(channels, total_20_ch_num);

    struct AFC_info AFC_info[total_20_ch_num];
    initialize_AFC_info(AFC_info, 20);


//-----------------Beginning of  Self-test part ----------------------//
        struct AP_info *ap_list = generateAPlist(numAPs);
//        debugging(ap_list, numAPs);
        process_rssi(ap_list, channels, numAPs);
//------------------- End of Self-test part --------------------------//

      // take_average(channels, num_M);

     /* FILE *file = fopen("output.txt", "w");

      if (file == NULL) {
         printf("Error opening the file.\n");
         return 1;
      }

       plot_with_Matlab(channels,AFC_info,total_20_ch_num, file);

       fclose(file);
      */
       //Print the accumulated RSSI values for each channel
       /*for (int i = 0; i < total_20_ch_num; i++) {
           printf("Channel %d: %.31f mW\n", channels[i].channelnumber, channels[i].accum_RSSI_mW);
       }*/
       /*
      MinimaResult result= find_channels_min_RSSI(20, channels, total_20_ch_num);

       printf("Minimum Value(s):\n");
       for(int i = 0; i < result.min_count; i++) {
           printf("%f at index %d\n", result.minima[i], result.indices[i]);
       }



       int ch_allc = allocateRandomToChannel(result.min_count);

       printf("Allocated channel: %d\n", result.indices[ch_allc]);

       // Clean up allocated memory
       free(result.minima);
       free(result.indices);
      */
//---------------------------------------------------MaximaResult ------------------------------------------------//
       MaximaResult result_max= find_channels_MAX_SINR(Bandwidth_to_use, channels, AFC_info, total_20_ch_num);

       printf("Maximum Value(s):\n");
       for(int i = 0; i < result_max.max_count; i++) {
           printf("%f at index %d\n", result_max.maxima[i], result_max.indices[i]);
       }



       int ch_allc = allocateRandomToChannel(result_max.max_count);

       printf("Allocated channel: %d\n", result_max.indices[ch_allc]);

       // Clean up allocated memory
       free(result_max.maxima);
       free(result_max.indices);

        return 0;
   }
