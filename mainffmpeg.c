#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h> 
#include <windows.h>
#include <math.h>


#define MAX_CMD_SIZE 1024


char* create_ffmpeg_command(
    const char* input, 
    const char* output, 
    int semitones, 
    float duration, 
    int freq_vibro, 
    float depth_vibro, 
    float start_fade_in, 
    float duration_fade_in, 
    float start_fade_out, 
    float duration_fade_out,

    float Echo1,
    float Echo2,
    float Echo3,
    float Echo4,
    float chorus,
    float Equalizerf,
    float Equalizert,
    float Equalizerw,
    float Equalizerg,

    float Flanger
) {
    static char cmd[MAX_CMD_SIZE];

   
    double factor = pow(2.0, semitones / 12.0);

    snprintf(cmd, MAX_CMD_SIZE, 
        "ffmpeg -y -i \"%s\" ", input);


    snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
        "-af \"asetrate=44100*%.5f,atempo=1/%.5f", factor, factor);


    if (freq_vibro > 0 && depth_vibro > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
            ",vibrato=f=%d:d=%.2f", freq_vibro, depth_vibro);
    }

 
    if (start_fade_in >= 0 && duration_fade_in > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
            ",afade=t=in:st=%.2f:d=%.2f", start_fade_in, duration_fade_in);
    }


    if (start_fade_out >= 0 && duration_fade_out > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
            ",afade=t=out:st=%.2f:d=%.2f", start_fade_out, duration_fade_out);
    }

    if (Echo1 > 0 && Echo2 > 0 && Echo3 > 0 && Echo4 > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",aecho=%.2f:%.2f:%.2f:%.2f", Echo1, Echo2, Echo3, Echo4);
    }


    if (chorus > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",chorus=%.2f:0.7:60:0.4:0.25:2", chorus);
    }


    if (Equalizerf != 0 && Equalizert != 0 && Equalizerw != 0 && Equalizerg != 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",equalizer=f=%.2f:t=q:w=%.2f:g=%.2f",
            Equalizerf, Equalizert, Equalizerw, Equalizerg);
    }

 
    if (Flanger > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",flanger=delay=%.2f", Flanger);
    }



    snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
        ",aresample=44100\" ");

    snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
        "-t %.2f \"%s\"", duration, output);

    return cmd;
}


void runModifiers(
    const char* input, 
    const char* output, 
    int pitch, 
    float duration, 
    int freq_vibro, 
    float depth_vibro, 
    float start_fade_in, 
    float duration_fade_in, 
    float start_fade_out, 
    float duration_fade_out,

    float Echo1,
    float Echo2,
    float Echo3,
    float Echo4,

    float chorus,
    float Equalizerf,
    float Equalizert,
    float Equalizerw,
    float Equalizerg,

    float Flanger
) {
    char cmd[MAX_CMD_SIZE];

    strcpy(cmd, create_ffmpeg_command(input, output, pitch, duration, freq_vibro, depth_vibro, start_fade_in, duration_fade_in, start_fade_out, duration_fade_out, Echo1, Echo2, Echo3, Echo4, chorus, Equalizerf, Equalizert, Equalizerw, Equalizerg, Flanger));

 
    printf("Processing command:\n%s\n", cmd);
    system(cmd);
}


void merge_wav_files(
    const char** wavFiles, 
    int* pitches, 
    float* durations, 
    int* frequencies, 
    float* depths, 
    float* starts_fade_in, 
    float* durations_fade_in, 
    float* starts_fade_out, 
    float* durations_fade_out, 

    float* Echo1,
    float* Echo2,
    float* Echo3,
    float* Echo4,
    float* chorus,
    float* Equalizerf,
    float* Equalizert,
    float* Equalizerw,
    float* Equalizerg,

    float* Flanger,



    int fileCount, 
    const char* output
) {
    if (fileCount == 0) {
        printf("No files to merge.\n");
        return;
    }

    char** uniqueTmpFiles = malloc(fileCount * sizeof(char*));


    for (int i = 0; i < fileCount; i++) {
        char tmpFileName[128];
        snprintf(tmpFileName, sizeof(tmpFileName), "temp_modifier_%d_%s.wav", i, wavFiles[i]);
        uniqueTmpFiles[i] = strdup(tmpFileName);

        runModifiers(
            wavFiles[i], 
            uniqueTmpFiles[i], 
            pitches[i], 
            durations[i], 
            frequencies[i], 
            depths[i], 
            starts_fade_in[i], 
            durations_fade_in[i], 
            starts_fade_out[i], 
            durations_fade_out[i],
            Echo1[i], 
            Echo2[i], 
            Echo3[i], 
            Echo4[i], 
            chorus[i], 
            Equalizerf[i], 
            Equalizert[i], 
            Equalizerw[i], 
            Equalizerg[i], 
            Flanger[i]
  
        );
    }

    FILE* listFile = fopen("file_list.txt", "w");
    if (!listFile) {
        printf("Error creating list file\n");
        goto cleanup;
    }

    for (int i = 0; i < fileCount; i++) {
        fprintf(listFile, "file '%s'\n", uniqueTmpFiles[i]);
    }
    fclose(listFile);

    char mergeCmd[MAX_CMD_SIZE];
    snprintf(mergeCmd, sizeof(mergeCmd), "ffmpeg -y -f concat -safe 0 -i file_list.txt -c copy \"%s\"", output);
    printf("Final merge command:\n%s\n", mergeCmd);
    system(mergeCmd);

cleanup:
    remove("file_list.txt");
    for (int i = 0; i < fileCount; i++) {
        remove(uniqueTmpFiles[i]);
        free(uniqueTmpFiles[i]);
    }
    free(uniqueTmpFiles);

    printf("Files processed successfully into %s\n", output);
}

// Главная функция
int main() {
    char **fileNames = NULL;
    int *pitches = NULL;
    float *durations = NULL;
    int *frequencies = NULL;  
    float *depths = NULL;      
    float *starts_fade_in = NULL; 
    float *durations_fade_in = NULL; 
    float *starts_fade_out = NULL; 
    float *durations_fade_out = NULL; 
    float* Echo1 = NULL;
    float* Echo2 = NULL;
    float* Echo3 = NULL;
    float* Echo4 = NULL;
    float* chorus = NULL;
    float* Equalizerf = NULL; 
    float* Equalizert = NULL;
    float* Equalizerw = NULL;
    float* Equalizerg = NULL;

    float* Flanger = NULL;



    int fileCount = 0;

    FILE *inputFile = fopen("output.txt", "r");
    if (!inputFile) {
        printf("Cannot open input.txt\n");
        return 1;
    }

    _chdir("voicebank"); 

    char filenameLine[256];
    char pitchLine[256];
    char durationLine[256];
    char frequencyLine[256];
    char depthLine[256];     
    char startFadeInLine[256]; 
    char durationFadeInLine[256]; 
    char startFadeOutLine[256]; 
    char durationFadeOutLine[256]; 

    char Echo1Line[256];
    char Echo2Line[256];
    char Echo3Line[256];
    char Echo4Line[256];
    char chorusLine[256];
    char EqualizerfLine[256];
    char EqualizertLine[256];
    char EqualizerwLine[256];
    char EqualizergLine[256];

    char FlangeLine[256];




    while (fgets(filenameLine, sizeof(filenameLine), inputFile)) {
     
        filenameLine[strcspn(filenameLine, "\r\n")] = 0;
        
        if (strlen(filenameLine) == 0) {
            continue;
        }

        if (!fgets(pitchLine, sizeof(pitchLine), inputFile)) break;
        pitchLine[strcspn(pitchLine, "\r\n")] = 0;

        if (!fgets(durationLine, sizeof(durationLine), inputFile)) break;
        durationLine[strcspn(durationLine, "\r\n")] = 0;

        if (!fgets(frequencyLine, sizeof(frequencyLine), inputFile)) break;
        frequencyLine[strcspn(frequencyLine, "\r\n")] = 0;

        if (!fgets(depthLine, sizeof(depthLine), inputFile)) break;
        depthLine[strcspn(depthLine, "\r\n")] = 0;

        if (!fgets(startFadeInLine, sizeof(startFadeInLine), inputFile)) break;
        startFadeInLine[strcspn(startFadeInLine, "\r\n")] = 0;

        if (!fgets(durationFadeInLine, sizeof(durationFadeInLine), inputFile)) break;
        durationFadeInLine[strcspn(durationFadeInLine, "\r\n")] = 0;

        if (!fgets(startFadeOutLine, sizeof(startFadeOutLine), inputFile)) break;
        startFadeOutLine[strcspn(startFadeOutLine, "\r\n")] = 0;

        if (!fgets(durationFadeOutLine, sizeof(durationFadeOutLine), inputFile)) break;
        durationFadeOutLine[strcspn(durationFadeOutLine, "\r\n")] = 0;

        if (!fgets(Echo1Line, sizeof(Echo1Line), inputFile)) break;
        Echo1Line[strcspn(Echo1Line, "\r\n")] = 0;

        if (!fgets(Echo2Line, sizeof(Echo2Line), inputFile)) break;
        Echo2Line[strcspn(Echo2Line, "\r\n")] = 0;

        if (!fgets(Echo3Line, sizeof(Echo3Line), inputFile)) break;
        Echo3Line[strcspn(Echo3Line, "\r\n")] = 0;

        if (!fgets(Echo4Line, sizeof(Echo4Line), inputFile)) break;
        Echo4Line[strcspn(Echo4Line, "\r\n")] = 0;

        if (!fgets(chorusLine, sizeof(chorusLine), inputFile)) break;
        chorusLine[strcspn(chorusLine, "\r\n")] = 0;

        if (!fgets(EqualizerfLine, sizeof(EqualizerfLine), inputFile)) break;
        EqualizerfLine[strcspn(EqualizerfLine, "\r\n")] = 0;

        if (!fgets(EqualizertLine, sizeof(EqualizertLine), inputFile)) break;
        EqualizertLine[strcspn(EqualizertLine, "\r\n")] = 0;

        if (!fgets(EqualizerwLine, sizeof(EqualizerwLine), inputFile)) break;
        EqualizerwLine[strcspn(EqualizerwLine, "\r\n")] = 0;

        if (!fgets(EqualizergLine, sizeof(EqualizergLine), inputFile)) break;
        EqualizergLine[strcspn(EqualizergLine, "\r\n")] = 0;

        if (!fgets(FlangeLine, sizeof(FlangeLine), inputFile)) break;
        FlangeLine[strcspn(FlangeLine, "\r\n")] = 0;




        int pitch = atoi(pitchLine);
        float duration = atof(durationLine);
        int freq_vibro = atoi(frequencyLine); 
        float depth_vibro = atof(depthLine);        
        float start_fade_in = atof(startFadeInLine); 
        float duration_fade_in = atof(durationFadeInLine);
        float start_fade_out = atof(startFadeOutLine); 
        float duration_fade_out = atof(durationFadeOutLine); 

        float echo1_val = atof(Echo1Line); // Первый параметр Echo
        float echo2_val = atof(Echo2Line); // Второй параметр Echo
        float echo3_val = atof(Echo3Line); // Третий параметр Echo
        float echo4_val = atof(Echo4Line); // Четвертый параметр Echo
        float chorus_val = atof(chorusLine); // Параметр Chorus
        float equalizerf = atof(EqualizerfLine); // Частота эквалайзера
        float equalizert = atof(EqualizertLine); // Тип эквалайзера
        float equalizerw = atof(EqualizerwLine); // Ширина эквалайзера
        float equalizerg = atof(EqualizergLine); // Гейн эквалайзера

        float flange_val = atof(FlangeLine); // Параметр Flanger



        // Ограничиваем диапазон тональности
        if (pitch < -36 || pitch > 36) {
            pitch = 0;
        }

        // Добавляем новый файл и параметры
        fileNames = realloc(fileNames, (fileCount + 1) * sizeof(char*));
        pitches = realloc(pitches, (fileCount + 1) * sizeof(int));
        durations = realloc(durations, (fileCount + 1) * sizeof(float));
        frequencies = realloc(frequencies, (fileCount + 1) * sizeof(int)); // Частоты вибрато
        depths = realloc(depths, (fileCount + 1) * sizeof(float));         // Глубины вибрато
        starts_fade_in = realloc(starts_fade_in, (fileCount + 1) * sizeof(float)); // Times of fade-in
        durations_fade_in = realloc(durations_fade_in, (fileCount + 1) * sizeof(float)); // Durations of fade-in
        starts_fade_out = realloc(starts_fade_out, (fileCount + 1) * sizeof(float)); // Times of fade-out
        durations_fade_out = realloc(durations_fade_out, (fileCount + 1) * sizeof(float)); // Durations of fade-out

        Echo1 = realloc(Echo1, (fileCount + 1) * sizeof(float));          // Echo 1
        Echo2 = realloc(Echo2, (fileCount + 1) * sizeof(float));          // Echo 2
        Echo3 = realloc(Echo3, (fileCount + 1) * sizeof(float));          // Echo 3
        Echo4 = realloc(Echo4, (fileCount + 1) * sizeof(float));          // Echo 4
        chorus = realloc(chorus, (fileCount + 1) * sizeof(float));        // Chorus
        Equalizerf = realloc(Equalizerf, (fileCount + 1) * sizeof(float)); // EQ Frequency
        Equalizert = realloc(Equalizert, (fileCount + 1) * sizeof(float)); // EQ Type
        Equalizerw = realloc(Equalizerw, (fileCount + 1) * sizeof(float)); // EQ Width
        Equalizerg = realloc(Equalizerg, (fileCount + 1) * sizeof(float)); // EQ Gain

        Flanger = realloc(Flanger, (fileCount + 1) * sizeof(float));      // Flanger delay




        // Копируем оригинальный путь файла
        size_t len = strlen(filenameLine);
        fileNames[fileCount] = malloc(len + 5); // Оставляем место для расширения ".wav"
        snprintf(fileNames[fileCount], len + 5, "%s.wav", filenameLine);
        pitches[fileCount] = pitch;
        durations[fileCount] = duration;
        frequencies[fileCount] = freq_vibro; // Частота вибрато
        depths[fileCount] = depth_vibro;     // Глубина вибрато
        starts_fade_in[fileCount] = start_fade_in; // Time of fade-in
        durations_fade_in[fileCount] = duration_fade_in; // Duration of fade-in
        starts_fade_out[fileCount] = start_fade_out; // Time of fade-out
        durations_fade_out[fileCount] = duration_fade_out; // Duration of fade-out

        Echo1[fileCount] = echo1_val;           // Echo1
        Echo2[fileCount] = echo2_val;           // Echo2
        Echo3[fileCount] = echo3_val;           // Echo3
        Echo4[fileCount] = echo4_val;           // Echo4
        chorus[fileCount] = chorus_val;         // Chorus
        Equalizerf[fileCount] = equalizerf;     // Частота эквалайзера
        Equalizert[fileCount] = equalizert;     // Тип эквалайзера
        Equalizerw[fileCount] = equalizerw;     // Ширина эквалайзера
        Equalizerg[fileCount] = equalizerg;     // Уровень усиления

        Flanger[fileCount] = flange_val;        // Delay flanger


        fileCount++;
    }

    fclose(inputFile);

    if (fileCount == 0) {
        printf("No input files found in input.txt\n");
        return 1;
    }

    // Совмещаем файлы с использованием дополнительных параметров
    merge_wav_files(
        (const char**)fileNames, 
        pitches, 
        durations, 
        frequencies, 
        depths, 
        starts_fade_in, 
        durations_fade_in, 
        starts_fade_out, 
        durations_fade_out, 
        
        Echo1,
        Echo2,
        Echo3,
        Echo4,
        chorus,
        Equalizerf,
        Equalizert,
        Equalizerw,
        Equalizerg,

        Flanger,
 
        
        fileCount, 
        "output.wav"
    );

    if (fileCount > 0) {
        char srcPath[] = "output.wav"; // Исходный файл
        char destPath[] = "done/output.wav"; // Папка назначения

        // Попытка создать папку, если её ещё нет
        CreateDirectoryA("done", NULL);

        // КОПИРОВАНИЕ ФАЙЛА
        BOOL success = CopyFile(srcPath, destPath, FALSE); // Если TRUE, заменяет существующий файл
        if (!success) {
            DWORD errorCode = GetLastError();
            printf("error copy file  (%lu)\n", errorCode);
        } else {
            printf("file in 'done'.\n");
        }
    }

    // Освобождение памяти
    for (int i = 0; i < fileCount; i++) {
        free(fileNames[i]);
    }
    free(fileNames);
    free(pitches);
    free(durations);
    free(frequencies);
    free(depths);
    free(starts_fade_in);
    free(durations_fade_in);
    free(starts_fade_out);
    free(durations_fade_out);

    free(Echo1);
    free(Echo2);
    free(Echo3);
    free(Echo4);
    free(chorus);
    free(Equalizerf);
    free(Equalizert);
    free(Equalizerw);
    free(Equalizerg);

    free(Flanger);


    return 0;
}