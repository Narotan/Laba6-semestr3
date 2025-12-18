#include "db_manager.h"
#include <iostream>
#include <limits>
#include <string>

using namespace std;

void printMenu() {
  cout << "\nТрекер активности программиста (Лаб 6)\n";
  cout << "1. Проекты и общее время\n";
  cout << "2. Последние 10 сессий (JOIN)\n";
  cout << "3. Сессии по языку (WHERE)\n";
  cout << "4. Время по проектам (AGGREGATE)\n";
  cout << "5. Средняя длительность по разработчику (AVG)\n";
  cout << "6. Топ языков по времени (LIMIT)\n";
  cout << "7. Использование редакторов (WINDOW/HAVING)\n";
  cout << "8. Нагруженные дни (HAVING)\n";
  cout << "9. Лидерборд проектов разработчика (SUBQUERY)\n";
  cout << "10. Добавить новую сессию (INSERT)\n";
  cout << "0. Выход\n";
  cout << "Выбор: ";
}

int main(int argc, char *argv[]) {
  string connStr = "dbname=lab6 host=localhost";

  if (argc > 1) {
    connStr = argv[1];
  }

  DBManager db;
  cout << "Подключение к: " << connStr << "...\n";
  if (!db.connect(connStr)) {
    cerr << "Не удалось подключиться. Убедитесь, что база 'lab6' существует.\n";
    cerr << "Использование: ./lab6 \"dbname=lab6 user=youruser host=localhost\"\n";
    return 1;
  }

  int choice;
  while (true) {
    printMenu();
    if (!(cin >> choice)) {
      cin.clear();
      cin.ignore(10000, '\n');
      continue;
    }

    if (choice == 0)
      break;

    cin.ignore(10000, '\n'); 

    switch (choice) {
    case 1:
      db.listProjects();
      break;
    case 2:
      db.listRecentActivities();
      break;
    case 3: {
      string lang;
      int minMinutes;
      cout << "Язык программирования (например, Python): ";
      getline(cin, lang);
      cout << "Минут не меньше: ";
      cin >> minMinutes;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      db.findSessionsByLanguage(lang, minMinutes);
      break;
    }
    case 4:
      db.totalTimePerProject();
      break;
    case 5: {
      string nick;
      cout << "Ник разработчика (например, alice): ";
      getline(cin, nick);
      db.avgSessionLengthByDeveloper(nick);
      break;
    }
    case 6:
      db.topLanguages(5);
      break;
    case 7:
      db.editorUsageShare();
      break;
    case 8: {
      int minMinutes;
      cout << "Минут кода за день не меньше: ";
      cin >> minMinutes;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      db.busyDays(minMinutes);
      break;
    }
    case 9: {
      string nick;
      cout << "Ник разработчика: ";
      getline(cin, nick);
      db.projectLeaderboardForDev(nick);
      break;
    }
    case 10: {
      string project, dev, lang, editor, branch, os;
      int minutes;
      cout << "Проект: ";
      getline(cin, project);
      cout << "Разработчик (ник): ";
      getline(cin, dev);
      cout << "Язык: ";
      getline(cin, lang);
      cout << "Редактор: ";
      getline(cin, editor);
      cout << "Длительность (мин): ";
      cin >> minutes;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Бранч: ";
      getline(cin, branch);
      cout << "OS: ";
      getline(cin, os);
      db.createNewActivity(project, dev, lang, editor, minutes, branch, os);
      break;
    }
    default:
      cout << "Неверный выбор.\n";
    }
  }

  return 0;
}
