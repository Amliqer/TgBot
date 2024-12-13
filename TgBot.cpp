#include <iostream>
#include <tgbot/tgbot.h>
#include <vector>
#include <cstdlib> // Для rand() и srand()
#include <ctime>   // Для time()

using namespace std;
using namespace TgBot;

// Структура для хранения вопроса и вариантов ответов
struct Question {
    string photoUrl;
    vector<string> options;
    string rightAnswer;
};

string token("7724200502:AAH7WPCSx74rJuI3YO6BAFP7ejedqSHUJwA");

// Массив с вопросами
const vector<Question> questions = {
    {
        "https://commons.wikimedia.org/wiki/File:Flag_of_Germany.svg?uselang=ru",
        {"Germany", "France", "Italy"},
        "Germany"
    },
    {
        "https://commons.wikimedia.org/wiki/File:Flag_of_France.svg?uselang=ru",
        {"Germany", "France", "Spain"},
        "France"
    },
    {
        "https://commons.wikimedia.org/wiki/File:Flag_of_Italy.svg?uselang=ru",
        {"Italy", "Portugal", "Spain"},
        "Italy"
    },
    {
        "https://commons.wikimedia.org/wiki/File:Flag_of_Spain.svg?uselang=ru",
        {"Spain", "Mexico", "Argentina"},
        "Spain"
    },
    {
        "https://commons.wikimedia.org/wiki/File:Flag_of_Russia.svg?uselang=ru",
        {"Russia", "Ukraine", "Belarus"},
        "Russia"
    }
};

void sendRandomQuestionWithOptions(TgBot::Bot& bot, int64_t chatId) {
    // Генерируем случайный индекс
    srand(static_cast<unsigned int>(time(0)));
    int randomIndex = rand() % questions.size();

    // Получаем случайный вопрос
    const Question& question = questions[randomIndex];

    // Отправляем случайное изображение
    bot.getApi().sendPhoto(chatId, question.photoUrl);

    // Создаем клавиатуру с вариантами ответов
    InlineKeyboardMarkup::Ptr keyboard(new InlineKeyboardMarkup);
    vector<InlineKeyboardButton::Ptr> row0;

    // Добавляем варианты ответов в клавиатуру
    for (size_t i = 0; i < question.options.size(); ++i) {
        InlineKeyboardButton::Ptr option(new InlineKeyboardButton);
        option->text = question.options[i];
        option->callbackData = "answer_" + to_string(randomIndex) + "_" + to_string(i); // Уникальный callbackData
        row0.push_back(option);
    }

    keyboard->inlineKeyboard.push_back(row0);

    // Отправляем сообщение с вопросом и клавиатурой
    bot.getApi().sendMessage(chatId, "What country does this flag represent?", false, 0, keyboard);
}


int main() {
    TgBot::Bot bot(token);

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hello, this is quizzy, quiz bot. Here you can test your knowledge.");
        });

    bot.getEvents().onCommand("question", [&bot](TgBot::Message::Ptr message) {
        sendRandomQuestionWithOptions(bot, message->chat->id);
        });

    bot.getEvents().onCallbackQuery([&bot](CallbackQuery::Ptr query) {
        // Извлекаем индекс вопроса и выбранный вариант ответа
        size_t underscorePos = query->data.find('_');
        if (underscorePos != string::npos) {
            int questionIndex = stoi(query->data.substr(7, underscorePos - 7)); // Получаем индекс вопроса
            int answerIndex = stoi(query->data.substr(underscorePos + 1)); // Получаем индекс ответа
            string userAnswer = questions[questionIndex].options[answerIndex];
            string correctAnswer = questions[questionIndex].rightAnswer; 

            // Проверяем правильность ответа
            if (userAnswer == correctAnswer) {
                bot.getApi().sendMessage(query->message->chat->id, "Правильно!");
            }
            else {
                bot.getApi().sendMessage(query->message->chat->id, "Неправильно. Правильный ответ: " + correctAnswer);
            }
        }
        else {
            bot.getApi().sendMessage(query->message->chat->id, "Unknown option.");
        }
        });

    try {
        bot.getApi().deleteWebhook();
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            cout << "Long poll started\n";
            longPoll.start();
        }
    }
    catch (exception& e) {
        cout << "error:\n" << e.what();
    }
}

