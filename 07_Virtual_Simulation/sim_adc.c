#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ADC_RESOLUTION 1024  // 10-bit resolution for the ADC
#define ADC_CHANNELS 16      // Number of available ADC channels

typedef struct {
    int currentValue;
} VirtualADC;

VirtualADC adc[ADC_CHANNELS];

void initADC() {
    srand(time(NULL));  // Seed the random number generator
    for(int i = 0; i < ADC_CHANNELS; i++) {
        adc[i].currentValue = 0;
    }
}

int readADC(int channel) {
    if (channel < 0 || channel >= ADC_CHANNELS) {
        printf("Error: Invalid ADC channel\n");
        return -1;
    }
    adc[channel].currentValue = rand() % ADC_RESOLUTION;  // Simulate ADC reading
    return adc[channel].currentValue;
}

int main() {
    initADC();
    for (int i = 0; i < ADC_CHANNELS; i++) {
        int value = readADC(i);
        printf("ADC Channel %d Reading: %d\n", i, value);
    }
    return 0;
}