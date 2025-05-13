/**
 * @file trnskrp.c
 * @brief Программная реализация разбиения текста на слоги и создание шаблона для последующей обработки
 *
 * Данная программа демонстрирует работу с текстом, проверку символов,
 * обработку специальных комбинаций букв и запись результатов в файл.
 */

#include <stdio.h>          ///< Стандартная библиотека ввода-вывода
#include <ctype.h>          ///< Библиотека для работы с типами символов
#include <string.h>         ///< Библиотека для работы со строками

// Настройки звучания без заметных эффектов
/** @defgroup SoundSettings Настройки звука
 * @{
 */
#define VELOCITY "1.0"               ///< Нормальная скорость воспроизведения
#define FREQ_VIBRO "0"               ///< Без вибрато
#define DEPTH_VIBRO "0.0"            ///< Полностью отключено вибрато
#define START_FADE_IN "0"            ///< Нет плавного нарастания громкости
#define DURATION_FADE_IN "0.0"       ///< Нулевое время нарастания
#define START_FADE_OUT "0"           ///< Нет плавного затухания громкости
#define DURATION_FADE_OUT "0.0"      ///< Нулевое время затухания

#define ECHO_FEEDBACK "0.0"          ///< Отключена обратная связь эхо
#define ECHO_GAIN_1 "0.0"            ///< Эхо первой задержки отсутствует
#define ECHO_DELAY "0"               ///< Отсутствие задержанного сигнала
#define ECHO_GAIN_2 "0.0"            ///< Второе эхо также выключено

#define CHORUS_RATE "0.0"            ///< Чоруса нет

#define EQ_FREQ "1000"               ///< Центральная частота эквалайзера остается неизменной
#define EQ_TYPE "flat"               ///< Плоская характеристика эквалайзера (без изменений частот)
#define EQ_WIDTH "1.0"               ///< Не влияет при flat-типе
#define EQ_GAIN "0"                  ///< Эквалайзер нейтральный, никаких усилений/ослаблений

#define FLANGER_DEPTH "0.0"          ///< Фленжер выключен
/** @}*/

/**
 * Проверяет, является ли заданный символ согласной буквой.
 *
 * @param[in] c Символ для проверки
 * @return 1, если символ является согласной буквой, иначе 0
 */
int isConsonant(char c) {
    c = tolower(c);
    return strchr("bcdfghiklmnpqrstvwxz", c) != NULL;
}

/**
 * Проверяет, является ли заданный символ гласной буквой.
 *
 * @param[in] c Символ для проверки
 * @return 1, если символ является гласной буквой, иначе 0
 */
int isVowel(char c) {
    c = tolower(c);
    return strchr("aejouy", c) != NULL;
}

/**
 * Проверяет наличие специфичных сочетаний букв ("ch", "sh" и др.).
 *
 * @param[in] str Строка для проверки
 * @return 1, если комбинация найдена, иначе 0
 */
int isSpecialCombination(const char *str) {
    return strcmp(str, "ch") == 0 || strcmp(str, "ch'") == 0 || strcmp(str, "tch") == 0 || strcmp(str, "sh") == 0;
}

/**
 * Записывает буфер в выходной файл с дополнительными параметрами.
 *
 * @param[out] file Выходной файл
 * @param[in] buffer Буфер с данными
 * @param[in] length Длина буфера
 */
void printBuffer(FILE *file, const char *buffer, int length) {
    fwrite(buffer, sizeof(char), length, file);
    fputc('\n', file);
    fputs("0\n", file);                   ///< Тон равен 0
    fprintf(file, "%s\n", VELOCITY);      ///< Скорость
    fprintf(file, "%s\n", FREQ_VIBRO);    ///< Частота вибрато
    fprintf(file, "%s\n", DEPTH_VIBRO);   ///< Глубина вибрато
    fprintf(file, "%s\n", START_FADE_IN); ///< Начало fade-in
    fprintf(file, "%s\n", DURATION_FADE_IN); ///< Длительность fade-in
    fprintf(file, "%s\n", START_FADE_OUT); ///< Начало fade-out
    fprintf(file, "%s\n", DURATION_FADE_OUT); ///< Длительность fade-out

    fprintf(file, "%s\n", ECHO_FEEDBACK); ///< Обратная связь эха
    fprintf(file, "%s\n", ECHO_GAIN_1);   ///< Задержка эха
    fprintf(file, "%s\n", ECHO_DELAY);    ///< Задержка эха
    fprintf(file, "%s\n", ECHO_GAIN_2);   ///< Обратная связь эха

    fprintf(file, "%s\n", CHORUS_RATE);   ///< Темп хорового эффекта

    fprintf(file, "%s\n", EQ_FREQ);       ///< Центральная частота эквалайзера
    fprintf(file, "%s\n", EQ_TYPE);       ///< Тип эквалайзера
    fprintf(file, "%s\n", EQ_WIDTH);      ///< Диапазон эквалайзера
    fprintf(file, "%s\n", EQ_GAIN);       ///< Усиливание эквалайзера

    fprintf(file, "%s\n", FLANGER_DEPTH); ///< Глубина фленжера
}

/**
 * Записывает специальный символ "_" в выходной файл.
 *
 * @param[out] file Выходной файл
 */
void printSpaceBuffer(FILE *file) {
    fputs("_\n", file);
    fputs("0\n", file);                   ///< Тон равен 0
    fprintf(file, "%s\n", VELOCITY);      ///< Скорость
    fprintf(file, "%s\n", FREQ_VIBRO);    ///< Частота вибрато
    fprintf(file, "%s\n", DEPTH_VIBRO);   ///< Глубина вибрато
    fprintf(file, "%s\n", START_FADE_IN); ///< Начало fade-in
    fprintf(file, "%s\n", DURATION_FADE_IN); ///< Длительность fade-in
    fprintf(file, "%s\n", START_FADE_OUT); ///< Начало fade-out
    fprintf(file, "%s\n", DURATION_FADE_OUT); ///< Длительность fade-out

    fprintf(file, "%s\n", ECHO_FEEDBACK); ///< Обратная связь эха
    fprintf(file, "%s\n", ECHO_GAIN_1);   ///< Задержка эха
    fprintf(file, "%s\n", ECHO_DELAY);    ///< Задержка эха
    fprintf(file, "%s\n", ECHO_GAIN_2);   ///< Обратная связь эха

    fprintf(file, "%s\n", CHORUS_RATE);   ///< Темп хорового эффекта

    fprintf(file, "%s\n", EQ_FREQ);       ///< Центральная частота эквалайзера
    fprintf(file, "%s\n", EQ_TYPE);       ///< Тип эквалайзера
    fprintf(file, "%s\n", EQ_WIDTH);      ///< Диапазон эквалайзера
    fprintf(file, "%s\n", EQ_GAIN);       ///< Усиливание эквалайзера

    fprintf(file, "%s\n", FLANGER_DEPTH); ///< Глубина фленжера
}

/**
 * Главная функция программы.
 *
 * Читает данные из файла `input2.txt`, обрабатывает их и записывает результат в `output.txt`.
 *
 * @return Код завершения программы (0 — успех)
 */
int main() {
    FILE *inFile = fopen("input2.txt", "r");    ///< Открываем входной файл
    FILE *outFile = fopen("output.txt", "w");   ///< Открываем выходной файл

    if (inFile == NULL || outFile == NULL) {
        perror("Ошибка открытия файлов");
        return 1;
    }

    char input[256];                            ///< Буфер для чтения строк
    while (fgets(input, sizeof(input), inFile)) {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';              ///< Удаляем символ новой строки
        }

        char buffer[8];                         ///< Буфер для хранения слогов
        int buf_len = 0;
        int space_count = 0;                    ///< Количество встреченных пробелов

        for (size_t i = 0; i < len; ) {
            char c = input[i++];

            ///< Игнорируем первые два пробела
            if (space_count < 2 && c == ' ') {
                ++space_count;
                continue;
            }

            ///< Другие непечатаемые символы пропускаем
            if (!(isalpha(c) || c == '-' || c == '_')) {
                continue;
            }

            if (c == ' ') {                     ///< Обработка пробела между словами
                if (buf_len > 0) {
                    printBuffer(outFile, buffer, buf_len); ///< выводим накопленный слог
                    buf_len = 0;
                }

                printSpaceBuffer(outFile);       ///< специальный символ разделения слов
                continue;
            }

            if (c == '-') {                     ///< Апостроф добавляется прямо в буфер
                buffer[buf_len++] = c;
                continue;
            }

            ///< Если накапливается больше одного символа
            if (buf_len == 0) {
                buffer[buf_len++] = c;
                continue;
            }

            ///< Формируем временный буфер для анализа
            char tempBuf[4] = {buffer[buf_len-1], c};
            tempBuf[2] = '\0';

            ///< Проверка особых комбинаций букв (например, "ch")
            if (isSpecialCombination(tempBuf)) {
                buffer[buf_len++] = c;
                continue;
            }

            ///< Обработка пары согласная+гласная
            if (isConsonant(buffer[0]) && isVowel(c)) {
                buffer[buf_len++] = c;
                printBuffer(outFile, buffer, buf_len); ///< выводим сочетание согласная-гласная
                buf_len = 0;
                continue;
            }

            ///< Специальные комбинации вида "ia", "io", "iu"
            if (buf_len >= 1 &&
                ((buffer[buf_len - 1] == 'i' && isVowel(c)) ||
                 (buffer[buf_len - 1] == 'a' && c == 'i') ||
                 (buffer[buf_len - 1] == 'o' && c == 'i'))) {
                buffer[buf_len++] = c;
                continue;
            }

            ///< Общее правило
            printBuffer(outFile, buffer, buf_len); ///< выводим текущий буфер
            buf_len = 0;
            buffer[buf_len++] = c;                 ///< формируем новый буфер
        }

        ///< Последний остаток буфера после цикла
        if (buf_len > 0) {
            printBuffer(outFile, buffer, buf_len);
        }
    }

    fclose(inFile);                               ///< Закрытие файлов
    fclose(outFile);

    return 0;
}