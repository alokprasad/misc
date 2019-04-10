#include<stdio.h>
#include<inttypes.h>
uint64_t Resample_s16(const int16_t *input, int16_t *output, int inSampleRate, int outSampleRate, uint64_t inputSize,
                      uint32_t channels
) {
    if (input == NULL)
        return 0;
    uint64_t outputSize = inputSize * outSampleRate / inSampleRate;
    if (output == NULL)
        return outputSize;
    double stepDist = ((double) inSampleRate / (double) outSampleRate);
    const uint64_t fixedFraction = (1LL << 32);
    const double normFixed = (1.0 / (1LL << 32));
    uint64_t step = ((uint64_t) (stepDist * fixedFraction + 0.5));
    uint64_t curOffset = 0;
    for (uint32_t i = 0; i < outputSize; i += 1) {
        for (uint32_t c = 0; c < channels; c += 1) {
            *output++ = (int16_t) (input[c] + (input[c + channels] - input[c]) * (
                    (double) (curOffset >> 32) + ((curOffset & (fixedFraction - 1)) * normFixed)
            )
            );
        }
        curOffset += step;
        input += (curOffset >> 32) * channels;
        curOffset &= (fixedFraction - 1);
    }
    return outputSize;
}

main()
{

		int FRAME_SIZE = 160;
		short x[FRAME_SIZE];
		short y[FRAME_SIZE];

		/* upsample 16khz pcm file to 48khz pcm file)*/
		FILE *fin = fopen("16khz_voice.pcm","rb");
		FILE *fout = fopen("48khz_voice.pcm", "wb");	

		while(!feof(fin))
		{
				fread(x, sizeof(short), FRAME_SIZE, fin);
				Resample_s16(x,y,16000,48000,2,1);
				fwrite(x, sizeof(short), FRAME_SIZE, fout);
		}

		fclose(fin);	
		fclose(fout);
}
