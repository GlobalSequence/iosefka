#ifndef TEST_H
#define TEST_H

#define TEST_PRINT_NL printf("\n");
#define TEST_PRINT_ONE(a) printf("%d", a);
#define TEST_PRINT_ONE_NL(a) printf("%d\n", a);
#define TEST_PRINT_ONE_TAB(a) printf("%d\t", a);
#define TEST_PRINT_TWO_NL(a,b) printf("%d\t%d\n", a, b);
#define TEST_PRINT_INDEX_ONE_NL(a,b) printf("%d: %d\n", a, b);
#define TEST_PRINT_INDEX_ONE_TAB(a,b) printf("%d: %d\t", a, b);
#define TEST_PRINT_TEXT(a) printf(a);

#define TEST_PRINT_MICROS_ELAPSED_START unsigned long t1 = time_us_32();
#define TEST_PRINT_MICROS_ELAPSED_END unsigned long t2 = time_us_32(); printf("%d\n", t2 - t1);

#define TEST_SEQ_STEP_1 {24, 255, 4, 0, 0};
#define TEST_SEQ_STEP_2 {22, 255, 0, 0, 0};
#define TEST_SEQ_STEP_3 {24, 255, 2, 0, 0};
#define TEST_SEQ_STEP_4 {36, 255, 1, 0, 0};
#define TEST_SEQ_STEP_5 {41, 255, 0, 0, 0};
#define TEST_SEQ_STEP_6 {24, 255, 1, 0, 0};
#define TEST_SEQ_STEP_7 {36, 255, 0, 0, 0};
#define TEST_SEQ_STEP_8 {24, 255, 3, 0, 0};

#define TEST_STEP_DATA_PATTERN_GEN_ON ({ step_data_t test_step[8]; \
    test_step[0] = (step_data_t) TEST_SEQ_STEP_1\
    test_step[1] = (step_data_t) TEST_SEQ_STEP_2\
    test_step[2] = (step_data_t) TEST_SEQ_STEP_3\
    test_step[3] = (step_data_t) TEST_SEQ_STEP_4\
    test_step[4] = (step_data_t) TEST_SEQ_STEP_5\
    test_step[5] = (step_data_t) TEST_SEQ_STEP_6\
    test_step[6] = (step_data_t) TEST_SEQ_STEP_7\
    test_step[7] = (step_data_t) TEST_SEQ_STEP_8\
    for(int i = 0; i < 8; i++) {\
        synth->seq.setStepData(i, test_step[i]); \
        synth->seq.setWrittenStep(i);\
        synth->seq.setActiveStep(i);\
    }\
    synth->seq.setBpm(120);\
    synth->pattern_gen.chance = 128;\
    synth->pattern_gen.density = 2;\
    synth->pattern_gen.octave_range = 2;\
    synth->pattern_gen.repeat = 1;\
    synth->pattern_gen.on = true;\
    synth->midi.channel = 11;\
});\

#endif