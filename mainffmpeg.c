/** 
 * @file mainffmpeg.c 
 * @brief Сборщик и процессор аудиофайлов с поддержкой различных эффектов. 
 * Программа предназначена для объединения и обработки набора аудиофайлов формата 
 * WAV с возможностью применения эффектов вроде смены тональности, наложения вибрато, 
 * плавного затухания и других эффектов с помощью FFmpeg. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h> 
#include <windows.h>
#include <math.h>

#define MAX_CMD_SIZE 1024

/** @brief Формирует командную строку для FFmpeg с заданными параметрами обработки аудиофайла. 
 *  Формируется полная команда для запуска FFmpeg с набором фильтров, позволяющих изменить 
 *  высоту тона, добавить эффекты вибрато, затухания, эха, хоруса, эквалайзера и фленджер. 
 *  @param[in] input Исходный аудиофайл.
 *  @param[out] output Финальный аудиофайл.
 *  @param[in] semitones Количество полутонов для изменения тональности. 
 *  @param[in] duration Общая длительность файла. 
 *  @param[in] freq_vibro Частота вибрато. 
 *  @param[in] depth_vibro Глубина вибрато. 
 *  @param[in] start_fade_in Время начала плавного нарастания громкости. 
 *  @param[in] duration_fade_in Длительность плавного нарастания громкости. 
 *  @param[in] start_fade_out Время начала плавного затухания громкости. 
 *  @param[in] duration_fade_out Длительность плавного затухания громкости. 
 *  @param[in] Echo1 Первый коэффициент эха. 
 *  @param[in] Echo2 Второй коэффициент эха. 
 *  @param[in] Echo3 Третий коэффициент эха. 
 *  @param[in] Echo4 Четвертый коэффициент эха. 
 *  @param[in] chorus Интенсивность эффекта хоруса. 
 *  @param[in] Equalizerf Центральная частота эквалайзера. 
 *  @param[in] Equalizert Тип эквалайзера. 
 *  @param[in] Equalizerw Ширина полосы пропускания эквалайзера. 
 *  @param[in] Equalizerg Усиление эквалайзера. 
 *  @param[in] Flanger Величина задержки для эффекта фленджер. 
 *  @return Строку с готовой командой для запуска FFmpeg. */
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

    //! Вычисляем коэффициент изменения частоты для изменения тональности
    double factor = pow(2.0, semitones / 12.0);

    //! Формируем базовую команду для FFmpeg
    snprintf(cmd, MAX_CMD_SIZE, 
        "ffmpeg -y -i \"%s\" ", input);

    //! Добавляем фильтры для изменения частоты и темпоральной характеристики звука
    snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
        "-af \"asetrate=44100*%.5f,atempo=1/%.5f", factor, factor);

    //! Если включены параметры вибрато, добавляем этот эффект
    if (freq_vibro > 0 && depth_vibro > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
            ",vibrato=f=%d:d=%.2f", freq_vibro, depth_vibro);
    }

    //! Если активированы параметры плавного нарастания громкости, добавляем их
    if (start_fade_in >= 0 && duration_fade_in > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
            ",afade=t=in:st=%.2f:d=%.2f", start_fade_in, duration_fade_in);
    }

    //! Если активированы параметры плавного затухания громкости, добавляем их
    if (start_fade_out >= 0 && duration_fade_out > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
            ",afade=t=out:st=%.2f:d=%.2f", start_fade_out, duration_fade_out);
    }

    //! Если настроены параметры эха, добавляем этот эффект
    if (Echo1 > 0 && Echo2 > 0 && Echo3 > 0 && Echo4 > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",aecho=%.2f:%.2f:%.2f:%.2f", Echo1, Echo2, Echo3, Echo4);
    }

    //! Если настроен эффект хоруса, добавляем его
    if (chorus > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",chorus=%.2f:0.7:60:0.4:0.25:2", chorus);
    }

    //! Если настроены параметры эквалайзера, добавляем их
    if (Equalizerf != 0 && Equalizert != 0 && Equalizerw != 0 && Equalizerg != 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",equalizer=f=%.2f:t=q:w=%.2f:g=%.2f",
            Equalizerf, Equalizert, Equalizerw, Equalizerg);
    }

    //! Если настроен эффект фленджер, добавляем его
    if (Flanger > 0) {
        snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd),
            ",flanger=delay=%.2f", Flanger);
    }

    //! Добавляем заключительный этап ресемплинга
    snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
        ",aresample=44100\" ");

    //! Устанавливаем требуемую длительность и имя выходного файла
    snprintf(cmd + strlen(cmd), MAX_CMD_SIZE - strlen(cmd), 
        "-t %.2f \"%s\"", duration, output);

    return cmd;
}

/** 
 * @brief Применяет созданные параметры и команду для обработки отдельного аудиофайла. 
 *  Запускает команду FFmpeg для конкретного файла, создавая промежуточный обработанный файл.
 *  @param[in] input Исходный аудиофайл. 
 *  @param[out] output Выходной аудиофайл. 
 *  @param[in] pitch Изменение тональности в полутонах. 
 *  @param[in] duration Требуемая длительность. 
 *  @param[in] freq_vibro Частота вибрато. 
 *  @param[in] depth_vibro Глубина вибрато. 
 *  @param[in] start_fade_in Время начала плавного нарастания громкости. 
 *  @param[in] duration_fade_in Длительность плавного нарастания громкости. 
 *  @param[in] start_fade_out Время начала плавного затухания громкости. 
 *  @param[in] duration_fade_out Длительность плавного затухания громкости. 
 *  @param[in] Echo1 Первый коэффициент эха. 
 *  @param[in] Echo2 Второй коэффициент эха. 
 *  @param[in] Echo3 Третий коэффициент эха. 
 *  @param[in] Echo4 Четвертый коэффициент эха. 
 *  @param[in] chorus Интенсивность эффекта хоруса. 
 *  @param[in] Equalizerf Центральная частота эквалайзера. 
 *  @param[in] Equalizert Тип эквалайзера. 
 *  @param[in] Equalizerw Ширина полосы пропускания эквалайзера. 
 *  @param[in] Equalizerg Усиление эквалайзера. 
 *  @param[in] Flanger Величина задержки для эффекта фленджер. */
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

    //! Формируем команду FFmpeg для текущего файла
    strcpy(cmd, create_ffmpeg_command(input, output, pitch, duration, freq_vibro, depth_vibro, start_fade_in, duration_fade_in, start_fade_out, duration_fade_out, Echo1, Echo2, Echo3, Echo4, chorus, Equalizerf, Equalizert, Equalizerw, Equalizerg, Flanger));

    //! Выводим команду для контроля
    printf("Processing command:\n%s\n", cmd);

    //! Выполняем команду через систему
    system(cmd);
}

/** 
 * @brief Процессор для объединения обработанных файлов в один аудиофайл. 
 * Обрабатывает каждый файл с заданными параметрами и затем объединяет полученные промежуточные файлы в один общий аудиофайл. 
 * @param[in] wavFiles Массив путей к исходным файлам. 
 * @param[in] pitches Массив индексов изменения тональности. 
 * @param[in] durations Массив длительностей для каждого файла. 
 * @param[in] frequencies Массив частот вибрато. 
 * @param[in] depths Массив глубин вибрато. 
 * @param[in] starts_fade_in Массив начальных точек плавного нарастания громкости. 
 * @param[in] durations_fade_in Массив длительностей плавного нарастания громкости.
 * @param[in] starts_fade_out Массив начальных точек плавного затухания громкости. 
 * @param[in] durations_fade_out Массив длительностей плавного затухания громкости. 
 * @param[in] Echo1 Массив коэффициентов первого эха. 
 * @param[in] Echo2 Массив коэффициентов второго эха. 
 * @param[in] Echo3 Массив коэффициентов третьего эха.
 * @param[in] Echo4 Массив коэффициентов четвёртого эха. 
 * @param[in] chorus Массив интенсивностей эффекта хоруса. 
 * @param[in] Equalizerf Массив центральных частот эквалайзера.
 * @param[in] Equalizert Массив типов эквалайзера. 
 * @param[in] Equalizerw Массив ширин эквалайзера. 
 * @param[in] Equalizerg Массив усилений эквалайзера. 
 * @param[in] Flanger Массив задержек для эффекта фленджер. 
 * @param[in] fileCount Количество файлов для обработки. 
 * @param[out] output Выходной файл, куда будет собран результат. */

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

    //! Массив для хранения уникальных временных файлов
    char** uniqueTmpFiles = malloc(fileCount * sizeof(char*));

    //! Проходим по каждому файлу и создаем уникальные временные файлы
    for (int i = 0; i < fileCount; i++) {
        char tmpFileName[128];
        snprintf(tmpFileName, sizeof(tmpFileName), "temp_modifier_%d_%s.wav", i, wavFiles[i]);
        uniqueTmpFiles[i] = strdup(tmpFileName);

        //! Обрабатываем отдельный файл с заданными параметрами
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

    //! Создаем файл-список для последующего объединения файлов
    FILE* listFile = fopen("file_list.txt", "w");
    if (!listFile) {
        printf("Error creating list file\n");
        goto cleanup;
    }

    //! Записываем в файл список временных файлов
    for (int i = 0; i < fileCount; i++) {
        fprintf(listFile, "file '%s'\n", uniqueTmpFiles[i]);
    }
    fclose(listFile);

    //! Формируем команду для объединения файлов через FFmpeg
    char mergeCmd[MAX_CMD_SIZE];
    snprintf(mergeCmd, sizeof(mergeCmd), "ffmpeg -y -f concat -safe 0 -i file_list.txt -c copy \"%s\"", output);
    printf("Final merge command:\n%s\n", mergeCmd);
    system(mergeCmd);

cleanup:
    //! Чистим временные файлы и освобождаем память
    remove("file_list.txt");
    for (int i = 0; i < fileCount; i++) {
        remove(uniqueTmpFiles[i]);
        free(uniqueTmpFiles[i]);
    }
    free(uniqueTmpFiles);

    printf("Files processed successfully into %s\n", output);
}
/** @brief Главная функция программы, выполняющая чтение параметров и обработку файлов.
 * Читает конфигурационные данные из файла "output.txt", проходит по каждому
 * указанному файлу, применяет нужные параметры * обработки и объединяет обработанные файлы в один общий файл.
 * @return Код возврата (0 — успешное завершение, другое — ошибка). */
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

        //@{
        //! Чтение и обработка строк с параметрами эквалайзера и фленджер.
        //! Преобразование строк в числовую форму.
        int pitch = atoi(pitchLine);
        float duration = atof(durationLine);
        int freq_vibro = atoi(frequencyLine); 
        float depth_vibro = atof(depthLine);        
        float start_fade_in = atof(startFadeInLine); 
        float duration_fade_in = atof(durationFadeInLine);
        float start_fade_out = atof(startFadeOutLine); 
        float duration_fade_out = atof(durationFadeOutLine); 

        float echo1_val = atof(Echo1Line); //!< Значение первого параметра эха.
        float echo2_val = atof(Echo2Line); //!< Значение второго параметра эха.
        float echo3_val = atof(Echo3Line); //!< Значение третьего параметра эха.
        float echo4_val = atof(Echo4Line); //!< Значение четвёртого параметра эха.
        float chorus_val = atof(chorusLine); //!< Значение параметра хоруса.
        float equalizerf = atof(EqualizerfLine); //!< Центральная частота эквалайзера.
        float equalizert = atof(EqualizertLine); //!< Тип эквалайзера.
        float equalizerw = atof(EqualizerwLine); //!< Ширина полосы эквалайзера.
        float equalizerg = atof(EqualizergLine); //!< Усиление эквалайзера.

        float flange_val = atof(FlangeLine); //!< Значение задержки для эффекта фленджер.
        //@}

        //! Ограничение изменения тональности в пределах ±36 полутонов.
        if (pitch < -36 || pitch > 36) {
            pitch = 0;
        }

        //! Увеличение размера массивов под очередную запись.
        fileNames = realloc(fileNames, (fileCount + 1) * sizeof(char*));
        pitches = realloc(pitches, (fileCount + 1) * sizeof(int));
        durations = realloc(durations, (fileCount + 1) * sizeof(float));
        frequencies = realloc(frequencies, (fileCount + 1) * sizeof(int)); //!< Частоты вибрато.
        depths = realloc(depths, (fileCount + 1) * sizeof(float));         //!< Глубины вибрато.
        starts_fade_in = realloc(starts_fade_in, (fileCount + 1) * sizeof(float)); //!< Начальные точки Fade-In.
        durations_fade_in = realloc(durations_fade_in, (fileCount + 1) * sizeof(float)); //!< Длительности Fade-In.
        starts_fade_out = realloc(starts_fade_out, (fileCount + 1) * sizeof(float)); //!< Начальные точки Fade-Out.
        durations_fade_out = realloc(durations_fade_out, (fileCount + 1) * sizeof(float)); //!< Длительности Fade-Out.

        Echo1 = realloc(Echo1, (fileCount + 1) * sizeof(float));          //!< Echo1.
        Echo2 = realloc(Echo2, (fileCount + 1) * sizeof(float));          //!< Echo2.
        Echo3 = realloc(Echo3, (fileCount + 1) * sizeof(float));          //!< Echo3.
        Echo4 = realloc(Echo4, (fileCount + 1) * sizeof(float));          //!< Echo4.
        chorus = realloc(chorus, (fileCount + 1) * sizeof(float));        //!< Intensity of chorus effect.
        Equalizerf = realloc(Equalizerf, (fileCount + 1) * sizeof(float)); //!< Эквалайзер (частота).
        Equalizert = realloc(Equalizert, (fileCount + 1) * sizeof(float)); //!< Эквалайзер (тип).
        Equalizerw = realloc(Equalizerw, (fileCount + 1) * sizeof(float)); //!< Эквалайзер (полоса пропускания).
        Equalizerg = realloc(Equalizerg, (fileCount + 1) * sizeof(float)); //!< Эквалайзер (усиление).

        Flanger = realloc(Flanger, (fileCount + 1) * sizeof(float));      //!< Delay value for flanger effect.

        //! Присваивание нового имени файла с форматом `.wav`
        size_t len = strlen(filenameLine);
        fileNames[fileCount] = malloc(len + 5); // Осталось место для расширения ".wav"
        snprintf(fileNames[fileCount], len + 5, "%s.wav", filenameLine);
        pitches[fileCount] = pitch;
        durations[fileCount] = duration;
        frequencies[fileCount] = freq_vibro; //!< Частота вибрато.
        depths[fileCount] = depth_vibro;     //!< Глубина вибрато.
        starts_fade_in[fileCount] = start_fade_in; //!< Start point of fade-in.
        durations_fade_in[fileCount] = duration_fade_in; //!< Fade-in duration.
        starts_fade_out[fileCount] = start_fade_out; //!< Start point of fade-out.
        durations_fade_out[fileCount] = duration_fade_out; //!< Fade-out duration.

        Echo1[fileCount] = echo1_val;           //!< Echo1 coefficient.
        Echo2[fileCount] = echo2_val;           //!< Echo2 coefficient.
        Echo3[fileCount] = echo3_val;           //!< Echo3 coefficient.
        Echo4[fileCount] = echo4_val;           //!< Echo4 coefficient.
        chorus[fileCount] = chorus_val;         //!< Chorus intensity.
        Equalizerf[fileCount] = equalizerf;     //!< Equilizer central frequency.
        Equalizert[fileCount] = equalizert;     //!< Equilizer type.
        Equalizerw[fileCount] = equalizerw;     //!< Equilizer bandwidth.
        Equalizerg[fileCount] = equalizerg;     //!< Equilizer gain.

        Flanger[fileCount] = flange_val;        //!< Flanger delay parameter.

        fileCount++; // Переходим к следующей итерации обработки
    }

    fclose(inputFile); // Закрытие открытого файла

    if (fileCount == 0) { // Нет записей для обработки
        printf("No input files found in input.txt\n");
        return 1;
    }

    //! Выполнение процедуры объединения файлов с результатами обработки
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

    if (fileCount > 0) { // Если были успешно обработаны файлы
        char srcPath[] = "output.wav"; //!< Источник для копирования.
        char destPath[] = "done/output.wav"; //!< Назначение (каталог назначения).

        //! Создаём папку "done", если она не существует
        CreateDirectoryA("done", NULL);

        //! Копируем итоговый файл в папку "done"
        BOOL success = CopyFile(srcPath, destPath, FALSE); // Если true, заменяется существующий файл
        if (!success) {
            DWORD errorCode = GetLastError(); //!< Получаем код последней ошибки.
            printf("error(copy file) (%lu)\n", errorCode);
        } else {
            printf("file in 'done'.\n");
        }
    }

    //! Освобождение памяти после завершения работы
    for (int i = 0; i < fileCount; i++) {
        free(fileNames[i]); //!< Освобождаем память под каждое имя файла.
    }
    free(fileNames); //!< Освобождаем основной массив.
    free(pitches); //!< Освобождаем массив тональностей.
    free(durations); //!< Освобождаем массив длительностей.
    free(frequencies); //!< Освобождаем массив частот вибрато.
    free(depths); //!< Освобождаем массив глубин вибрато.
    free(starts_fade_in); //!< Освобождаем массив начальных моментов Fade-In.
    free(durations_fade_in); //!< Освобождаем массив длительностей Fade-In.
    free(starts_fade_out); //!< Освобождаем массив начальных моментов Fade-Out.
    free(durations_fade_out); //!< Освобождаем массив длительностей Fade-Out.

    free(Echo1); //!< Освобождаем массив коэффициентов Echo1.
    free(Echo2); //!< Освобождаем массив коэффициентов Echo2.
    free(Echo3); //!< Освобождаем массив коэффициентов Echo3.
    free(Echo4); //!< Освобождаем массив коэффициентов Echo4.
    free(chorus); //!< Освобождаем массив интенсивностей хоруса.
    free(Equalizerf); //!< Освобождаем массив центральных частот эквалайзера.
    free(Equalizert); //!< Освобождаем массив типов эквалайзера.
    free(Equalizerw); //!< Освобождаем массив ширины эквалайзера.
    free(Equalizerg); //!< Освобождаем массив усилений эквалайзера.

    free(Flanger); //!< Освобождаем массив задержек для эффекта фленджер.

    return 0; 
}