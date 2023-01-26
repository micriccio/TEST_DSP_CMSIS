#include "mbed.h"
/* Include arm_math.h mathematic functions */
#include "arm_math.h"
/* Include mbed-dsp libraries */
#include "arm_common_tables.h"
#include "arm_const_structs.h"
#include "math_helper.h"

/* FFT settings */
// Allows 16, 32, 64, 128, 256, 512, 1024, 2048 or 4096 samples for FFT size
#define SAMPLES                 4096             /* 2048 real party and 2048 imaginary parts */
#define FFT_SIZE                2048     /* FFT size is always the same size as we have samples, so 2048 in our case */

/* Global variables */
float Input[SAMPLES];
float val;
float Output[FFT_SIZE];
volatile bool trig=false;
const static arm_cfft_instance_f32 *S;
/* MBED class APIs */
AnalogIn   myADC(A0);
PwmOut sig(D9); // if shorted to A0 is used to verify the program (FFT of a square wave)

BufferedSerial pc(USBTX, USBRX, 115200);
Ticker timer;

//uint16_t frame_start = 0xBBAA;
//uint16_t* ptr1;

void sample()
{
    trig=true;
}

int main()
{
    sig.period_ms(30);      // 30m second period
    sig.write(0.50f);      // 50% duty cycle, relative to period
    int i=0;
    //float maxValue;            // Max FFT value is stored here
    //uint32_t maxIndex;         // Index in Output array where max value is
    //pc.printf("Starting FFT\r\n");
    timer.attach(&sample,1ms); //1ms 1KHz sampling rate
    // Init arm_ccft_32
    switch (FFT_SIZE) {
        case 16:
            S = & arm_cfft_sR_f32_len16;
            break;
        case 32:
            S = & arm_cfft_sR_f32_len32;
            break;
        case 64:
            S = & arm_cfft_sR_f32_len64;
            break;
        case 128:
            S = & arm_cfft_sR_f32_len128;
            break;
        case 256:
            S = & arm_cfft_sR_f32_len256;
            break;
        case 512:
            S = & arm_cfft_sR_f32_len512;
            break;
        case 1024:
            S = & arm_cfft_sR_f32_len1024;
            break;
        case 2048:
            S = & arm_cfft_sR_f32_len2048;
            break;
        case 4096:
            S = & arm_cfft_sR_f32_len4096;
            break;
    }
    while(1) {
        //ciao
        if (trig) {
            Input[i] = myADC.read() - 0.5f; //Real part NB removing DC offset
            Input[i + 1] = 0;               //Imaginary Part set to zero
            trig = false;
            i=i+2;
            if (i==SAMPLES) {
                // Init the Complex FFT module, intFlag = 0, doBitReverse = 1
                //NB using predefined arm_cfft_sR_f32_lenXXX, in this case XXX is 256
                arm_cfft_f32(S, Input, 0, 1);
                // Complex Magniture Module put results into Output(Half size of the Input)
                arm_cmplx_mag_f32(Input, Output, FFT_SIZE);
                //Calculates maxValue and returns corresponding value
                //arm_max_f32(Output, FFT_SIZE, &maxValue, &maxIndex);
                //pc.printf("Maximum is %f\r\n",maxValue);
                for (int j=0; j<FFT_SIZE/2; j++) {
                    //pc.write(ptr1,2);
                    val = Output[j];
                    pc.write(&val,4);
                    ThisThread::sleep_for(1ms);
                }
                return 0;
            }
        }
    }
}


