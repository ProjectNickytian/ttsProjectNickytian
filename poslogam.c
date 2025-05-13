/**
 * @file poslogam.c
 *
 * @brief Программная реализация фонетической транскрипции русских слов латиницей.
 *
 * Этот модуль позволяет конвертировать русский текст в транскрипционную форму,
 * используя специальные обозначения для передачи звучания каждого звука.
 */

#include <stdio.h>      ///< Стандартные ввод/вывод
#include <stdlib.h>     ///< Функции выделения памяти и управления ею
#include <locale.h>     ///< Управление локализацией
#include <wchar.h>      ///< Работа с широкими символами
#include <wctype.h>     ///< Классификация символов широкого формата

// Максимальная длина строки
#define MAX_LEN 256

/**
 * Проверка, является ли символ гласной буквой русского алфавита.
 *
 * @param c Символ для проверки.
 * @return true, если символ является гласной буквой, иначе false.
 */
int isVowel(wchar_t c) {
    wchar_t vowels[] = L"аеёиоуыэюяАЕЁИОУЫЭЮЯ"; ///< Массив всех русских гласных
    for (int i = 0; vowels[i] != L'\0'; ++i) {
        if (c == vowels[i]) return 1;
    }
    return 0;
}

/**
 * Проверка, является ли символ согласной буквой русского алфавита.
 *
 * @param c Символ для проверки.
 * @return true, если символ является согласной буквой, иначе false.
 */
int isConsonant(wchar_t c) {
    if (((c >= L'а' && c <= L'я') || (c >= L'А' && c <= L'Я')) ||
        c == L'ё' || c == L'Ё') {           ///< Все русские буквы, включая Ё
        return !isVowel(c);                 ///< Это согласная, если не гласная
    }
    return 0;
}

/**
 * Проверка, является ли символ согласной, которая всегда звучит мягко.
 *
 * @param c Символ для проверки.
 * @return true, если символ всегда мягкий, иначе false.
 */
int isAlwaysSoft(wchar_t c) {
    wchar_t soft[] = L"йчщЙЧЩ";             ///< Всегда мягкие согласные
    for (int i = 0; soft[i] != L'\0'; ++i) {
        if (c == soft[i]) return 1;
    }
    return 0;
}

/**
 * Проверка, является ли символ согласной, которая всегда звучит твёрдо.
 *
 * @param c Символ для проверки.
 * @return true, если символ всегда твёрдый, иначе false.
 */
int isAlwaysHard(wchar_t c) {
    wchar_t hard[] = L"жшцЖШЦ";             ///< Всегда твёрдые согласные
    for (int i = 0; hard[i] != L'\0'; ++i) {
        if (c == hard[i]) return 1;
    }
    return 0;
}

/**
 * Печать фонетического представления символа в заданный поток вывода.
 *
 * @param c     Символ для печати.
 * @param soft  Признак мягкости (true — мягкая форма).
 * @param out   Поток вывода (обычно stdout или файл).
 */
void printPhoneme(wchar_t c, int soft, FILE* out) {
    wchar_t lower = towlower(c);              ///< Приводим символ к нижнему регистру

    // Выбор соответствующей фонетической транскрипции
    switch (lower) {
        case L'а': fwprintf(out, L"a"); break;
        case L'е':
            fwprintf(out, soft ? L"ie" : L"e"); ///< "е" становится "ie" в мягкой форме
            break;
        case L'ё':
            fwprintf(out, soft ? L"io" : L"o"); ///< "ё" становится "io" в мягкой форме
            break;
        case L'и': fwprintf(out, L"y"); break;
        case L'о': fwprintf(out, soft ? L"o" : L"o"); break;
        case L'у': fwprintf(out, soft ? L"u" : L"u"); break;
        case L'ы': fwprintf(out, L"j"); break;
        case L'э': fwprintf(out, L"e"); break;
        case L'ю':
            fwprintf(out, soft ? L"iu" : L"u"); ///< "ю" становится "iu" в мягкой форме
            break;
        case L'я':
            fwprintf(out, soft ? L"ia" : L"a"); ///< "я" становится "ia" в мягкой форме
            break;
        
        // Транскрипция согласных
        case L'б': fwprintf(out, soft ? L"b-" : L"b"); break;
        case L'в': fwprintf(out, soft ? L"v-" : L"v"); break;
        case L'г': fwprintf(out, soft ? L"g-" : L"g"); break;
        case L'д': fwprintf(out, soft ? L"d-" : L"d"); break;
        case L'ж': fwprintf(out, L"tch"); break;
        case L'з': fwprintf(out, soft ? L"z-" : L"z"); break;
        case L'к': fwprintf(out, soft ? L"k-" : L"k"); break;
        case L'л': fwprintf(out, soft ? L"l-" : L"l"); break;
        case L'м': fwprintf(out, soft ? L"m-" : L"m"); break;
        case L'н': fwprintf(out, soft ? L"n-" : L"n"); break;
        case L'п': fwprintf(out, soft ? L"p-" : L"p"); break;
        case L'р': fwprintf(out, soft ? L"r-" : L"r"); break;
        case L'с': fwprintf(out, soft ? L"s-" : L"s"); break;
        case L'т': fwprintf(out, soft ? L"t-" : L"t"); break;
        case L'ф': fwprintf(out, soft ? L"f-" : L"f"); break;
        case L'х': fwprintf(out, soft ? L"h-" : L"h"); break;
        case L'ц': fwprintf(out, L"c"); break;
        case L'ч': fwprintf(out, L"ch"); break;
        case L'ш': fwprintf(out, L"sh"); break;
        case L'щ': fwprintf(out, L"ch-"); break;
        case L'й': fwprintf(out, L"i"); break;
        case L'ь': fwprintf(out, L"-"); break;
    
        default:
            fputwc(c, out);                  ///< Непонятные символы оставляем как есть
            break;
    }
}

/**
 * Главная точка входа программы.
 *
 * @return Код завершения программы (0 — успешное завершение).
 */
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");       ///< Установка русской локализации

    FILE* input_file = _wfopen(L"input.txt", L"r, ccs=UTF-8");
    if (!input_file) {
        wprintf(L"Ошибка открытия файла input.txt.\n");
        return 1;
    }

    FILE* output_file = _wfopen(L"input2.txt", L"w, ccs=UTF-8");
    if (!output_file) {
        wprintf(L"Ошибка открытия файла input2.txt.\n");
        fclose(input_file);
        return 1;
    }

    wchar_t line[MAX_LEN];                   ///< Буфер для хранения одной строки
    int isStartOfWord = 1;                   ///< Признак начала нового слова

    while (fgetws(line, MAX_LEN, input_file)) {
        size_t len = wcslen(line);
        if (len > 0 && line[len - 1] == L'\n') {
            line[len - 1] = L'\0';           ///< Удаляем символ новой строки
            --len;
        }

        for (size_t i = 0; i < len; ++i) {
            wchar_t c = line[i];
            
            // Проверяем начало слова
            if (iswspace(c) || c == L'\0') {
                isStartOfWord = 1;           ///< Пробел или конец строки — новое слово начинается
                fputwc(c, output_file);      ///< Просто печатаем такие символы
                continue;
            } else {
                isStartOfWord = 0;           ///< Не начало слова
            }

            // Печатаем символы, отличные от букв и знаков мягкости-твёрдости
            if (!isConsonant(c) && !isVowel(c) &&
                c != L'ь' && c != L'Ь' && c != L'ъ' && c != L'Ъ') {
                fputwc(c, output_file);
                continue;
            }

            // Обрабатываем гласные
            if (isVowel(c)) {
                if (isStartOfWord) {
                    // Специальная обработка первой гласной буквы в слове
                    if (c == L'е') {
                        printPhoneme(L'е', 1, output_file); ///< Первая е → йэ
                    } else if (c == L'ё') {
                        printPhoneme(L'ё', 1, output_file); ///< Первая ё → йо
                    } else if (c == L'ю') {
                        printPhoneme(L'ю', 1, output_file); ///< Первая ю → йу
                    } else if (c == L'я') {
                        printPhoneme(L'я', 1, output_file); ///< Первая я → йа
                    } else {
                        printPhoneme(c, 0, output_file);    ///< Простые гласные
                    }
                } else {
                    // Повторение гласных внутри слова тоже учитывается
                    if (i > 0 && line[i - 1] == c) {
                        printPhoneme(c, 1, output_file);    ///< Вторая подряд такая же гласная → йотированная версия
                    } else {
                        printPhoneme(c, 0, output_file);    ///< Обычная гласная
                    }
                }
                continue;
            }

            // Пропускаем мягкие и твёрдые знаки
            if (c == L'ь' || c == L'Ь' || c == L'ъ' || c == L'Ъ') {
                continue;
            }

            // Определяем степень мягкости согласной
            int soft = 0;
            if (isAlwaysHard(c)) {
                soft = 0;                     ///< Постоянно твёрдая согласная
            } else if (isAlwaysSoft(c)) {
                soft = 1;                     ///< Постоянно мягкая согласная
            } else {
                wchar_t next = (i + 1 < len) ? line[i + 1] : L'\0';
                
                // Условия мягкости: мягкий знак впереди или следующая гласная особая
                if (next == L'ь' || next == L'Ь') {
                    soft = 1;
                } else if (next == L'я' || next == L'е' || next == L'ё' || next == L'ю' || next == L'и') {
                    soft = 1;
                } else if (next == L'\0' || next == L' ' || next == L'ъ' || next == L'Ъ') {
                    soft = 0;
                } else if (isConsonant(next) && !isAlwaysHard(next)) {
                    soft = 0;
                } else {
                    soft = 0;
                }
            }

            printPhoneme(c, soft, output_file); ///< Печатаем согласную с учётом её мягкости
        }
        fputwc(L'\n', output_file);          ///< Завершаем строку символом перевода строки
    }

    fclose(input_file);                      
    fclose(output_file);                   

    wprintf(L"Фонетический разбор завершён! Результат сохранён в input2.txt\n");

    return 0;
}